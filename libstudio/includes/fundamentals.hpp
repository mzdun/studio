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

#ifndef __LIBSTUDIO_FUNDAMENTALS_HPP__
#define __LIBSTUDIO_FUNDAMENTALS_HPP__

#include <cmath>

namespace studio
{
	namespace math
	{
		inline long double __abs(long double ld)
		{
			if (ld < 0) return -ld;
			return ld;
		}

		template <size_t width, size_t height>
		class MatrixBase
		{
		protected:
			long double m_data[width*height];
		public:
			enum
			{
				my_width = width,
				my_height = height
			};
			long double at(size_t x, size_t y) const { return m_data[x + y * width]; }
			long double& at(size_t x, size_t y) { return m_data[x + y * width]; }
		};

		template <size_t common, size_t width, size_t height>
		void matrix_multiply(MatrixBase<width, height>& out, const MatrixBase<common, height>& lhs, const MatrixBase<width, common>& rhs)
		{
			for (size_t w = 0; w < width; ++w)
			{
				for (size_t h = 0; h < height; ++h)
				{
					out.at(w, h) = 0;
					for (size_t c = 0; c < common; ++c)
					{
						out.at(w, h) += lhs.at(c, h) * rhs.at(w, c);
					}
				}
			}
		}

#define MATRIX_OPS(Type) \
	typedef Type my_t; \
	Type(const Type& rhs) \
	{ \
		for (size_t i = 0; i < sizeof(m_data) / sizeof(m_data[0]); ++i) \
			m_data[i] = rhs.m_data[i]; \
	} \
\
	Type(Type&& rhs) \
	{ \
		for (size_t i = 0; i < sizeof(m_data) / sizeof(m_data[0]); ++i) \
		{\
			m_data[i] = rhs.m_data[i]; \
			rhs.m_data[i] = 0; \
		}\
	} \
\
	Type& operator = (const Type& rhs) \
	{ \
		for (size_t i = 0; i < sizeof(m_data) / sizeof(m_data[0]); ++i) \
			m_data[i] = rhs.m_data[i]; \
		return *this; \
	} \
\
	Type& operator = (Type && rhs) \
	{ \
		for (size_t i = 0; i < sizeof(m_data) / sizeof(m_data[0]); ++i) \
		{\
			m_data[i] = rhs.m_data[i]; \
			rhs.m_data[i] = 0; \
		}\
		return *this; \
	} \
\
	my_t& set_at(size_t x, size_t y, long double val) { m_data[x + y * my_width] = val; return *this; }

#define MATRIX_PROP(name, pos) \
	long double name() const { return m_data[pos]; } \
	long double& name() { return m_data[pos]; } \
	my_t& set_ ## name(long double val) { m_data[pos] = val; return *this; }


		class Matrix : public MatrixBase<4, 4>
		{
		public:
			Matrix()
			{
				// identity
				for (size_t i = 0; i < sizeof(m_data) / sizeof(m_data[0]); ++i)
					m_data[i] = i % 15 ? 0 : 1;
			}

			MATRIX_OPS(Matrix)

			static Matrix identity() { return Matrix(); }
			static Matrix translate(long double dx, long double dy = 0, long double dz = 0)
			{
				return Matrix().set_at(3, 0, dx).set_at(3, 1, dy).set_at(3, 2, dz);
			}
			static Matrix scale(long double sx, long double sy = 0, long double sz = 0)
			{
				if (__abs(sy) < 0.00001) sy = sx;
				if (__abs(sz) < 0.00001) sz = sy;
				return Matrix().set_at(0, 0, sx).set_at(1, 1, sy).set_at(2, 2, sz);
			}
			static Matrix rotateX(long double theta)
			{
				long double cosTheta = std::cos(theta);
				long double sinTheta = std::sin(theta);
				return Matrix().set_at(2, 2, cosTheta).set_at(3, 2, -sinTheta).set_at(2, 3, sinTheta).set_at(3, 3, cosTheta);
			}
			static Matrix rotateY(long double theta)
			{
				long double cosTheta = std::cos(theta);
				long double sinTheta = std::sin(theta);
				return Matrix().set_at(0, 0, cosTheta).set_at(3, 0, sinTheta).set_at(0, 3, -sinTheta).set_at(3, 3, cosTheta);
			}
			static Matrix rotateZ(long double theta)
			{
				long double cosTheta = std::cos(theta);
				long double sinTheta = std::sin(theta);
				return Matrix().set_at(0, 0, cosTheta).set_at(1, 0, -sinTheta).set_at(0, 1, sinTheta).set_at(1, 1, cosTheta);
			}
		};

		class Vertex : public MatrixBase<1, 4>
		{
		public:
			Vertex()
			{
				for (size_t i = 0; i < my_height - 1; ++i)
					m_data[i] = 0;
				m_data[my_height - 1] = 1;
			}
			Vertex(long double x, long double y, long double z)
			{
				m_data[0] = x;
				m_data[1] = y;
				m_data[2] = z;
				m_data[3] = 1;
			}
			MATRIX_OPS(Vertex);
			MATRIX_PROP(x, 0);
			MATRIX_PROP(y, 1);
			MATRIX_PROP(z, 2);
		};

		class Vector : public MatrixBase<1, 4>
		{
		public:
			Vector()
			{
				for (size_t i = 0; i < my_height; ++i)
					m_data[i] = i == my_height - 1 ? 1 : 0;
			}
			Vector(long double i, long double j, long double k)
			{
				m_data[0] = i;
				m_data[1] = j;
				m_data[2] = k;
				m_data[3] = 1;
			}
			MATRIX_OPS(Vector);
			MATRIX_PROP(i, 0);
			MATRIX_PROP(j, 1);
			MATRIX_PROP(k, 2);

			long double length() const { return std::sqrt(lengthSquared()); }
			long double lengthSquared() const { return dotProduct(*this, *this); }

#define COORD(next, prev) (lhs.next() * rhs.prev() - rhs.next() * lhs.prev())
			static Vector crossProduct(const Vector& lhs, const Vector& rhs)
			{
				return { COORD(j, k), COORD(k, i), COORD(i, j) };
			}
#undef COORD

			static long double dotProduct(const Vector& lhs, const Vector& rhs)
			{
				return lhs.i() * rhs.i() + lhs.j() * rhs.j() + lhs.k() * rhs.k();
			}

			static long double cosTheta(const Vector& lhs, const Vector& rhs);
		};

		inline Vector operator - (const Vertex& lhs, const Vertex& rhs)
		{
			return { lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z() };
		}

		inline Vector operator* (const Matrix& lhs, const Vector& rhs)
		{
			Vector v;
			matrix_multiply(v, lhs, rhs);
			return v;
		}

		inline Vertex operator* (const Matrix& lhs, const Vertex& rhs)
		{
			Vertex v;
			matrix_multiply(v, lhs, rhs);
			return v;
		}

		inline Matrix operator* (const Matrix& lhs, const Matrix& rhs)
		{
			Matrix m;
			matrix_multiply(m, lhs, rhs);
			return m;
		}

	}
}

#endif //__LIBSTUDIO_FUNDAMENTALS_HPP__