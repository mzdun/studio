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

#ifndef __LIBSTUDIO_CAMERA_HPP__
#define __LIBSTUDIO_CAMERA_HPP__

#include "renderable.hpp"
#include "canvas.hpp"
#include <memory>

namespace studio
{
	class Triangle;

	class Camera : public Renderable
	{
		long double m_eye;
		math::Vertex m_position;
		math::Vertex m_target;
		std::shared_ptr<Canvas> m_canvas;

	public:
		Camera(long double eye, const math::Vertex& position, const math::Vertex& target)
			: m_eye(eye)
			, m_position(position)
			, m_target(target)
		{}
		template <typename T, typename... Args>
		std::shared_ptr<T> create_canvas(Args&& ... args)
		{
			auto tmp = std::make_shared<T>(std::forward<Args>(args)...);
			m_canvas = tmp;
			return tmp;
		}

		void renderTo(const Camera* cam, const math::Matrix& parent) const override {}
		math::Vector normal() const { return m_target - m_position; }
		void transform(math::Vertex& pt, const math::Matrix& local) const;
		long double project(long double z, long double other) const
		{
			return other * m_eye / (m_eye + z);
		}
		math::Point project(const math::Vertex& pt) const
		{
			return { project(pt.z(), pt.x()), project(pt.z(), pt.y()) };
		}

		template <size_t len>
		void transformPoints(math::Vertex(&points)[len], const math::Matrix& local) const
		{
			for (auto && pt : points)
				transform(pt, local);
		}

		template <size_t len>
		void project(math::Point (&out)[len], math::Vertex (&points)[len]) const
		{
			auto* dest = out;
			for (auto && src : points)
				*dest++ = project(src);
		}

		virtual void render(const Triangle*, const math::Matrix&) const;
		virtual void renderLine(const math::Vertex& start, const math::Vertex& stop) const;

		math::Vertex position() const { return m_position; }
		math::Vertex target() const { return m_target; }
	};
}

#endif //__LIBSTUDIO_CAMERA_HPP__