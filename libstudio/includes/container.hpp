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

#ifndef __LIBSTUDIO_CONTAINER_HPP__
#define __LIBSTUDIO_CONTAINER_HPP__

#include "renderable.hpp"
#include "shared_vector.hpp"

namespace studio
{
	class Container : public Renderable
	{
	protected:
		typedef std::shared_vector<Renderable> Renderables;
		Renderables m_children;
	public:
		typedef Renderables::const_iterator const_iterator;

		template <typename T, typename... Args>
		std::shared_ptr<T> add(Args && ... args)
		{
			return m_children.emplace_back<T>(std::forward<Args>(args)...);
		}

		void renderTo(const ICamera* cam, const math::Matrix& parent, const Lights& lights) const override
		{
			math::Matrix accumulated = parent * localMatrix();
			for (auto && child : m_children)
				child->renderTo(cam, accumulated, lights);
		}
		MaterialPtr material() const { return nullptr; }

		const_iterator begin() const { return m_children.begin(); }
		const_iterator end() const { return m_children.end(); }
	};
}

#endif //__LIBSTUDIO_CONTAINER_HPP__