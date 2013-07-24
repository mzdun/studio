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

#ifndef __LIBSTUDIO_SHADER_HPP__
#define __LIBSTUDIO_SHADER_HPP__

#include "fundamentals.hpp"
#include "light.hpp"
#include "material.hpp"

namespace studio
{
	struct Shader
	{
		virtual ~Shader() {}
		virtual unsigned char shade(const math::Point& pt) = 0;
	};

	class UniformShader : public Shader
	{
		unsigned int m_color;
	public:
		UniformShader(unsigned int color)
			: m_color(color)
		{}

		unsigned char shade(const math::Point& pt) override
		{
			return m_color;
		}
	};

	class LightsShader : public Shader
	{
		MaterialPtr m_material;
		LightsInfo m_info;

		struct ProjectedPoint
		{
			fixed y;
			fixed x;

			fixed x3;
			fixed y3;
			fixed z3;

			ProjectedPoint() {}
			ProjectedPoint(const math::Point& p, const math::Vertex& v)
				: x(p.x())
				, y(p.y())
				, x3(v.x())
				, y3(v.y())
				, z3(v.z())
			{
			}
			ProjectedPoint(const fixed& x, const fixed& y, const fixed& x3, const fixed& y3, const fixed& z3)
				: x(x)
				, y(y)
				, x3(x3)
				, y3(y3)
				, z3(z3)
			{}
		};

		struct Delta
		{
			fixed dy;
			fixed dx;

			fixed dx3;
			fixed dy3;
			fixed dz3;

			Delta() {}
			Delta(const ProjectedPoint& p0, const ProjectedPoint& p1)
				: dx(p1.x - p0.x)
				, dy(p1.y - p0.y)
				, dx3(p1.x3 - p0.x3)
				, dy3(p1.y3 - p0.y3)
				, dz3(p1.z3 - p0.z3)
			{
			}

			ProjectedPoint interpolate(const fixed& y, const ProjectedPoint& pt)
			{
				if (!dy) return pt;
				auto step = (y - pt.y) / dy;
				return { pt.x + step * dx, y, pt.x3 + step * dx3, pt.y3 + step * dy3, pt.z3 + step * dz3 };
			}

			ProjectedPoint interpolate(const ProjectedPoint& pt, const fixed& x)
			{
				if (!dx) return pt;
				auto step = (x - pt.x) / dx;
				return { x, pt.y + step * dy, pt.x3 + step * dx3, pt.y3 + step * dy3, pt.z3 + step * dz3 };
			}
		};

		ProjectedPoint m_func[3];
		Delta A, B, C;

		math::Vertex counterProject(const math::Point& pt);
	public:
		LightsShader(const MaterialPtr& material, LightsInfo && info, const math::Point& p0, const math::Point& p1, const math::Point& p2, const math::Vertex& v0, const math::Vertex& v1, const math::Vertex& v2);
		unsigned char shade(const math::Point& pt) override;
	};
}

#endif //__LIBSTUDIO_SHADER_HPP__