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

#include "pch.h"
#include "camera.hpp"
#include "triangle.hpp"
#include <stdlib.h>
#include <memory.h>
#include <iomanip>

namespace studio
{
	void Camera::transform(math::Vertex& pt, const math::Matrix& local) const
	{
		pt = local * pt;

		pt.x() -= m_position.x();
		pt.y() -= m_position.y();
		pt.z() -= m_position.z();

		//TODO: apply pitch and yaw as resulting from m_target
	}

	static int round(long double ld)
	{
		if (ld < 0)
			return (int) (ld - 0.5);
		return (int) (ld + 0.5);
	}

	void Camera::render(const Triangle* triangle, const math::Matrix& local, const Lights& lights) const
	{
		Triangle::vertices_t vertices;
		{
			math::Vertex* pt = vertices;
			for (auto && vertex : triangle->vertices())
				*pt++ = vertex;
		}
		transformPoints(vertices, local);

#if 0
		auto cosTh = math::Vector::cosTheta(
			Triangle(vertices[0], vertices[1], vertices[2]).normal(),
			vertices[1] - m_position
			);
		{
			auto normal = Triangle(vertices[0], vertices[1], vertices[2]).normal();

			wchar_t buffer[300];
			swprintf_s(buffer, L"<%d, %d, %d>", round(normal.i()), round(normal.j()), round(normal.k()));

			auto midpoint = ((vertices[0] + vertices[2]) / 2);// +vertices[1]) / 2;
			auto textPos = midpoint + normal / (normal.length() / 400);
			m_canvas->line(project(midpoint), project(midpoint + normal / (normal.length() / 150)));

			//m_canvas->text(project(textPos), buffer);
			normal = vertices[1] - m_position;
			m_canvas->line(project(midpoint), { 0, 0 });
		}
#endif

		math::Point points [sizeof(vertices)/sizeof(vertices[0])];
		project(points, vertices);

		std::cout << "." << std::flush;

		if (m_canvas)
		switch (m_canvas->getRenderType())
		{
		case Render::Wireframe:
			m_canvas->flood(
			{ points[0], vertices[0].z() },
			{ points[1], vertices[1].z() },
			{ points[2], vertices[2].z() }
			);
			m_canvas->line(points[0], points[1], vertices[0].z(), vertices[1].z());
			m_canvas->line(points[1], points[2], vertices[1].z(), vertices[2].z());
			break;
		case Render::Solid:
			{
				LightsInfo info;
				info.m_normal = Triangle(vertices[0], vertices[1], vertices[2]).normal();

				for (auto && light : lights)
				{
					auto pos = light->position();
					transform(pos, math::Matrix::identity());
					info.m_lights.emplace_back(pos, light->power());
				}

#if 0
				auto midpoint = ((vertices[0] + vertices[2]) / 2);// +vertices[1]) / 2;
				auto intensity = info.getIntensity(midpoint);
				unsigned char shade = 0xFF;
				unsigned int color = (unsigned char)(intensity * shade);

				UniformShader white(color | (color << 8) | (color << 16) | (color << 24));
#else
				LightsShader white(triangle->material(), std::move(info), points[0], points[1], points[2], vertices[0], vertices[1], vertices[2]);
#endif
				m_canvas->fill(
				{ points[0], vertices[0].z() },
				{ points[1], vertices[1].z() },
				{ points[2], vertices[2].z() },
				&white
				);
			}
			break;
		}
	}

	void Camera::renderLine(const math::Vertex& start, const math::Vertex& stop, const Lights& lights) const
	{
		math::Vertex vertices [2] = {start, stop};
		transformPoints(vertices, math::Matrix::identity());
		math::Point points[sizeof(vertices) / sizeof(vertices[0])];
		project(points, vertices);
		if (m_canvas)
			m_canvas->line(points[0], points[1], vertices[0].z(), vertices[1].z());
	}
}
