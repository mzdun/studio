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
#include <tuple>

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
			blend(x, y, Pixel::black(), brightness);
		}
		void plot(int x, int y)
		{
			plot(x, y, Pixel::black());
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

		inline math::Point tr(const math::Point& pt) const
		{
			auto pThis = static_cast<const T*>(this);
			return { pt.x() + pThis->m_width / 2, pt.y() + pThis->m_height / 2 };
		}

		inline math::Point revTr(const math::Point& pt) const
		{
			auto pThis = static_cast<const T*>(this);
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

		const fixed& getDepth(int x, int y) const { return m_depth[y * static_cast<const T*>(this)->m_width + x]; }

		bool isInside(int x, int y) const
		{
			return
				x >= 0 && x < static_cast<const T*>(this)->m_width &&
				y >= 0 && y < static_cast<const T*>(this)->m_height;
		}

		struct DepthPlotter
		{
			template <typename S>
			struct Slope
			{
				S v0;
				S v1;
				S dv;

				Slope() {}
				Slope(const S& p0, const S& p1)
					: v0(p0)
					, v1(p1)
					, dv(p1 - p0)
				{
				}
				S interpolate(int x, int dx)
				{
					return v0 + x * dv / dx;
				}
			};

			const DepthMap& ref;
			bool steep;
			Slope<int> x;
			Slope<int> y;
			Slope<fixed> z;
			DepthPlotter(const DepthMap& ref, int x, int y, fixed z, int X, int Y, fixed Z)
				: ref(ref)
				, x(x, X)
				, y(y, Y)
				, z(z, Z)
			{
				steep = std::abs(this->y.dv) > std::abs(this->x.dv);
				if (steep)
					std::swap(this->x, this->y);
			}

			const fixed& depth(int x, int y) const
			{
				if (steep) return ref.getDepth(y, x);
				return ref.getDepth(x, y);
			}

			bool isInside(int x, int y) const
			{
				if (steep) return ref.isInside(y, x);
				return ref.isInside(x, y);
			}

			std::pair<int, fixed> interpolate(int _x)
			{
				return std::make_pair(y.interpolate(_x, x.dv), z.interpolate(_x, x.dv));
			}
		};

		bool isBlockedFrom(int x, int y, int X, int Y, fixed Z)
		{
			DepthPlotter dp{ *this, x, y, getDepth(x, y), X, Y, Z };

			for (int _x = 0; _x < dp.x.dv; ++_x)
			{
				int x = dp.x.v0 + _x;
				int y;
				fixed z;
				std::tie(y, z) = dp.interpolate(_x);
				if (dp.isInside(x, y) && z < dp.depth(x, y))
					return true;
			}
			return false;
		}

		std::shared_ptr<PlatformBitmap<BitmapType::G8>> calcShadow(const math::Point& light, const fixed& Z) const
		{
			auto pT = static_cast<const T*>(this);
			auto black = Grayscale::black();
			math::Point tr = pT->tr(light);
			auto X = cast<int>(tr.x());
			auto Y = cast<int>(tr.y());

			auto shadow = std::make_shared<GrayscaleBitmap>(pT->m_width, pT->m_height);
			shadow->erase();
			auto depths = [&](const math::Point& test) -> bool {
				auto pt = pT->tr(test);
				DepthPlotter dp{ *this, cast<int>(pt.x()), cast<int>(pt.y()), getDepth(cast<int>(pt.x()), cast<int>(pt.y())), X, Y, Z };

				Grayscale end = Grayscale::white();
				for (int _x = 0; _x < dp.x.dv; ++_x)
				{
					int x = dp.x.v0 + _x;
					int y;
					fixed z;
					std::tie(y, z) = dp.interpolate(_x);
					if (dp.isInside(x, y))
					{
						auto z1 = dp.depth(x, y);
						if (z > z1)
							return false;
					}
					else
						return true;
				}
				return true;
			};

			auto dx = pT->m_width / 2;
			auto dy = pT->m_height / 2;
			for (int y = 0; y < pT->m_height; y++)
			{
				for (int x = 0; x < pT->m_width; x++)
				{
					if (!depths({ x - dx, y - dy }))
					{
						shadow->plot(x, y, Grayscale::black());
						//pT->blend(x, y, Color::black(), fixed(82)/1000);
					}
				}
			}
			return shadow;
		}

		void applyShadow(const PlatformBitmap<BitmapType::G8>* shadow)
		{
			auto sh = shadow->m_pixels;
			auto pT = static_cast<T*>(this);
			for (int y = 0; y < pT->m_height; y++)
			{
				auto src = sh + y * shadow->stride();
				for (int x = 0; x < pT->m_width; x++)
				{
					auto c = *src++;
					if (c == 0xFF)
						continue;
					c = 0xFF - c;
					pT->blend(x, y, Color::black(), fixed(82) * c / (1000*0xFF));
				}
			}
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
		void blend(int x, int y, const Color& color, const fixed& brightness)
		{
			PlatformBitmap<BitmapType::RGB24>::blend(x, y, color, brightness);
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