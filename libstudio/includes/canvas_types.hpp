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

#ifndef __LIBSTUDIO_CANVAS_TYPES_HPP__
#define __LIBSTUDIO_CANVAS_TYPES_HPP__

#include "bitmap.hpp"
#include <string.h>

namespace studio
{
	template <typename BasicBitmap = GrayscaleBitmap>
	class SimpleCanvas : public BasicBitmap
	{
	public:
		SimpleCanvas(int w, int h) : BasicBitmap(w, h) {}
	};

	template <typename BasicBitmap>
	class StereoCanvasImpl : public StereoCanvas
	{
	protected:
		BasicBitmap m_leftEye;
		BasicBitmap m_rightEye;
	public:
		StereoCanvasImpl(int w, int h)
			: m_leftEye(w, h)
			, m_rightEye(w, h)
		{
		}

		void line(const math::Point& start, const math::Point& stop, long double startDepth, long double stopDepth, bool leftEye) override
		{
			(leftEye ? m_leftEye : m_rightEye).line(start, stop, startDepth, stopDepth);
		}

		void text(const math::Point& pos, const wchar_t* _text, long double depth, bool leftEye) override
		{
			(leftEye ? m_leftEye : m_rightEye).text(pos, _text, depth);
		}

		void flood(const PointWithDepth& p1, const PointWithDepth& p2, const PointWithDepth& p3, bool leftEye) override
		{
			(leftEye ? m_leftEye : m_rightEye).flood(p1, p2, p3);
		}

		Render getRenderType(bool leftEye) const override
		{
			return (leftEye ? m_leftEye : m_rightEye).getRenderType();
		}

		void setRenderType(Render renderType)
		{
			m_leftEye.setRenderType(renderType);
			m_rightEye.setRenderType(renderType);
		}
	};

	template <typename BasicBitmap = GrayscaleBitmap>
	class CyanMagentaCanvas : public StereoCanvasImpl<BasicBitmap>, public ColorBitmap
	{
	public:
		CyanMagentaCanvas(int w, int h)
			: StereoCanvasImpl<BasicBitmap>(w, h)
			, ColorBitmap(w, h)
		{
		}

		void setRenderType(Render renderType)
		{
			StereoCanvasImpl<BasicBitmap>::setRenderType(renderType);
		}

		void save(const char* path)
		{
			int stride = this->stride();
			int eyeStride = m_leftEye.stride();

			for (int y = 0; y < m_height; ++y)
			{
				unsigned char* dst = (unsigned char*) m_pixels + y * stride;
				unsigned char* lhs = (unsigned char*) m_leftEye.m_pixels + y * eyeStride;
				unsigned char* rhs = (unsigned char*) m_rightEye.m_pixels + y * eyeStride;
				for (int x = 0; x < m_width; x++)
				{
					if (*lhs)
					{
						if (*rhs)
						{
							dst[0] = *rhs / 2;
							dst[1] = *lhs;
							dst[2] = *rhs;
						}
						else
						{
							dst[0] = 0x00;
							dst[1] = *lhs;
							dst[2] = 0x00;
						}
					}
					else if (*rhs)
					{
						dst[0] = *rhs / 2;
						dst[1] = 0x00;
						dst[2] = *rhs;
					}
					else
					{
						dst[0] = 0x00;
						dst[1] = 0x00;
						dst[2] = 0x00;
					}
					dst += 3;
					lhs++;
					rhs++;
				}
			}

			ColorBitmap::save(path);
		}
	};

	template <typename BasicBitmap = GrayscaleBitmap>
	class SideBySideCanvas : public StereoCanvasImpl<BasicBitmap>, public GrayscaleBitmap
	{
	public:
		SideBySideCanvas(int w, int h)
			: StereoCanvasImpl<BasicBitmap>(w, h)
			, GrayscaleBitmap(w * 2, h)
		{
		}

		void setRenderType(Render renderType)
		{
			StereoCanvasImpl<BasicBitmap>::setRenderType(renderType);
		}

		void srcCopy(int x, int y, const BasicBitmap& eye)
		{
			int eyeX = 0;
			int eyeY = 0;

			if (x < 0)
			{
				eyeX = -x;
				x = 0;
			}
			if (y < 0)
			{
				eyeY = -y;
				y = 0;
			}

			if (x > m_width || y > m_height)
				return;
			if (eyeX > eye.m_width || eyeY > eye.m_height)
				return;

			int stride = this->stride();
			int eyeStride = eye.stride();

			int width = eye.m_width - eyeX;
			if (width > m_width - x)
				width = m_width - x;

			int lines = eye.m_height - eyeY;
			if (lines > m_height - y)
				lines = m_height - y;

			auto src = eye.m_pixels + eyeY * eyeStride + eyeX;
			auto dst = m_pixels + y * stride + x;

			for (int y = 0; y < lines; ++y)
			{
				memcpy(dst, src, width);
				src += eyeStride;
				dst += stride;
			}
		}

		void save(const char* path)
		{
			srcCopy(0, 0, m_leftEye);
			srcCopy(m_leftEye.m_width, 0, m_rightEye);
			GrayscaleBitmap::save(path);
		}
	};

}

#endif //__LIBSTUDIO_CANVAS_TYPES_HPP__