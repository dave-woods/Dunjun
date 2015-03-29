#ifndef DUNJUN_MATH_RANDOM_HPP
#define DUNJUN_MATH_RANDOM_HPP

#include <Dunjun/Common.hpp>

#include <random>

namespace Dunjun
{
	class Random
	{
	public:
		Random() = default;
		Random(std::mt19937::result_type seed)
			: m_engine(seed)
		{

		}

		void setSeed(std::mt19937::result_type seed)
		{
			m_engine.seed(seed);
		}

		//inclusive
		i32 getInt(i32 min, i32 max)
		{
			std::uniform_int_distribution<i32> dist(min, max);
			return dist(m_engine);
		}

		f32 getFloat(f32 min, f32 max)
		{
			std::uniform_real_distribution<f32> dist(min, max);
			return dist(m_engine);
		}

	private:
		std::mt19937 m_engine{ std::random_device{}() };
	};
}


#endif // !DUNJUN_MATH_RANDOM_HPP
