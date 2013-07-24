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

#ifndef __LIBSTUDIO_MATERIAL_HPP__
#define __LIBSTUDIO_MATERIAL_HPP__

#include "fundamentals.hpp"
#include "shared_vector.hpp"

namespace studio
{
	struct Material
	{
		virtual ~Material() {}
		virtual unsigned int color(/* position on the surface... */) const = 0;
	};

	typedef std::shared_ptr<Material> MaterialPtr;

	class SimpleMaterial : public Material
	{
		unsigned int m_color;
	public:
		SimpleMaterial(unsigned int color)
			: m_color(color)
		{}

		unsigned int color(/* position on the surface... */) const override { return m_color; }
	};
}

#endif //__LIBSTUDIO_MATERIAL_HPP__