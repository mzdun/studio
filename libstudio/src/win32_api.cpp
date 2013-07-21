/*
* Copyright (C) 2013 Marcin Zdun
*
* Permission is hereby granted, free of charge, to any person
* obtaining a copy of this software and associated documentation
* files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy,
* modify, merge, publish, distribute, sublicense, and/or sell copies
* of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be
* included in all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
* BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
* ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
* CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include "pch.h"
#include "platform_api.hpp"

#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib, "Gdiplus.lib")
#pragma comment (lib, "Gdi32.lib")
#pragma comment (lib, "User32.lib")

namespace studio
{
	static int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
	{
		UINT  num = 0;          // number of image encoders
		UINT  size = 0;         // size of the image encoder array in bytes

		Gdiplus::ImageCodecInfo* pImageCodecInfo = NULL;

		Gdiplus::GetImageEncodersSize(&num, &size);
		if (size == 0)
			return -1;  // Failure

		pImageCodecInfo = (Gdiplus::ImageCodecInfo*) (malloc(size));
		if (pImageCodecInfo == NULL)
			return -1;  // Failure

		GetImageEncoders(num, size, pImageCodecInfo);

		for (UINT j = 0; j < num; ++j)
		{
			if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
			{
				*pClsid = pImageCodecInfo[j].Clsid;
				free(pImageCodecInfo);
				return j;  // Success
			}
		}

		free(pImageCodecInfo);
		return -1;  // Failure
	}

	template <BitmapType Type>
	struct PlatformBitmapCreator;

	template <>
	struct PlatformBitmapCreator<BitmapType::G8>
	{
		static void createBitmap(int w, int h, HBITMAP& hBmp, unsigned char*& pixels)
		{
			HDC hDC = CreateCompatibleDC(NULL);

			struct
			{
				BITMAPINFOHEADER hdr;
				RGBQUAD colors[1 << 8];
			} info = {};

			info.hdr.biSize = sizeof(BITMAPINFOHEADER);
			info.hdr.biWidth = w;
			info.hdr.biHeight = h;
			info.hdr.biBitCount = 8;
			info.hdr.biPlanes = 1;
			info.hdr.biCompression = 0;
			info.hdr.biSizeImage = w * h;

			BYTE gray = 0;
			for (auto && color : info.colors)
				color.rgbBlue = color.rgbGreen = color.rgbRed = gray++;

			hBmp = CreateDIBSection(hDC, (LPBITMAPINFO)&info, DIB_RGB_COLORS, (LPVOID*) &pixels, nullptr, 0);

			DeleteDC(hDC);
		}
	};

	template <>
	struct PlatformBitmapCreator<BitmapType::RGB24>
	{
		static void createBitmap(int w, int h, HBITMAP& hBmp, unsigned char*& pixels)
		{
			HDC hDC = CreateCompatibleDC(NULL);

			BITMAPINFOHEADER binfo = {};
			binfo.biSize = sizeof(BITMAPINFOHEADER);
			binfo.biWidth = w;
			binfo.biHeight = h;
			binfo.biBitCount = 24;
			binfo.biPlanes = 1;
			binfo.biCompression = 0;
			binfo.biSizeImage = (((w * 3 + 3) >> 2) << 2) * h;

			hBmp = CreateDIBSection(hDC, (LPBITMAPINFO)&binfo, DIB_RGB_COLORS, (LPVOID*) &pixels, nullptr, 0);

			DeleteDC(hDC);
		}
	};

	struct WindowsBitmapAPI : public PlatformBitmapAPI
	{
		HBITMAP m_hBitmap;
		unsigned char* m_pixels;
		WindowsBitmapAPI() : m_hBitmap(nullptr) {}
		~WindowsBitmapAPI()
		{
			if (m_hBitmap)
				DeleteObject(m_hBitmap);
		}

		unsigned char* getPixels() override { return m_pixels; }
		void save(const char* filename)
		{
			LPWSTR path = nullptr;
			WCHAR mime[100] = L"image/";
			int size = MultiByteToWideChar(CP_UTF8, 0, filename, -1, nullptr, 0);
			if (!size)
				return;
			path = (LPWSTR) malloc(size * sizeof(WCHAR));
			if (!path)
				return;

			size = MultiByteToWideChar(CP_UTF8, 0, filename, -1, path, size);
			if (!size)
			{
				free(path);
				return;
			}

			LPCWSTR dot = wcsrchr(path, L'.');
			if (dot)
				wcsncat(mime, dot + 1, (sizeof(mime) - sizeof(L"image/")) / sizeof(WCHAR));
			else
				wcscat(mime, L"png");

			CLSID clsid;
			GetEncoderClsid(mime, &clsid);
			Gdiplus::Bitmap(m_hBitmap, nullptr).Save(path, &clsid, NULL);

			free(path);
		}
	};

	static ULONG_PTR s_gdiplusToken;
	bool PlatformBitmapAPI::initAPI()
	{
		Gdiplus::GdiplusStartupInput gdiplusStartupInput;
		return Gdiplus::GdiplusStartup(&s_gdiplusToken, &gdiplusStartupInput, NULL) == Gdiplus::Ok;
	}

	void PlatformBitmapAPI::shutdownAPI()
	{
		Gdiplus::GdiplusShutdown(s_gdiplusToken);
	}

	template <BitmapType Type>
	static PlatformBitmapAPI* create(int w, int h)
	{
		WindowsBitmapAPI* api = new (std::nothrow) WindowsBitmapAPI();
		PlatformBitmapCreator<Type>::createBitmap(w, h, api->m_hBitmap, api->m_pixels);
		return api;
	}

	PlatformBitmapAPI* PlatformBitmapAPI::createBitmap(int w, int h, BitmapType type)
	{
		switch (type)
		{
		case BitmapType::RGB24: return create<BitmapType::RGB24>(w, h);
		case BitmapType::G8: return create<BitmapType::G8>(w, h);
		}
		return nullptr;
	}

}
