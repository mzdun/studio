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
		static const long double PI = 3.14159265358979323846264338327950288419716939937510L;

#define PROXY_OP_M(op) \
	fixed& operator op(const fixed& rhs) { v op rhs.v; return *this; } \
	fixed& operator op(long double rhs) { v op rhs; return *this; } \
	fixed& operator op(double rhs) { v op rhs; return *this; } \
	fixed& operator op(float rhs) { v op rhs; return *this; } \
	fixed& operator op(long long rhs) { v op rhs; return *this; } \
	fixed& operator op(long rhs) { v op rhs; return *this; } \
	fixed& operator op(int rhs) { v op rhs; return *this; } \
	fixed& operator op(unsigned long long rhs) { v op rhs; return *this; } \
	fixed& operator op(unsigned long rhs) { v op rhs; return *this; } \
	fixed& operator op(unsigned int rhs) { v op rhs; return *this; }
#define PROXY_OP_HELPER(op, type) \
	inline fixed operator op(type lhs, const fixed& rhs) { return fixed(lhs op rhs.v); } \
	inline fixed operator op(const fixed& lhs, type rhs) { return fixed(lhs.v op rhs); }

#define PROXY_OP(op) \
	inline fixed operator op(const fixed& lhs, const fixed& rhs) { return fixed(lhs.v op rhs.v); } \
	PROXY_OP_HELPER(op, long double) \
	PROXY_OP_HELPER(op, double) \
	PROXY_OP_HELPER(op, float) \
	PROXY_OP_HELPER(op, long long) \
	PROXY_OP_HELPER(op, long) \
	PROXY_OP_HELPER(op, int) \
	PROXY_OP_HELPER(op, unsigned long long) \
	PROXY_OP_HELPER(op, unsigned long) \
	PROXY_OP_HELPER(op, unsigned int)

#define PROXY_OP_UNARY(op) inline fixed operator op () const { return fixed(-v); }
#define PROXY_CMP_HELPER(op, type) \
	inline bool operator op(type lhs, const fixed& rhs) { return lhs op rhs.v; } \
	inline bool operator op(const fixed& lhs, type rhs) { return lhs.v op rhs; }
#define PROXY_CMP(op) \
	inline bool operator op(const fixed& lhs, const fixed& rhs) { return lhs.v op rhs.v; } \
	PROXY_CMP_HELPER(op, long double) \
	PROXY_CMP_HELPER(op, double) \
	PROXY_CMP_HELPER(op, float) \
	PROXY_CMP_HELPER(op, long int) \
	PROXY_CMP_HELPER(op, int)

		struct fixed
		{
			float v;
			fixed(): v(0) {}
			fixed(fixed && rhs) : v(rhs.v) { }
			fixed(long long v) : v(v) {}
			fixed(int v) : v(v) {}
			fixed(unsigned long long v) : v(v) {}
			fixed(unsigned int v) : v(v) {}

			explicit fixed(long double v) : v(v) {}
			explicit fixed(double v) : v(v) {}
			explicit fixed(float v) : v(v) {}

			fixed& operator = (fixed && rhs) { v = rhs.v; return *this; }
			fixed& operator = (const fixed& rhs) { v = rhs.v; return *this; }
			fixed& operator = (long long rhs) { v = rhs; return *this; }
			fixed& operator = (int rhs) { v = rhs; return *this; }
			fixed& operator = (unsigned long long rhs) { v = rhs; return *this; }
			fixed& operator = (unsigned int rhs) { v = rhs; return *this; }

			PROXY_OP_M( += );
			PROXY_OP_M( -= );
			PROXY_OP_M( *= );
			PROXY_OP_M( /= );
			PROXY_OP_UNARY(-);
			PROXY_OP_UNARY(+);
		private:
		};

		PROXY_CMP( == );
		PROXY_CMP( != );
		PROXY_CMP( <  );
		PROXY_CMP( >  );
		PROXY_CMP( <= );
		PROXY_CMP( >= );

		PROXY_OP( + );
		PROXY_OP( - );
		PROXY_OP( * );
		PROXY_OP( / );

		inline fixed __abs(const fixed& ld)
		{
			if (ld.v < 0) return fixed{ -ld.v };
			return ld;
		}

		inline fixed sin(const fixed& v) { return fixed(std::sin(v.v)); }
		inline fixed cos(const fixed& v) { return fixed(std::cos(v.v)); }
		inline fixed sqrt(const fixed& v) { return fixed(std::sqrt(v.v)); }

		template <typename T>
		inline T cast(const fixed& v) { return (T)v.v; }

		template <size_t width, size_t height>
		class MatrixBase
		{
		protected:
			fixed m_data[width*height];
		public:
			enum
			{
				my_width = width,
				my_height = height
			};
			fixed at(size_t x, size_t y) const { return m_data[x + y * width]; }
			fixed& at(size_t x, size_t y) { return m_data[x + y * width]; }
		};

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
			rhs.m_data[i] = fixed(); \
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
			rhs.m_data[i] = fixed(); \
		}\
		return *this; \
	} \
\
	my_t& set_at(size_t x, size_t y, const fixed& val) { m_data[x + y * my_width] = val; return *this; }

#define MATRIX_PROP(name, pos) \
	const fixed& name() const { return m_data[pos]; } \
	fixed& name() { return m_data[pos]; } \
	my_t& set_ ## name(const fixed& val) { m_data[pos] = val; return *this; }


		class Matrix : public MatrixBase<4, 4>
		{
		public:
			Matrix()
			{
				// identity
				for (size_t i = 0; i < sizeof(m_data) / sizeof(m_data[0]); ++i)
					m_data[i] = i % 5 ? 0 : 1;
			}

			MATRIX_OPS(Matrix)

			static Matrix identity() { return Matrix(); }
			static Matrix translate(const fixed& dx, const fixed& dy = fixed(), const fixed& dz = fixed())
			{
				return Matrix().set_at(3, 0, dx).set_at(3, 1, dy).set_at(3, 2, dz);
			}
			static Matrix scale(const fixed& sx, const fixed& sy = fixed(), const fixed& sz = fixed())
			{
				auto _sy = sy;
				auto _sz = sz;
				if (__abs(sy) < fixed(0.00001)) _sy = sx;
				if (__abs(sz) < fixed(0.00001)) _sz = sy;
				return Matrix().set_at(0, 0, sx).set_at(1, 1, _sy).set_at(2, 2, _sz);
			}
			static Matrix rotateX(const fixed& theta)
			{
				fixed cosTheta = cos(theta);
				fixed sinTheta = sin(theta);
				return Matrix().set_at(2, 2, cosTheta).set_at(3, 2, -sinTheta).set_at(2, 3, sinTheta).set_at(3, 3, cosTheta);
			}
			static Matrix rotateY(const fixed& theta)
			{
				fixed cosTheta = cos(theta);
				fixed sinTheta = sin(theta);
				return Matrix().set_at(0, 0, cosTheta).set_at(3, 0, sinTheta).set_at(0, 3, -sinTheta).set_at(3, 3, cosTheta);
			}
			static Matrix rotateZ(const fixed& theta)
			{
				fixed cosTheta = cos(theta);
				fixed sinTheta = sin(theta);
				return Matrix().set_at(0, 0, cosTheta).set_at(1, 0, -sinTheta).set_at(0, 1, sinTheta).set_at(1, 1, cosTheta);
			}

			bool is_identity() const
			{
				for (size_t i = 0; i < sizeof(m_data) / sizeof(m_data[0]); ++i)
					if (m_data[i] != (i % 5 ? 0 : 1)) return false;

				return true;
			}


			template <typename T>
			T multiply(const T& rhs) const
			{
				static_assert(my_width == T::my_height, "Only matrices of height 4 allowed");
				if (is_identity())
					return rhs;

				T out;
				for (size_t w = 0; w < T::my_width; ++w)
				{
					for (size_t h = 0; h < T::my_height; ++h)
					{
						out.at(w, h) = fixed();
						for (size_t c = 0; c < my_width; ++c)
						{
							out.at(w, h) += at(c, h) * rhs.at(w, c);
						}
					}
				}
				return out;
			}
		};

		class Point
		{
			enum { my_width = 2 };
			fixed m_data[2];
		public:
			Point()
			{
				m_data[0] = m_data[1] = fixed();
			}
			Point(const fixed& x, const fixed& y)
			{
				m_data[0] = x;
				m_data[1] = y;
			}
			MATRIX_OPS(Point);
			MATRIX_PROP(x, 0);
			MATRIX_PROP(y, 1);
		};

		class Vertex : public MatrixBase<1, 4>
		{
		public:
			Vertex()
			{
				for (size_t i = 0; i < my_height - 1; ++i)
					m_data[i] = fixed();
				m_data[my_height - 1] = fixed(1);
			}
			Vertex(const fixed& x, const fixed& y, const fixed& z)
			{
				m_data[0] = x;
				m_data[1] = y;
				m_data[2] = z;
				m_data[3] = fixed(1);
			}
			Vertex(long long x, long long y, long long z)
			{
				m_data[0] = fixed(x);
				m_data[1] = fixed(y);
				m_data[2] = fixed(z);
				m_data[3] = fixed(1);
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
					m_data[i] = fixed(i == my_height - 1 ? 1 : 0);
			}
			Vector(const fixed& i, const fixed& j, const fixed& k)
			{
				m_data[0] = i;
				m_data[1] = j;
				m_data[2] = k;
				m_data[3] = fixed(1);
			}
			MATRIX_OPS(Vector);
			MATRIX_PROP(i, 0);
			MATRIX_PROP(j, 1);
			MATRIX_PROP(k, 2);

			fixed length() const { return sqrt(lengthSquared()); }
			fixed lengthSquared() const { return dotProduct(*this, *this); }

#define COORD(next, prev) (lhs.next() * rhs.prev() - rhs.next() * lhs.prev())
			static Vector crossProduct(const Vector& lhs, const Vector& rhs)
			{
				return { COORD(j, k), COORD(k, i), COORD(i, j) };
			}
#undef COORD

			static fixed dotProduct(const Vector& lhs, const Vector& rhs)
			{
				return lhs.i() * rhs.i() + lhs.j() * rhs.j() + lhs.k() * rhs.k();
			}

			static fixed cosTheta(const Vector& lhs, const Vector& rhs);
		};

		inline Vector operator - (const Vertex& lhs, const Vertex& rhs)
		{
			return { lhs.x() - rhs.x(), lhs.y() - rhs.y(), lhs.z() - rhs.z() };
		}

		inline Vertex operator + (const Vertex& lhs, const Vertex& rhs)
		{
			return { lhs.x() + rhs.x(), lhs.y() + rhs.y(), lhs.z() + rhs.z() };
		}

		inline Vertex operator + (const Vector& lhs, const Vertex& rhs)
		{
			return { lhs.i() + rhs.x(), lhs.j() + rhs.y(), lhs.k() + rhs.z() };
		}

		inline Vertex operator + (const Vertex& lhs, const Vector& rhs)
		{
			return { lhs.x() + rhs.i(), lhs.y() + rhs.j(), lhs.z() + rhs.k() };
		}

		inline Vertex operator - (const Vector& lhs, const Vertex& rhs)
		{
			return { lhs.i() - rhs.x(), lhs.j() - rhs.y(), lhs.k() - rhs.z() };
		}

		inline Vertex operator - (const Vertex& lhs, const Vector& rhs)
		{
			return { lhs.x() - rhs.i(), lhs.y() - rhs.j(), lhs.z() - rhs.k() };
		}

		inline Vertex operator / (const Vertex& lhs, const fixed& rhs)
		{
			return { lhs.x() / rhs, lhs.y() / rhs, lhs.z() / rhs };
		}

		inline Vector operator / (const Vector& lhs, const fixed& rhs)
		{
			return { lhs.i() / rhs, lhs.j() / rhs, lhs.k() / rhs };
		}

		inline Vector operator* (const Matrix& lhs, const Vector& rhs)
		{
			return lhs.multiply(rhs);
		}

		inline Vertex operator* (const Matrix& lhs, const Vertex& rhs)
		{
			return lhs.multiply(rhs);
		}

		inline Matrix operator* (const Matrix& lhs, const Matrix& rhs)
		{
			return lhs.multiply(rhs);
		}

	}

	using math::fixed;
	using math::cast;

	inline void swap(studio::math::fixed& _Left, studio::math::fixed& _Right)
	{
		studio::math::fixed _Tmp = std::move(_Left);
		_Left = std::move(_Right);
		_Right = std::move(_Tmp);
	}
}

#endif //__LIBSTUDIO_FUNDAMENTALS_HPP__