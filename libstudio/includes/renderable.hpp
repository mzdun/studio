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

#ifndef __LIBSTUDIO_RENDERABLE_HPP__
#define __LIBSTUDIO_RENDERABLE_HPP__

#include "fundamentals.hpp"
#include "light.hpp"

namespace studio
{
	struct ICamera;

	class Renderable
	{
		math::Matrix m_local;
	public:

		virtual ~Renderable() {}

		virtual void renderTo(const ICamera* cam, const math::Matrix& parent, const Lights& lights) const = 0;
		virtual math::Vector normal() const
		{
			static math::Vector def(1, 0, 0); // i-hat
			return def;
		}

		const math::Matrix& localMatrix() const { return m_local; }

		void resetMatrix()
		{
			m_local = math::Matrix::identity();
		}

		void translate(const fixed& dx, const fixed& dy = fixed(), const fixed& dz = fixed())
		{
			m_local = m_local * math::Matrix::translate(dx, dy, dz);
		}

		void scale(const fixed& sx, const fixed& sy = fixed(), const fixed& sz = fixed())
		{
			m_local = m_local * math::Matrix::scale(sx, sy, sz);
		}

		void rotateX(const fixed& theta)
		{
			m_local = m_local * math::Matrix::rotateX(theta);
		}

		void rotateY(const fixed& theta)
		{
			m_local = m_local * math::Matrix::rotateY(theta);
		}

		void rotateZ(const fixed& theta)
		{
			m_local = m_local * math::Matrix::rotateZ(theta);
		}
	};
}

#endif //__LIBSTUDIO_RENDERABLE_HPP__