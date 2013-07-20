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

namespace studio
{
	class Triangle;

	class Camera : public Renderable
	{
		math::Vertex m_position;
		math::Vertex m_target;

	public:
		Camera(const math::Vertex& position, const math::Vertex& target)
			: m_position(position)
			, m_target(target)
		{}
		void renderTo(const Camera* cam, const math::Matrix& parent) const override {}
		math::Vector normal() const { return m_target - m_position; }
		void transform(math::Vertex& pt, const math::Matrix& local) const;
		bool canSee(const math::Vertex& pt) const;

		template <size_t len>
		void transformPoints(math::Vertex(&points)[len], const math::Matrix& local) const
		{
			for (auto && pt : points)
				transform(pt, local);
		}

		template <size_t len>
		bool canSee(const math::Vertex(&points)[len]) const
		{
			// is at least one vertex visible?
			bool ret = false;
			for (auto && pt : points)
				ret |= canSee(pt);
			return ret;
		}

		void render(const Triangle*, const math::Matrix&) const;
	};
}

#endif //__LIBSTUDIO_CAMERA_HPP__