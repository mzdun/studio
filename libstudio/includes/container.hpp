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

#ifndef __LIBSTUDIO_CONTAINER_HPP__
#define __LIBSTUDIO_CONTAINER_HPP__

#include "renderable.hpp"
#include <memory>
#include <vector>

namespace studio
{
	class Container : public Renderable
	{
	protected:
		typedef std::vector<std::shared_ptr<Renderable>> Renderables;
		Renderables m_children;
	public:
		template <typename T, typename... Args>
		std::shared_ptr<T> emplace_back(Args && ... args)
		{
			auto tmp = std::make_shared<T>(std::forward(args)...);
			auto child = std::static_pointer_cast<Renderable>(tmp);

			auto size = m_children.size();
			m_children.push_back(child);
			if (size == m_children.size())
				return nullptr;

			return tmp;
		}

		void renderTo(const Camera* cam, const math::Matrix& parent) const override
		{
			math::Matrix accumulated = parent * localMatrix();
			for (auto && child : m_children)
				child->renderTo(cam, accumulated);
		}
	};
}

#endif //__LIBSTUDIO_CONTAINER_HPP__