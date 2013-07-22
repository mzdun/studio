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

		inline void plot(const fixed& x, const fixed& y, const fixed& depth, const fixed& brighteness)
		{
			ref.blend(cast<int>(x), cast<int>(y), depth, brighteness);
		}

		// math helpers
		static inline fixed ipart(const fixed& d) { return cast<long long>(d); }
		static inline fixed round(const fixed& d) { return ipart(d + 0.5l); }
		static inline fixed fpart(const fixed& d) { return d - ipart(d); }
		static inline fixed rfpart(const fixed& d) { return 1.0l - fpart(d); }
		static inline fixed abs(const fixed& d) { return d < 0 ? -d : d; }

		// main API
		void draw(const math::Point& start, const math::Point& stop, const fixed& _startDepth, const fixed& _stopDepth)
		{
			auto x0 = start.x();
			auto y0 = start.y();
			auto x1 = stop.x();
			auto y1 = stop.y();
			auto startDepth = _startDepth;
			auto stopDepth = _stopDepth;

			auto steep = abs(y1 - y0) > abs(x1 - x0);

			if (steep)
			{
				swap(x0, y0);
				swap(x1, y1);
			}

			if (x0 > x1)
			{
				swap(x0, x1);
				swap(y0, y1);
				swap(startDepth, stopDepth);
			}

			auto dx = x1 - x0;
			auto dy = y1 - y0;
			auto dz = stopDepth - startDepth;
			auto gradient = dy / dx;

			// handle first endpoint
			auto xend = round(x0);
			auto yend = y0 + gradient * (xend - x0);
			auto xgap = rfpart(x0 + 0.5);
			auto xpxl1 = cast<long long>(xend);  //this will be used in the main loop
			auto ypxl1 = ipart(yend);

			if (steep)
			{
				plot(ypxl1, xpxl1, startDepth, rfpart(yend) * xgap);
				plot(ypxl1 + 1, xpxl1, startDepth, fpart(yend) * xgap);
			}
			else
			{
				plot(xpxl1, ypxl1, startDepth, rfpart(yend) * xgap);
				plot(xpxl1, ypxl1 + 1, startDepth, fpart(yend) * xgap);
			}

			auto intery = yend + gradient; // first y-intersection for the main loop

			// handle second endpoint

			xend = round(x1);
			yend = y1 + gradient * (xend - x1);
			xgap = fpart(x1 + 0.5);
			auto xpxl2 = cast<long long>(xend); //this will be used in the main loop
			auto ypxl2 = ipart(yend);
			if (steep)
			{
				plot(ypxl2, xpxl2, stopDepth, rfpart(yend) * xgap);
				plot(ypxl2 + 1, xpxl2, stopDepth, fpart(yend) * xgap);
			}
			else
			{
				plot(xpxl2, ypxl2, stopDepth, rfpart(yend) * xgap);
				plot(xpxl2, ypxl2 + 1, stopDepth, fpart(yend) * xgap);
			}

			// main loop

			if (steep)
			{
				for (auto x = xpxl1 + 1; x <= xpxl2 - 1; x++)
				{
					auto depth = startDepth + (dz * (x - xpxl1)) / dx;
					plot(ipart(intery), x, depth, rfpart(intery));
					plot(ipart(intery) + 1, x, depth, fpart(intery));
					intery = intery + gradient;
				}
			}
			else
			{
				for (auto x = xpxl1 + 1; x <= xpxl2 - 1; x++)
				{
					auto depth = startDepth + (dz * (x - xpxl1)) / dx;
					plot(x, ipart(intery), depth, rfpart(intery));
					plot(x, ipart(intery) + 1, depth, fpart(intery));
					intery = intery + gradient;
				}
			}
		}
	};

	template <typename T>
	inline XWDrawer<T> make_xwdrawer(T& ref) { return XWDrawer<T>(ref); }
}

#endif //__LIBSTUDIO_XWLINE_HPP__