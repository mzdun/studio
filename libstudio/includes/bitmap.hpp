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

#ifndef __LIBSTUDIO_BITMAP_HPP__
#define __LIBSTUDIO_BITMAP_HPP__

#include "xwline.hpp"
#include "platform_api.hpp"
#include "canvas.hpp"

namespace studio
{

	template <size_t BPP>
	struct RawBitmap
	{
		unsigned char* m_pixels;
		int m_width;
		int m_height;

		RawBitmap(unsigned char* pixels, int w, int h)
			: m_pixels(pixels)
			, m_width(w)
			, m_height(h)
		{
			static_assert(BPP % 8 == 0, "Only full-byte pixels allowed");
		}

		int bytesPerLine() const { return m_width * (BPP >> 3); }
		int stride() const { return ((bytesPerLine() + 3) >> 2) << 2; }
		void erase() { memset(m_pixels, 0, stride() * m_height); }
		static inline unsigned char blendChannel(unsigned char over, unsigned char under, long double brightness)
		{
			int value = (int) (brightness * over + (1.0l - brightness) * under);
			if (value < 0) value = 0;
			if (value > 0xFF) value = 0xFF;
			return (unsigned char) value;
		}

		void blend(int x, int y, unsigned int color, long double brightness)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			unsigned char* dst = (unsigned char*) m_pixels + (m_height - y) * stride() + x * (BPP >> 3);
			unsigned char* src = (unsigned char*) &color;
			for (size_t i = 0; i < (BPP >> 3); i++)
			{
				*dst = blendChannel(*src++, *dst, brightness); ++dst;
			}
		}
		void plot(int x, int y, unsigned int color)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			unsigned char* dst = (unsigned char*) m_pixels + (m_height - y) * stride() + x * (BPP >> 3);
			unsigned char* src = (unsigned char*) &color;
			for (size_t i = 0; i < (BPP >> 3); i++)
			{
				*dst++ = *src++;
			}
		}
		void blend(int x, int y, long double brightness)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			unsigned char* dst = (unsigned char*) m_pixels + (m_height - y) * stride() + x * (BPP >> 3);
			for (size_t i = 0; i < (BPP >> 3); i++)
			{
				*dst = blendChannel(0xFF, *dst, brightness); ++dst;
			}
		}
		void plot(int x, int y)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			unsigned char* dst = (unsigned char*) m_pixels + (m_height - y) * stride() + x * (BPP >> 3);
			for (size_t i = 0; i < (BPP >> 3); i++)
			{
				*dst++ = 0xFF;
			}
		}
	};

	template <BitmapType Type>
	struct Bits;

	template <>
	struct Bits<BitmapType::G8> {
		enum { value = 8 };
	};

	template <>
	struct Bits<BitmapType::RGB24> {
		enum { value = 24 };
	};

	template <BitmapType Type>
	struct PlatformBitmap : RawBitmap<Bits<Type>::value>
	{
		PlatformBitmapAPI* m_bmpAPI;

		PlatformBitmap(int w, int h)
			: RawBitmap<Bits<Type>::value>(nullptr, w, h)
			, m_bmpAPI(PlatformBitmapAPI::createBitmap(w, h, Type))
		{
			m_pixels = m_bmpAPI->getPixels();
		}

		~PlatformBitmap()
		{
			delete m_bmpAPI;
		}

		void save(const char* path)
		{
			m_bmpAPI->save(path);
		}
	};

	struct GrayscaleBitmap : public Canvas, public PlatformBitmap<BitmapType::G8>
	{
		GrayscaleBitmap(int w, int h)
			: PlatformBitmap<BitmapType::G8>(w, h)
		{
			erase();
		}

		void line(const math::Point& start, const math::Point& stop) override
		{
			make_xwdrawer(*this).draw(tr(start), tr(stop));
		}

		void text(const math::Point& pos, const wchar_t* _text) override
		{
		}

		inline math::Point tr(const math::Point& pt)
		{
			return { pt.x() + m_width / 2, -pt.y() + m_height / 2 };
		}
	};

	struct ColorBitmap : public Canvas, public PlatformBitmap<BitmapType::RGB24>
	{
		ColorBitmap(int w, int h)
			: PlatformBitmap<BitmapType::RGB24>(w, h)
		{
			erase();
		}

		void line(const math::Point& start, const math::Point& stop) override
		{
			make_xwdrawer(*this).draw(tr(start), tr(stop));
		}

		void text(const math::Point& pos, const wchar_t* _text) override
		{
		}

		inline math::Point tr(const math::Point& pt)
		{
			return { pt.x() + m_width / 2, -pt.y() + m_height / 2 };
		}
	};
}

#endif //__LIBSTUDIO_BITMAP_HPP__