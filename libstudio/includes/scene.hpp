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

#ifndef __LIBSTUDIO_SCENE_HPP__
#define __LIBSTUDIO_SCENE_HPP__

#include "container.hpp"
#include "camera.hpp"

namespace studio
{
	class Scene : public Container
	{
		typedef std::shared_vector<Camera> Cameras;
		Cameras m_cameras;

		template <typename T1, typename T2>
		struct is_
		{
			static T1 marker();
			static char test(const T2&);
			static char (&test(...))[2];
			enum { val = (sizeof(test(marker())) == sizeof(char) ) };
		};

		template <bool is_camera>
		struct _if;
		template <>
		struct _if<true> { static inline Cameras& get(Scene& ref) { return ref.m_cameras; } };

		template <>
		struct _if<false> { static inline Renderables& get(Scene& ref) { return ref.m_children; } };

	public:
		template <typename T, typename... Args>
		std::shared_ptr<T> emplace_back(Args && ... args)
		{
			return _if< is_<T, Camera>::val >::get(*this).emplace_back<T>(std::forward<Args>(args)...);
		}

		void renderAllCameras() const
		{
			for (auto && cam : m_cameras)
				renderTo(cam.get());
		}

		void renderTo(const Camera* cam) const
		{
			Container::renderTo(cam, math::Matrix::identity());
		}
	};
}

#endif //__LIBSTUDIO_SCENE_HPP__