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

#ifndef __LIBSTUDIO_SHADER_HPP__
#define __LIBSTUDIO_SHADER_HPP__

#include "fundamentals.hpp"

namespace studio
{
	struct Shader
	{
		virtual ~Shader() {}
		virtual unsigned char shade(const math::Point& pt) = 0;
	};

	class UniformShader: public Shader
	{
		unsigned int m_color;
	public:
		UniformShader(unsigned int color)
			: m_color(color)
		{}

		unsigned char shade(const math::Point& pt) override
		{
			return m_color;
		}
	};
}

#endif //__LIBSTUDIO_SHADER_HPP__