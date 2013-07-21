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

#ifndef __LIBSTUDIO_CANVAS_HPP__
#define __LIBSTUDIO_CANVAS_HPP__

#include "fundamentals.hpp"

namespace studio
{
	class Canvas
	{
	public:
		virtual ~Canvas(){}
		virtual void line(const math::Point& start, const math::Point& stop, long double startDepth, long double stopDepth) = 0;
		virtual void text(const math::Point& pos, const wchar_t* _text, long double depth) = 0;
	};

	struct StereoCanvas
	{
		virtual ~StereoCanvas() {}
		virtual void line(const math::Point& start, const math::Point& stop, long double startDepth, long double stopDepth, bool leftEye) = 0;
		virtual void text(const math::Point& pos, const wchar_t* _text, long double depth, bool leftEye) = 0;
	};

	class SingleEyeCanvas : public Canvas
	{
		bool m_leftEye;
		std::shared_ptr<StereoCanvas> m_ref;
	public:
		SingleEyeCanvas(bool leftEye, const std::shared_ptr<StereoCanvas>& ref)
			: m_leftEye(leftEye)
			, m_ref(ref)
		{
		}

		void line(const math::Point& start, const math::Point& stop, long double startDepth, long double stopDepth) override
		{
			m_ref->line(start, stop, startDepth, stopDepth, m_leftEye);
		}

		void text(const math::Point& pos, const wchar_t* _text, long double depth) override
		{
			m_ref->text(pos, _text, depth, m_leftEye);
		}
	};

}

#endif //__LIBSTUDIO_CANVAS_HPP__