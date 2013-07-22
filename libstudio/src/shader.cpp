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
#include "shader.hpp"
#include <algorithm>
#include <tuple>

namespace studio
{
	LinearShader::LinearShader(unsigned int color, const math::Point& p0, const math::Point& p1, const math::Point& p2, long double int0, long double int1, long double int2)
		: m_color(color)
	{
		m_func[0] = ShadedPoint(p0, int0);
		m_func[1] = ShadedPoint(p1, int1);
		m_func[2] = ShadedPoint(p2, int2);
		std::sort(m_func, m_func + 3, [](const ShadedPoint& lhs, const ShadedPoint& rhs) { return lhs.y > rhs.y; });

		A = Delta(m_func[0], m_func[2]);
		B = Delta(m_func[0], m_func[1]);
		C = Delta(m_func[1], m_func[2]);
	}

	long double LinearShader::interpolate(const math::Point& pt)
	{
		long double x0, x1, int0, int1;
		std::tie(x0, int0) = B.interpolate(pt.y(), m_func[0]);

		if (pt.y() > m_func[1].y)
		{
			// the point is in the upper "half"
			std::tie(x1, int1) = B.interpolate(pt.y(), m_func[0]);
		}
		else
		{
			// the point is in the lower "half"
			std::tie(x1, int1) = C.interpolate(pt.y(), m_func[1]);
		}

		return (pt.x() - x0) * (int1 - int0) / (x1 - x0);
	}

	unsigned char LinearShader::shade(const math::Point& pt)
	{
		auto intensity = interpolate(pt);

		// BW only...
		auto R = (unsigned char) (m_color & 0xFF);
		auto color = (unsigned int) (R * intensity);
		return color | (color << 8) | (color << 16) | (color << 24);
	}
}
