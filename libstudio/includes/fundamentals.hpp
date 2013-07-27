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
#include <iostream>
#include <iomanip>

namespace studio
{
	typedef signed char i8;
	typedef signed short i16;
	typedef signed int i32;
	typedef signed long long i64;

	typedef unsigned char u8;
	typedef unsigned short u16;
	typedef unsigned int u32;
	typedef unsigned long long u64;

	namespace math
	{
		static const long double PI = 3.14159265358979323846264338327950288419716939937510L;
#if 1
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
			inline fixed __abs() const
			{
				if (v < 0) return -*this;
				return *this;
			}

			long double v;
			fixed(): v(0) {}
			fixed(fixed && rhs) : v(rhs.v) { }
			fixed(long long v) : v((decltype(this->v)) v) {}
			fixed(int v) : v(v) {}
			fixed(unsigned long long v) : v((decltype(this->v)) v) {}
			fixed(unsigned int v) : v(v) {}

			explicit fixed(long double v) : v(v) {}
			explicit fixed(double v) : v(v) {}
			explicit fixed(float v) : v(v) {}

			fixed& operator = (fixed && rhs) { v = rhs.v; return *this; }
			fixed& operator = (const fixed& rhs) { v = rhs.v; return *this; }
			fixed& operator = (long long rhs) { v = (decltype(this->v)) rhs; return *this; }
			fixed& operator = (int rhs) { v = rhs; return *this; }
			fixed& operator = (unsigned long long rhs) { v = (decltype(this->v)) rhs; return *this; }
			fixed& operator = (unsigned int rhs) { v = rhs; return *this; }

			PROXY_OP_M( += );
			PROXY_OP_M( -= );
			PROXY_OP_M( *= );
			PROXY_OP_M( /= );
			PROXY_OP_UNARY(-);
			PROXY_OP_UNARY(+);
			bool operator ! () const { return __abs().v < 0.00001; }
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
			return ld.__abs();
		}

		inline fixed sin(const fixed& v) { return fixed(std::sin(v.v)); }
		inline fixed cos(const fixed& v) { return fixed(std::cos(v.v)); }
		inline fixed sqrt(const fixed& v) { return fixed(std::sqrt(v.v)); }

		template <typename T>
		inline T cast(const fixed& v) { return (T)v.v; }
#else
#define PROXY_OP_M(op) \
	fixed& operator op(long double rhs) { return *this op fixed(rhs); } \
	fixed& operator op(double rhs) { return *this op fixed(rhs); } \
	fixed& operator op(float rhs) { return *this op fixed(rhs); } \
	fixed& operator op(long long rhs) { return *this op fixed(rhs); } \
	fixed& operator op(long rhs) { return *this op fixed(rhs); } \
	fixed& operator op(int rhs) { return *this op fixed(rhs); } \
	fixed& operator op(unsigned long long rhs) { return *this op fixed(rhs); } \
	fixed& operator op(unsigned long rhs) { return *this op fixed((unsigned long long)rhs); } \
	fixed& operator op(unsigned int rhs) { return *this op fixed(rhs); }

#define PROXY_CMP_HELPER(op, type) \
	inline bool operator op(type lhs, const fixed& rhs) { return fixed(lhs).v op rhs.v; } \
	inline bool operator op(const fixed& lhs, type rhs) { return lhs.v op fixed(rhs).v; }
#define PROXY_CMP(op) \
	inline bool operator op(const fixed& lhs, const fixed& rhs) { return lhs.v op rhs.v; } \
	PROXY_CMP_HELPER(op, long double) \
	PROXY_CMP_HELPER(op, double) \
	PROXY_CMP_HELPER(op, float) \
	PROXY_CMP_HELPER(op, long int) \
	PROXY_CMP_HELPER(op, int)

#define PROXY_OP_HELPER(op, type) \
	inline fixed operator op(type lhs, const fixed& rhs) { return fixed(lhs) op rhs; } \
	inline fixed operator op(const fixed& lhs, type rhs) { return lhs op fixed(rhs); }

#define PROXY_OP(op) \
	PROXY_OP_HELPER(op, long double) \
	PROXY_OP_HELPER(op, double) \
	PROXY_OP_HELPER(op, float) \
	PROXY_OP_HELPER(op, long long) \
	PROXY_OP_HELPER(op, long) \
	PROXY_OP_HELPER(op, int) \
	PROXY_OP_HELPER(op, unsigned long long) \
	inline fixed operator op(unsigned long lhs, const fixed& rhs) { return fixed((unsigned long long)lhs) op rhs; } \
	inline fixed operator op(const fixed& lhs, unsigned long rhs) { return lhs op fixed((unsigned long long)rhs); } \
	PROXY_OP_HELPER(op, unsigned int)

		struct fixed
		{
			enum { BITS = 8, DENOM = 1 << BITS };
			long long v;

			static long long conv(long long v)
			{
				auto neg = v < 0;
				if (neg) v = -v;
				v = v << BITS;
				if (neg) v = -v;
				return v;
			}
			static long long conv(int v)  { return conv((long long) v); }
			static long long conv(unsigned long long v) { return (0x7FFFFFFFFFFFFFFFull & (v << BITS)); }
			static long long conv(unsigned int v) { return v << BITS; }
			static long long conv(long double v)
			{
				v *= DENOM;
				v += 0.5;
				return (long long) v;
			}

			static long long conv(double v)
			{
				v *= DENOM;
				v += 0.5;
				return (long long) v;
			}

			static long long conv(float v)
			{
				v *= DENOM;
				v += 0.5;
				return (long long) v;
			}

			fixed() : v(0) {}
			fixed(fixed && rhs) : v(rhs.v){}
			fixed(const fixed & rhs) : v(rhs.v){}
			fixed(long long v) : v(conv(v)) {}
			fixed(int v) : v(conv(v)) {}
			fixed(unsigned long long v) : v(conv(v)) {}
			fixed(unsigned int v) : v(conv(v)) {}

			explicit fixed(long double v) : v(conv(v)) {}
			explicit fixed(double v) : v(conv(v)) {}
			explicit fixed(float v) : v(conv(v)) {}

			static fixed fromValue(long long v)
			{
				fixed ret;
				ret.v = v;
				return ret;
			}

			fixed& operator = (fixed && rhs) { v = rhs.v; return *this; }
			fixed& operator = (const fixed& rhs) { v = rhs.v; return *this; }
			fixed& operator = (long long rhs) { v = conv(rhs); return *this; }
			fixed& operator = (int rhs) { v = conv(rhs); return *this; }
			fixed& operator = (unsigned long long rhs) { v = conv(rhs); return *this; }
			fixed& operator = (unsigned int rhs) { v = conv(rhs); return *this; }
#if 1
			fixed& operator += (const fixed& rhs) { v += rhs.v; return *this; }
			fixed& operator -= (const fixed& rhs) { v -= rhs.v; return *this; }
			fixed& operator *= (const fixed& rhs) { v *= rhs.v; v /= DENOM; return *this; }
			fixed& operator /= (const fixed& rhs) { v *= DENOM; v /= rhs.v; return *this; }
#else
			fixed& operator += (const fixed& rhs)
			{
				auto tmp = v;
				v += rhs.v;

				auto t1 = (float) tmp / DENOM;
				auto v1 = (float) v / DENOM;
				auto r1 = (float) rhs.v / DENOM;
				if (t1 + r1 != v1)
					std::cout << "!" << std::flush;
				return *this;
			}
			fixed& operator -= (const fixed& rhs)
			{
				auto tmp = v;
				v += rhs.v;

				auto t1 = (float) tmp / DENOM;
				auto v1 = (float) v / DENOM;
				auto r1 = (float) rhs.v / DENOM;
				if (t1 - r1 != v1)
					std::cout << "!" << std::flush;
				return *this;
			}
			fixed& operator *= (const fixed& rhs)
			{
				auto tmp = v;
				v += rhs.v;

				auto t1 = (float) tmp / DENOM;
				auto v1 = (float) v / DENOM;
				auto r1 = (float) rhs.v / DENOM;
				if (t1 * r1 != v1)
					std::cout << "!" << std::flush;
				return *this;
			}
			fixed& operator /= (const fixed& rhs)
			{
				auto tmp = v;
				v += rhs.v;

				auto t1 = (float) tmp / DENOM;
				auto v1 = (float) v / DENOM;
				auto r1 = (float) rhs.v / DENOM;
				if (t1 / r1 != v1)
					std::cout << "!" << std::flush;
				return *this;
			}
#endif

			PROXY_OP_M(+= );
			PROXY_OP_M(-= );
			PROXY_OP_M(*= );
			PROXY_OP_M(/= );

			inline fixed operator -() const { return fromValue(-v); }
			inline fixed operator +() const { return fromValue(+v); }
			inline bool operator !() const { return v == 0; }

			inline long long ipart() const { return v / DENOM; }
		};

		PROXY_CMP(== );
		PROXY_CMP(!= );
		PROXY_CMP(<);
		PROXY_CMP(>);
		PROXY_CMP(<= );
		PROXY_CMP(>= );

#if 1
		inline fixed operator + (const fixed& lhs, const fixed& rhs) { return fixed::fromValue(lhs.v + rhs.v); }
		inline fixed operator - (const fixed& lhs, const fixed& rhs) { return fixed::fromValue(lhs.v - rhs.v); }
		inline fixed operator * (const fixed& lhs, const fixed& rhs) { return fixed::fromValue((lhs.v * rhs.v) / fixed::DENOM); }
		inline fixed operator / (const fixed& lhs, const fixed& rhs) { return fixed::fromValue((lhs.v * fixed::DENOM) / rhs.v); }
#else
		inline fixed operator + (const fixed& lhs, const fixed& rhs)
		{
			auto ret = v;
			v += rhs.v;

			auto t1 = (float) tmp / DENOM;
			auto v1 = (float) v / DENOM;
			auto r1 = (float) rhs.v / DENOM;
			if (t1 + r1 != v1)
				std::cout << "!" << std::flush;
			return ret;
		}
		inline fixed operator - (const fixed& lhs, const fixed& rhs) { return fixed(lhs.v - rhs.v); }
		inline fixed operator * (const fixed& lhs, const fixed& rhs) { return fixed((lhs.v * rhs.v) / fixed::DENOM); }
		inline fixed operator / (const fixed& lhs, const fixed& rhs) { return fixed((lhs.v * fixed::DENOM) / rhs.v); }
#endif
		PROXY_OP(+);
		PROXY_OP(-);
		PROXY_OP(*);
		PROXY_OP(/ );

		inline fixed __abs(const fixed& ld)
		{
			if (ld < 0) return -ld;
			return ld;
		}

		template <class T> struct caster;
#define FLOAT_CAST(T) template <> struct caster<T> { static T cast(const fixed& v) { return ((T)v.v) / fixed::DENOM; } }
#define INT_CAST(T) template <> struct caster<T> { static T cast(const fixed& v) { return (T) v.ipart(); } }

		FLOAT_CAST(long double);
		FLOAT_CAST(double);
		FLOAT_CAST(float);
		INT_CAST(long long);
		INT_CAST(long);
		INT_CAST(int);
		INT_CAST(char);
		INT_CAST(unsigned long long);
		INT_CAST(unsigned long);
		INT_CAST(unsigned int);
		INT_CAST(unsigned char);

		template <typename T>
		inline T cast(const fixed& v) { return caster<T>::cast(v); }

		inline fixed sin(const fixed& v) { return fixed(std::sin(cast<double>(v))); }
		inline fixed cos(const fixed& v) { return fixed(std::cos(cast<double>(v))); }
		inline fixed sqrt(const fixed& v) { return fixed(std::sqrt(cast<double>(v))); }
#endif

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

	struct Color
	{
		u8 R;
		u8 G;
		u8 B;
		enum { channels = 3 };
		u8 channel(int ch) const
		{
			switch (ch)
			{
			case 0: return B;
			case 1: return G;
			default:
				break;
			}
			return R;
		}

		u8& channel(int ch)
		{
			switch (ch)
			{
			case 0: return B;
			case 1: return G;
			default:
				break;
			}
			return R;
		}

		Color() : R(0), G(0), B(0) {}
		Color(u8 R, u8 G, u8 B) : R(R), G(G), B(B) {}
		explicit Color(u32 color)
			: R((color >> 16) & 0xFF)
			, G((color >> 8) & 0xFF)
			, B((color) & 0xFF)
		{
		}
		Color(const Color& rhs) : R(rhs.R), G(rhs.G), B(rhs.B) {}
		Color& operator = (const Color& rhs)
		{
			R = rhs.R;
			G = rhs.G;
			B = rhs.B;
			return *this;
		}
		static Color fromCStr(const char* str);
		static Color white() { return { 0xFF, 0xFF, 0xFF }; }
		static Color black() { return { 0x00, 0x00, 0x00 }; }
	};

	struct Grayscale
	{
		u8 Y;
		enum { channels = 1 };
		u8 channel(int) const { return Y; }
		explicit Grayscale(u8 Y = 0) : Y(Y) {}
		Grayscale(const Grayscale& rhs) : Y(rhs.Y) {}
		Grayscale& operator = (const Grayscale& rhs)
		{
			Y = rhs.Y;
			return *this;
		}
		explicit Grayscale(const Color& color)
		{
			if (color.R == color.G && color.R == color.B)
			{
				Y = color.R;
				return;
			}

			u64 lum = (u64) color.R * 299;
			lum += (u64) color.G * 587;
			lum += (u64) color.B * 114;
			lum /= 1000;
			Y = (u8) lum;
		}
		static Grayscale white() { return Grayscale{ 0xFF }; }
	};

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