﻿/*
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
	struct PixelSelect;

	template <>
	struct PixelSelect<8> { typedef Grayscale type; };
	template <>
	struct PixelSelect<24> { typedef Color type; };

	template <size_t BPP>
	struct RawBitmap
	{
		typedef typename PixelSelect<BPP>::type Pixel;

		u8* m_pixels;
		int m_width;
		int m_height;

		RawBitmap(u8* pixels, int w, int h)
			: m_pixels(pixels)
			, m_width(w)
			, m_height(h)
		{
			static_assert(BPP % 8 == 0, "Only full-byte pixels allowed");
		}

		int bytesPerLine() const { return m_width * (BPP >> 3); }
		int stride() const { return ((bytesPerLine() + 3) >> 2) << 2; }
		void erase() { memset(m_pixels, 0xFF, stride() * m_height); }
		static inline u8 blendChannel(u8 over, u8 under, const fixed& brightness)
		{
			int value = cast<int>(brightness * over + (1 - brightness) * under);
			if (value < 0) value = 0;
			if (value > 0xFF) value = 0xFF;
			return (u8) value;
		}

		u8* getDst(int x, int y)
		{
			return m_pixels + y * stride() + x * (BPP >> 3);
		}

		void blend(int x, int y, const Pixel& color, const fixed& brightness)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			u8* dst = getDst(x, y);
			for (size_t i = 0; i < (BPP >> 3); i++)
			{
				*dst = blendChannel(color.channel(i), *dst, brightness); ++dst;
			}
		}
		void plot(int x, int y, const Pixel& color)
		{
			if (x < 0 || x >= m_width ||
				y < 0 || y >= m_height)
			{
				return;
			}
			u8* dst = getDst(x, y);
			for (size_t i = 0; i < (BPP >> 3); i++)
			{
				*dst++ = color.channel(i);
			}
		}
		void blend(int x, int y, const fixed& depth, const fixed& brightness)
		{
			blend(x, y, Pixel::white(), brightness);
		}
		void plot(int x, int y)
		{
			plot(x, y, Pixel::white());
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

		void line(const math::Point& start, const math::Point& stop, const fixed& startDepth, const fixed& stopDepth) override
		{
			auto pThis = static_cast<T*>(this);
			make_xwdrawer(*pThis).draw(tr(start), tr(stop), startDepth, stopDepth);
		}

		void text(const math::Point& pos, const wchar_t* _text, const fixed& depth) override
		{
		}

		void flood(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3) override
		{
		}

		void fill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3, Shader* shader) override
		{
		}

		Render getRenderType() const override { return m_renderType; }
		void setRenderType(Render renderType) { m_renderType = renderType; }

		inline math::Point tr(const math::Point& pt)
		{
			auto pThis = static_cast<T*>(this);
			return { pt.x() + pThis->m_width / 2, pt.y() + pThis->m_height / 2 };
		}

		inline math::Point revTr(const math::Point& pt)
		{
			auto pThis = static_cast<T*>(this);
			return { pt.x() - pThis->m_width / 2, pt.y() - pThis->m_height / 2 };
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

	template <typename T>
	class DepthMap
	{
		fixed* m_depth;
		fixed m_minSet;
		fixed m_maxSet;

	public:
		DepthMap(int w, int h)
			: m_depth(new fixed[w*h])
			, m_minSet(std::numeric_limits<long double>::max())
			, m_maxSet(std::numeric_limits<long double>::min())
		{
			fixed* ptr = m_depth;
			fixed* end = m_depth + w*h;
			while (ptr != end)
			{
				*ptr++ = fixed(std::numeric_limits<long double>::max());
			}
		}

		~DepthMap()
		{
			delete [] m_depth;
		}

		bool isAbove(int x, int y, const fixed& depth)
		{
			if (x < 0 || x >= static_cast<T*>(this)->m_width ||
				y < 0 || y >= static_cast<T*>(this)->m_height)
			{
				return true;
			}

			fixed * ptr = static_cast<T*>(this)->m_depth + y * static_cast<T*>(this)->m_width + x;
			if (*ptr > depth)
			{
				*ptr = depth;
				if (m_minSet > depth) m_minSet = depth;
				if (m_maxSet < depth) m_maxSet = depth;
				return true;
			}
			return false;
		}

		void saveDepths(const char* path)
		{
			PlatformBitmap<BitmapType::G8> depths(static_cast<T*>(this)->m_width, static_cast<T*>(this)->m_height);
			depths.erase();

			auto dz = m_maxSet - m_minSet;
			for (int y = 0; y < static_cast<T*>(this)->m_height; ++y)
			{
				auto line = m_depth + y * static_cast<T*>(this)->m_width;
				for (int x = 0; x < static_cast<T*>(this)->m_width; ++x)
				{
					auto z = *line++;
					if (z > m_maxSet || z < m_minSet)
						continue;

					depths.plot(x, y, Grayscale { cast<u8>(0x40 + (m_maxSet - z)*(255 - 0x40) / dz) });
				}
			}
			depths.save(path);
		}
	};

	struct GrayscaleDepthBitmap : public CanvasImpl<GrayscaleDepthBitmap>, public DepthMap<GrayscaleDepthBitmap>, public PlatformBitmap<BitmapType::G8>
	{
		GrayscaleDepthBitmap(int w, int h)
			: PlatformBitmap<BitmapType::G8>(w, h)
			, DepthMap<GrayscaleDepthBitmap>(w, h)
		{
			erase();
		}
		void blend(int x, int y, const fixed& depth, const fixed& brightness)
		{
			if (isAbove(x, y, depth))
				PlatformBitmap<BitmapType::G8>::blend(x, y, depth, brightness);
		}

		void floodLine(int y, int start, int stop, const fixed& startDepth, const fixed& stopDepth, Shader* shader);
		void floodFill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3, Shader* shader);
		void flood(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3) override
		{
			UniformShader black(Color::black());
			floodFill(p1, p2, p3, &black);
		}
		void fill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3, Shader* shader) override
		{
			floodFill(p1, p2, p3, shader);
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

	struct ColorDepthBitmap : public CanvasImpl<ColorDepthBitmap>, public DepthMap<ColorDepthBitmap>, public PlatformBitmap<BitmapType::RGB24>
	{
		ColorDepthBitmap(int w, int h)
			: PlatformBitmap<BitmapType::RGB24>(w, h)
			, DepthMap<ColorDepthBitmap>(w, h)
		{
			erase();
		}
		void blend(int x, int y, const fixed& depth, const fixed& brightness)
		{
			if (isAbove(x, y, depth))
				PlatformBitmap<BitmapType::RGB24>::blend(x, y, depth, brightness);
		}

		void floodLine(int y, int start, int stop, const fixed& startDepth, const fixed& stopDepth, Shader* shader);
		void floodFill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3, Shader* shader);
		void flood(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3) override
		{
			UniformShader black(Color::black());
			floodFill(p1, p2, p3, &black);
		}
		void fill(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3, Shader* shader) override
		{
			floodFill(p1, p2, p3, shader);
		}
	};
}

#endif //__LIBSTUDIO_BITMAP_HPP__