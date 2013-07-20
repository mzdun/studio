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

#include "pch.h"
#include "block.hpp"
#include "triangle.hpp"

namespace studio
{
	static void make_rect(Block* b, const math::Vertex& tl, const math::Vertex& tr, const math::Vertex& br, const math::Vertex& bl)
	{
		b->add<Triangle>(tl, tr, br);
		b->add<Triangle>(br, bl, tl);
	}

	Block::Block(long double width, long double height, long double depth)
	{
		math::Vertex a(0, 0, 0), b(width, 0, 0), c(width, height, 0), d(0, height, 0),
			e(0, 0, depth), f(width, 0, depth), g(width, height, depth), h(0, height, depth);

		make_rect(this, d, c, b, a);
		make_rect(this, c, g, f, b);
		make_rect(this, h, g, c, d);
		make_rect(this, g, h, e, f);
		make_rect(this, h, d, a, e);
		make_rect(this, e, a, b, f);
	}
}
