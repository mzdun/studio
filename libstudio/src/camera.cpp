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

	void Camera::render(const Triangle* triangle, const math::Matrix& local) const
	{
		Triangle::vertices_t vertices;
		{
			math::Vertex* pt = vertices;
			for (auto && vertex : triangle->vertices())
				*pt++ = vertex;
		}
		transformPoints(vertices, local);
	}

	void Camera::renderLine(const math::Vertex& start, const math::Vertex& stop) const
	{
		math::Vertex vertices [2] = {start, stop};
		transformPoints(vertices, math::Matrix::identity());
		math::Point points[sizeof(vertices) / sizeof(vertices[0])];
		project(points, vertices);
	}
}
