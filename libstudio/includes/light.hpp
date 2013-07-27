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

#ifndef __LIBSTUDIO_LIGHT_HPP__
#define __LIBSTUDIO_LIGHT_HPP__

#include "fundamentals.hpp"
#include "shared_vector.hpp"

namespace studio
{
	struct Light
	{
		virtual ~Light() {}
		virtual const math::Vertex& position() const = 0;
		virtual fixed power() const = 0;
	};

	typedef std::shared_ptr<Light> LightPtr;
	typedef std::shared_vector<Light> Lights;

	class SimpleLight : public Light
	{
		math::Vertex m_position;
		fixed m_power;
	public:
		SimpleLight(const math::Vertex& position, const fixed& power)
			: m_position(position)
			, m_power(power)
		{}

		const math::Vertex& position() const override { return m_position; }
		fixed power() const override { return m_power;  }
	};

	struct LightInfo
	{
		math::Vertex position;
		fixed power;
		explicit LightInfo(const math::Vertex& position, const fixed& power)
			: position(position)
			, power(power / 100)
		{
		}
	};
	struct LightsInfo
	{
		std::vector<LightInfo> m_lights;
		math::Vector m_normal;

		fixed getIntensity(const math::Vertex& point) const
		{
			fixed intensity = 1;
			if (!m_lights.empty())
			{
				intensity = 0;
				for (auto && light : m_lights)
				{
					auto v = point - light.position;
					fixed lengthSq = v.lengthSquared() / 1000000;
					//if (lengthSq > 1)
					//	lengthSq = 1;
					intensity += 1 - math::Vector::cosTheta(m_normal, v) * light.power / lengthSq;
				}
				intensity /= 2;
				intensity /= m_lights.size();
			}
			return intensity;
		}
	};
}

#endif //__LIBSTUDIO_LIGHT_HPP__