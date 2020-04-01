#pragma once

#include <random>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <math.h>

typedef int unsigned uint;

struct RndEngine
{

	std::mt19937 gen;

	std::uniform_real_distribution<float> uniform = std::uniform_real_distribution<float>(0, 1);

	std::uniform_int_distribution<uint> distr_coin = std::uniform_int_distribution<uint>(1, 2);
	std::uniform_int_distribution<uint> distr_1d4 = std::uniform_int_distribution<uint>(1, 4);
	std::uniform_int_distribution<uint> distr_1d6 = std::uniform_int_distribution<uint>(1, 6);
	std::uniform_int_distribution<uint> distr_1d10 = std::uniform_int_distribution<uint>(1, 10);
	std::uniform_int_distribution<uint> distr_1d20 = std::uniform_int_distribution<uint>(1, 20);

	unsigned int seed;

	RndEngine()
	{
		init();
	}

	void init()
	{
		std::random_device rd;
		init(rd());
	}

	void init(unsigned int _seed)
	{
		seed = _seed;
		gen = std::mt19937(seed);
	}

	void init(uint64_t _seed)
	{
		std::mt19937_64 proxy(_seed); // cutre hash
		std::uniform_int_distribution<unsigned int> dist;
		init(dist(proxy));
	}


	inline uint roll_flipcoin() { return distr_coin(gen)-1; }
	inline uint roll_1d4() { return distr_1d4(gen); }
	inline uint roll_1d6() { return distr_1d6(gen); }
	inline uint roll_1d10() { return distr_1d10(gen); }
	inline uint roll_1d20() { return distr_1d20(gen); }

	inline uint roll(uint min, uint max) { return std::uniform_int_distribution<uint>(min, max)(gen); }
	inline uint64_t roll64(uint64_t min, uint64_t max) { return std::uniform_int_distribution<uint64_t>(min, max)(gen); }
	inline float rollf(float min, float max) { return std::uniform_real_distribution<float>(min, max)(gen); }

	inline float rollf() { return uniform(gen); }

	template<typename _T>
	void shuffle(_T *values, size_t count)
	{
		std::shuffle(values, values + (count - 1), gen);
	}

	template<typename _T, size_t _Count>
	void shuffle(_T(&values)[_Count])
	{
		return shuffle<_T>(values, _Count);
	}
};

namespace Random
{
	extern RndEngine action;

	inline uint roll_flipcoin() { return action.roll_flipcoin(); }
	inline uint roll_1d4() { return action.roll_1d4(); }
	inline uint roll_1d6() { return action.roll_1d6(); }
	inline uint roll_1d10() { return action.roll_1d10(); }
	inline uint roll_1d20() { return action.roll_1d20(); }

	inline uint roll(uint min, uint max) { return action.roll(min, max); }
	inline uint roll(uint max) { return roll(0, max); }

	inline uint64_t roll64(uint64_t min, uint64_t max) { return action.roll64(min, max); }
	inline uint64_t roll64(uint64_t max) { return roll(0, (uint)max); }

	inline float rollf(float min, float max) { return action.rollf(min, max); }
	inline float rollf(float max) { return rollf(0.f, max); }
	inline float rollf() { return rollf(0.f, 1.f); }

	template<typename _T>
	void shuffle(_T *values, size_t count)
	{
		action.shuffle(values, count);
	}

	template<typename _T, size_t _Count>
	void shuffle(_T(&values)[_Count])
	{
		action.shuffle(values);
	}

	inline void CircularRoll(float radius, float& x, float& y)
	{
		float r = rollf(0.0f, radius);
		float ang = rollf(0.0f, 360.0f);

		float rads = ang * static_cast<float>(M_PI) / 180.0f;

		x = r * std::cos(rads);
		y = r * std::sin(rads);
	}

	//returns a random number with a normal distribution. See method at
	//http://www.taygeta.com/random/gaussian.html
	inline float Gaussian(float mean = 0.0, float standard_deviation = 1.0)
	{
		float x1, x2, w, y1;
		static float y2;
		static int use_last = 0;

		if (use_last)		        /* use value from previous call */
		{
			y1 = y2;
			use_last = 0;
		}
		else
		{
			do
			{
				x1 = 2.0f * rollf() - 1.0f;
				x2 = 2.0f * rollf() - 1.0f;
				w = x1 * x1 + x2 * x2;
			} while (w >= 1.0f);

			w = sqrt((-2.0f * log(w)) / w);
			y1 = x1 * w;
			y2 = x2 * w;
			use_last = 1;
		}

		return(mean + y1 * standard_deviation);
	}

}
