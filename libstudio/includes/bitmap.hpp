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

#include <limits>

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

		unsigned char* getDst(int x, int y)
		{
			return (unsigned char*) m_pixels + (m_height - y) * stride() + x * (BPP >> 3);
		}

		void blend(int x, int y, unsigned int color, long double brightness)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			unsigned char* dst = getDst(x, y);
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
			unsigned char* dst = getDst(x, y);
			unsigned char* src = (unsigned char*) &color;
			for (size_t i = 0; i < (BPP >> 3); i++)
			{
				*dst++ = *src++;
			}
		}
		void blend(int x, int y, long double depth, long double brightness)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			unsigned char* dst = getDst(x, y);
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
			unsigned char* dst = getDst(x, y);
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

	template <typename T>
	struct CanvasImpl: Canvas
	{
		CanvasImpl()
			: m_renderType(Render::Wireframe)
		{
		}

		void line(const math::Point& start, const math::Point& stop, long double startDepth, long double stopDepth) override
		{
			auto pThis = static_cast<T*>(this);
			make_xwdrawer(*pThis).draw(tr(start), tr(stop), startDepth, stopDepth);
		}

		void text(const math::Point& pos, const wchar_t* _text, long double depth) override
		{
		}

		void flood(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3) override
		{
		}

		void fill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3) override
		{
		}

		Render getRenderType() const override { return m_renderType; }
		void setRenderType(Render renderType) { m_renderType = renderType; }

		inline math::Point tr(const math::Point& pt)
		{
			auto pThis = static_cast<T*>(this);
			return { pt.x() + pThis->m_width / 2, -pt.y() + pThis->m_height / 2 };
		}
	private:
		Render m_renderType;
	};

	struct GrayscaleBitmap : public CanvasImpl<GrayscaleBitmap>, public PlatformBitmap<BitmapType::G8>
	{
		GrayscaleBitmap(int w, int h)
			: PlatformBitmap<BitmapType::G8>(w, h)
		{
			erase();
		}
	};

	struct GrayscaleDepthBitmap : public CanvasImpl<GrayscaleDepthBitmap>, public PlatformBitmap<BitmapType::G8>
	{
		long double* m_depth;
		long double m_minSet;
		long double m_maxSet;

		GrayscaleDepthBitmap(int w, int h)
			: PlatformBitmap<BitmapType::G8>(w, h)
			, m_depth(new long double [w*h])
			, m_minSet(std::numeric_limits<long double>::max())
			, m_maxSet(std::numeric_limits<long double>::min())
		{
			erase();

			long double* ptr = m_depth;
			long double* end = m_depth + w*h;
			while (ptr != end)
			{
				*ptr++ = std::numeric_limits<long double>::max();
			}
		}

		~GrayscaleDepthBitmap()
		{
			delete [] m_depth;
		}

		bool isAbove(int x, int y, long double depth)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return true;
			}

			long double * ptr = m_depth + y * m_width + x;
			if (*ptr >= depth)
			{
				*ptr = depth;
				if (m_minSet > depth) m_minSet = depth;
				if (m_maxSet < depth) m_maxSet = depth;
				return true;
			}
			return false;
		}
		void blend(int x, int y, long double depth, long double brightness)
		{
			if (isAbove(x, y, depth))
				PlatformBitmap<BitmapType::G8>::blend(x, y, depth, brightness);
		}

		void floodLine(int y, int start, int stop, long double startDepth, long double stopDepth, unsigned int color);
		void floodFill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3, unsigned int color);
		void flood(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3) override
		{
			floodFill(p1, p2, p3, 0x000000);
		}
		void fill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3) override
		{
			floodFill(p1, p2, p3, 0xFFFFFF);
		}

		inline math::Point tr(const math::Point& pt)
		{
			return { pt.x() + m_width / 2, -pt.y() + m_height / 2 };
		}

		void saveDepths(const char* path)
		{
			PlatformBitmap<BitmapType::G8> depths(m_width, m_height);
			depths.erase();

			auto dz = m_maxSet - m_minSet;
			for (int y = 0; y < m_height; ++y)
			{
				auto line = m_depth + y * m_width;
				for (int x = 0; x < m_width; ++x)
				{
					auto z = *line++;
					if (z > m_maxSet || z < m_minSet)
						continue;

					depths.plot(x, y, (unsigned char) (0x40 + (m_maxSet - z)*(255 - 0x40) / dz));
				}
			}
			depths.save(path);
		}
	};

	struct ColorBitmap : public CanvasImpl<ColorBitmap>, public PlatformBitmap<BitmapType::RGB24>
	{
		ColorBitmap(int w, int h)
			: PlatformBitmap<BitmapType::RGB24>(w, h)
		{
			erase();
		}
	};
}

#endif //__LIBSTUDIO_BITMAP_HPP__