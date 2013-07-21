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

#ifndef __LIBSTUDIO_XWLINE_HPP__
#define __LIBSTUDIO_XWLINE_HPP__

#include "fundamentals.hpp"

namespace studio
{
	// based on a version from http://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm
	template <typename Bitmap>
	struct XWDrawer
	{
		Bitmap& ref;
		XWDrawer(Bitmap& ref) : ref(ref) {}

		inline void plot(long double x, long double y, long double depth, long double brighteness)
		{
			ref.blend((int) x, (int) y, depth, brighteness);
		}

		// math helpers
		static inline long double ipart(long double d) { return (long double) (long long) d; }
		static inline long double round(long double d) { return ipart(d + 0.5l); }
		static inline long double fpart(long double d) { return d - floor(d); }
		static inline long double rfpart(long double d) { return 1.0l - fpart(d); }
		static inline long double abs(long double d) { return d < 0 ? -d : d; }

		// main API
		void draw(const math::Point& start, const math::Point& stop, long double startDepth, long double stopDepth)
		{
			auto x0 = start.x();
			auto y0 = start.y();
			auto x1 = stop.x();
			auto y1 = stop.y();

			auto steep = abs(y1 - y0) > abs(x1 - x0);

			if (steep)
			{
				std::swap(x0, y0);
				std::swap(x1, y1);
			}

			if (x0 > x1)
			{
				std::swap(x0, x1);
				std::swap(y0, y1);
				std::swap(startDepth, stopDepth);
			}

			auto dx = x1 - x0;
			auto dy = y1 - y0;
			auto dz = stopDepth - startDepth;
			auto gradient = dy / dx;

			// handle first endpoint
			auto xend = round(x0);
			auto yend = y0 + gradient * (xend - x0);
			auto xgap = rfpart(x0 + 0.5);
			long long xpxl1 = (long long) xend;  //this will be used in the main loop
			auto ypxl1 = ipart(yend);

			if (steep)
			{
				plot(ypxl1, (long double) xpxl1, startDepth, rfpart(yend) * xgap);
				plot(ypxl1 + 1, (long double) xpxl1, startDepth, fpart(yend) * xgap);
			}
			else
			{
				plot((long double) xpxl1, ypxl1, startDepth, rfpart(yend) * xgap);
				plot((long double) xpxl1, ypxl1 + 1, startDepth, fpart(yend) * xgap);
			}

			auto intery = yend + gradient; // first y-intersection for the main loop

			// handle second endpoint

			xend = round(x1);
			yend = y1 + gradient * (xend - x1);
			xgap = fpart(x1 + 0.5);
			long long xpxl2 = (long long) xend; //this will be used in the main loop
			auto ypxl2 = ipart(yend);
			if (steep)
			{
				plot(ypxl2, (long double) xpxl2, stopDepth, rfpart(yend) * xgap);
				plot(ypxl2 + 1, (long double) xpxl2, stopDepth, fpart(yend) * xgap);
			}
			else
			{
				plot((long double) xpxl2, ypxl2, stopDepth, rfpart(yend) * xgap);
				plot((long double) xpxl2, ypxl2 + 1, stopDepth, fpart(yend) * xgap);
			}

			// main loop

			if (steep)
			{
				for (auto x = xpxl1 + 1; x <= xpxl2 - 1; x++)
				{
					auto depth = startDepth + (dz * (x - xpxl1)) / dx;
					plot(ipart(intery), (long double) x, depth, rfpart(intery));
					plot(ipart(intery) + 1, (long double) x, depth, fpart(intery));
					intery = intery + gradient;
				}
			}
			else
			{
				for (auto x = xpxl1 + 1; x <= xpxl2 - 1; x++)
				{
					auto depth = startDepth + (dz * (x - xpxl1)) / dx;
					plot((long double) x, ipart(intery), depth, rfpart(intery));
					plot((long double) x, ipart(intery) + 1, depth, fpart(intery));
					intery = intery + gradient;
				}
			}
		}
	};

	template <typename T>
	inline XWDrawer<T> make_xwdrawer(T& ref) { return XWDrawer<T>(ref); }
}

#endif //__LIBSTUDIO_XWLINE_HPP__