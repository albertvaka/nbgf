#pragma once

#ifndef _USE_MATH_DEFINES 
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include <random>
#include <algorithm>

#include "vec.h"
#include "bounds.h"

namespace Rand
{
	[[nodiscard]] inline float rollf(float min, float max) { return min + (rand() / (float(RAND_MAX) / (max - min))); } // Range [min, max) most of the time (except when rand() == RAND_MAX, then its [min, max])
	[[nodiscard]] inline float rollf(float max = 1.f) { return rollf(0.f, max); } // Range [0, max)

	[[nodiscard]] inline int roll(int min, int max) { return min + (rand() % (max - min)); } // Range [min, max)
	[[nodiscard]] inline int roll(int max) { return roll(0, max); } // Range [0, max)
	[[nodiscard]] inline bool OnceEvery(int n) { return roll(0, n) == 0; }
	[[nodiscard]] inline bool PercentChance(int percent) { return roll(0, 100) < percent; }

	// Unit vector in a random direction
	[[nodiscard]] inline vec DirInCircle()
	{
		float rads = rollf(0.0f, Angles::Tau);
		return vec(std::cos(rads), std::sin(rads));
	}

	// Randoms-sized vector in a random direction
	[[nodiscard]] inline vec PosInsideCircle(float radius)
	{
		// Rejection sampling is the fastest way I found to implement this
		// Doing DirInCircle()*radius is wrong because the point distribution is more dense in the center
		while (true) {
			float x = rollf(-1.f, 1.f);
			float y = rollf(-1.f, 1.f);
			if ((x * x + y * y) <= 1.f) {
				return vec(x,y)*radius;
			}
		}
	}

	[[nodiscard]] inline vec VecInRange(float minX, float minY, float maxX, float maxY) {
		return vec(rollf(minX, maxX), rollf(minY, maxY));
	}

	[[nodiscard]] inline vec VecInRange(vec min, vec max) {
		return VecInRange(min.x, min.y, max.x, max.y);
	}

	[[nodiscard]] inline vec VecInRange(const BoxBounds& bounds) {
		return VecInRange(bounds.Left(), bounds.Top(), bounds.Right(), bounds.Bottom());
	}
}

namespace GoodRand
{
	typedef unsigned int uint;

	inline struct RndEngine
	{
		std::mt19937 gen;

		// note: uniform_real_distribution interval is [a, b) but uniform_int_distribution is [a, b].
		std::uniform_real_distribution<float> uniform   = std::uniform_real_distribution<float>(0.f, 1.f);
		std::uniform_int_distribution<uint> distr_coin  = std::uniform_int_distribution<uint>(0, 1);
		std::uniform_int_distribution<uint> distr_1d3   = std::uniform_int_distribution<uint>(0, 2);
		std::uniform_int_distribution<uint> distr_1d4   = std::uniform_int_distribution<uint>(0, 3);
		std::uniform_int_distribution<uint> distr_1d6   = std::uniform_int_distribution<uint>(0, 5);
		std::uniform_int_distribution<uint> distr_1d10  = std::uniform_int_distribution<uint>(0, 9);
		std::uniform_int_distribution<uint> distr_1d20  = std::uniform_int_distribution<uint>(0, 19);
		std::uniform_int_distribution<uint> distr_1d100 = std::uniform_int_distribution<uint>(0, 99);
		std::uniform_int_distribution<uint> distr_1d360 = std::uniform_int_distribution<uint>(0, 359);

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
	} r;

	//Pre-defined distributions are faster than asking for an arbitrary range
	[[nodiscard]] inline uint roll_flipcoin() { return r.distr_coin(r.gen); } // [0,1]
	[[nodiscard]] inline uint roll_1d3() { return r.distr_1d3(r.gen); } // [0,2]
	[[nodiscard]] inline uint roll_1d4() { return r.distr_1d4(r.gen); } // [0,3]
	[[nodiscard]] inline uint roll_1d6() { return r.distr_1d6(r.gen); } // ...
	[[nodiscard]] inline uint roll_1d10() { return r.distr_1d10(r.gen); }
	[[nodiscard]] inline uint roll_1d20() { return r.distr_1d20(r.gen); }
	[[nodiscard]] inline uint roll_1d100() { return r.distr_1d100(r.gen); }
	[[nodiscard]] inline uint roll_1d360() { return r.distr_1d360(r.gen); }

	[[nodiscard]] inline float rollf() { return r.uniform(r.gen); } // [0.f,1.f)

	//returns a random number with a normal distribution. See method at
	//http://www.taygeta.com/random/gaussian.html
	[[nodiscard]] inline float Gaussian(float mean = 0.0, float standard_deviation = 1.0)
	{
		float x1, x2, w, y1;
		static float y2;
		static int use_last = 0;

		if (use_last) // use value from previous call
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
