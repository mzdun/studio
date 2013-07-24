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

	struct ICamera : public Renderable
	{
		virtual void render(const Triangle*, const math::Matrix&, const Lights& lights) const = 0;
		virtual void renderLine(const math::Vertex& start, const math::Vertex& stop, const Lights& lights) const = 0;
		void renderTo(const ICamera* cam, const math::Matrix& parent, const Lights& lights) const override {}
		MaterialPtr material() const override { return nullptr; }
	};

	class Camera : public ICamera
	{
		fixed m_eye;
		math::Vertex m_position;
		math::Vertex m_target;
		std::shared_ptr<Canvas> m_canvas;

	public:
		Camera(const fixed& eye, const math::Vertex& position, const math::Vertex& target)
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

		math::Vector normal() const { return m_target - m_position; }
		void transform(math::Vertex& pt, const math::Matrix& local) const;
		fixed project(const fixed& z, const fixed& other) const
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

		virtual void render(const Triangle*, const math::Matrix&, const Lights& lights) const;
		virtual void renderLine(const math::Vertex& start, const math::Vertex& stop, const Lights& lights) const;

		math::Vertex position() const { return m_position; }
		math::Vertex target() const { return m_target; }
	};

	class StereoCamera : public ICamera
	{
		Camera m_leftCam;
		Camera m_rightCam;
	public:
		StereoCamera(const fixed& eye, const math::Vertex& position, const math::Vertex& target)
			: m_leftCam(eye, position + math::Vertex(-25, 0, 0), target + math::Vertex(-25, 0, 0))
			, m_rightCam(eye, position + math::Vertex(25, 0, 0), target + math::Vertex(25, 0, 0))
		{
		}

		template <typename T, typename... Args>
		std::shared_ptr<T> create_canvas(Args && ... args)
		{
			auto ref = std::make_shared<T>(std::forward<Args>(args)...);
			m_leftCam.create_canvas<SingleEyeCanvas>(true, ref);
			m_rightCam.create_canvas<SingleEyeCanvas>(false, ref);
			return ref;
		}

		void render(const Triangle* triangle, const math::Matrix& local, const Lights& lights) const override
		{
			m_leftCam.render(triangle, local, lights);
			m_rightCam.render(triangle, local, lights);
		}

		void renderLine(const math::Vertex& start, const math::Vertex& stop, const Lights& lights) const override
		{
			m_leftCam.renderLine(start, stop, lights);
			m_rightCam.renderLine(start, stop, lights);
		}
	};
}

#endif //__LIBSTUDIO_CAMERA_HPP__