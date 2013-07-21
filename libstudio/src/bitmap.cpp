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
#include "bitmap.hpp"
#include <algorithm>

namespace studio
{
	struct SlopePart
	{
		long double v0, v1;
		long double dv;
		SlopePart(long double v0, long double v1)
			: v0(v0)
			, v1(v1)
			, dv(v1 - v0)
		{
		}
	};
	struct Slope
	{
		SlopePart y, x, z;

		Slope(long double y0, long double y1, long double x0, long double x1, long double z0, long double z1)
			: y(y0, y1)
			, x(x0, x1)
			, z(z0, z1)
		{
		}

		void calc(long double Y, long double& X, long double& Z)
		{
			auto dY = Y - y.v0;
			X = x.v0 + dY * x.dv / y.dv;
			Z = z.v0 + dY * z.dv / y.dv;
		}
	};

	void GrayscaleDepthBitmap::floodLine(int y, int start, int stop, long double startDepth, long double stopDepth, unsigned int color)
	{
		auto dz = stopDepth - startDepth;
		int dx = stop - start;

		for (int x = 0; x < dx; x++)
		{
			auto z = (long double)x / dx;
			if (isAbove(start + x, y, startDepth + dz * x / dx))
				plot(start + x, y, 0x000000);
		}
	}

	void GrayscaleDepthBitmap::flood(const PointWithDepth& _p1, const PointWithDepth& _p2, const PointWithDepth& _p3)
	{
		unsigned long col = (unsigned char)(rand() * 64 / RAND_MAX + (256 - 64)/2);
		col <<= 8;
		col <<= 16;

		PointWithDepth pts [] = { _p1, _p2, _p3 };
		pts[0].m_pos = tr(pts[0].m_pos);
		pts[1].m_pos = tr(pts[1].m_pos);
		pts[2].m_pos = tr(pts[2].m_pos);

		std::sort(pts, pts + 3, [](const PointWithDepth& lhs, const PointWithDepth& rhs) { return lhs.m_pos.y() < rhs.m_pos.y(); });

		Slope A(pts[0].m_pos.y(), pts[1].m_pos.y(), pts[0].m_pos.x(), pts[1].m_pos.x(), pts[0].m_depth, pts[1].m_depth);
		Slope B(pts[0].m_pos.y(), pts[2].m_pos.y(), pts[0].m_pos.x(), pts[2].m_pos.x(), pts[0].m_depth, pts[2].m_depth);
		Slope C(pts[1].m_pos.y(), pts[2].m_pos.y(), pts[1].m_pos.x(), pts[2].m_pos.x(), pts[1].m_depth, pts[2].m_depth);

		int y0 = (int) (pts[0].m_pos.y() + 1);
		int y1 = (int) (pts[1].m_pos.y() + 1);
		int y2 = (int) (pts[2].m_pos.y() + 1);

		for (int y = y0; y < y1; y++)
		{
			long double x0, x1, z0, z1;
			B.calc(y, x0, z0);
			A.calc(y, x1, z1);

			if (!(y % 10))
			{
			}

			if (x0 > x1)
			{
				std::swap(x0, x1);
				std::swap(z0, z1);
			}

			floodLine(y, (int) x0, (int) x1, z0, z1, col);
		}

		for (int y = y1; y < y2; y++)
		{
			long double x0, x1, z0, z1;
			B.calc(y, x0, z0);
			C.calc(y, x1, z1);

			if (x0 > x1)
			{
				std::swap(x0, x1);
				std::swap(z0, z1);
			}

			floodLine(y, (int) x0, (int) x1, z0, z1, col);
		}
	}
}
