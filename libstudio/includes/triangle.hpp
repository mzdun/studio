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

#ifndef __LIBSTUDIO_TRIANGLE_HPP__
#define __LIBSTUDIO_TRIANGLE_HPP__

#include "renderable.hpp"

namespace studio
{
	class Triangle : public Renderable
	{
	public:
		typedef math::Vertex vertices_t[3];

		Triangle(const math::Vertex& v1, const math::Vertex& v2, const math::Vertex& v3);
		void renderTo(const ICamera* cam, const math::Matrix& parent) const override;

		const vertices_t& vertices() const { return m_vertices; }
		math::Vector normal() const { return math::Vector::crossProduct(m_vertices[2] - m_vertices[1], m_vertices[0] - m_vertices[1]); }
	private:
		vertices_t m_vertices;
	};
}

#endif //__LIBSTUDIO_TRIANGLE_HPP__