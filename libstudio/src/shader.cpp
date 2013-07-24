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
	LightsShader::LightsShader(const MaterialPtr& material, LightsInfo && info, const math::Point& p0, const math::Point& p1, const math::Point& p2, const math::Vertex& v0, const math::Vertex& v1, const math::Vertex& v2)
		: m_material(material)
		, m_info(std::move(info))
	{
		m_func[0] = ProjectedPoint(p0, v0);
		m_func[1] = ProjectedPoint(p1, v1);
		m_func[2] = ProjectedPoint(p2, v2);
		std::sort(m_func, m_func + 3, [](const ProjectedPoint& lhs, const ProjectedPoint& rhs) { return lhs.y > rhs.y; });

		A = Delta(m_func[0], m_func[2]);
		B = Delta(m_func[0], m_func[1]);
		C = Delta(m_func[1], m_func[2]);
	}

	math::Vertex LightsShader::counterProject(const math::Point& pt)
	{
		ProjectedPoint p1, p2;
		p1 = A.interpolate(pt.y(), m_func[0]);

		if (pt.y() > m_func[1].y)
		{
			// the point is in the upper "half"
			p2 = B.interpolate(pt.y(), m_func[0]);
		}
		else
		{
			// the point is in the lower "half"
			p2 = C.interpolate(pt.y(), m_func[1]);
		}

		auto p = Delta(p1, p2).interpolate(p1, pt.x());

		return {p.x3, p.y3, p.z3};
	}

	unsigned int LightsShader::shade(const math::Point& pt)
	{
		auto intensity = m_info.getIntensity(counterProject(pt));

		// BW only...
		unsigned int color = m_material ? m_material->color() : 0xFFFFFF;
		unsigned short B = cast<unsigned short>((unsigned char) (color & 0xFF) * intensity);
		unsigned short G = cast<unsigned short>((unsigned char) ((color >> 8) & 0xFF) * intensity);
		unsigned short R = cast<unsigned short>((unsigned char) ((color >> 16) & 0xFF) * intensity);
		if (B > 0xFF) B = 0xFF;
		if (G > 0xFF) G = 0xFF;
		if (R > 0xFF) R = 0xFF;
		color = (R << 16) | (G << 8) | R;
		return color;
	}
}
