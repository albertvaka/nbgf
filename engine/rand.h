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
	[[nodiscard]] inline float rollf(float min, float max) { return min + (rand() / (float(RAND_MAX) / (max - min))); } // Range [min, max]
	[[nodiscard]] inline float rollf(float max = 1.f) { return rollf(0.f, max); } // Range [0, max]

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
		// Rejection sampling is the fastest way to implement this
		// Doing DirInCircle()*rollf(radius) is wrong because the point distribution is more dense in the center
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

struct SeededRand
{
	std::mt19937 eng;

	explicit SeededRand(unsigned int seed) : eng(seed) { }

	// Use a distribution instead of these functions to avoid bias
	[[nodiscard]] inline float rollf(float min, float max) { return min + (eng() / (float(rand_max()) / (max - min))); } // Range [min, max]
	[[nodiscard]] inline float rollf(float max = 1.f) { return rollf(0.f, max); } // Range [0, max]

	[[nodiscard]] inline int roll(int min, int max) { return min + ((unsigned int)(eng()) % (max - min)); } // Range [min, max)
	[[nodiscard]] inline int roll(int max) { return roll(0, max); } // Range [0, max)
	[[nodiscard]] inline bool OnceEvery(int n) { return roll(0, n) == 0; }
	[[nodiscard]] inline bool PercentChance(int percent) { return roll(0, 100) < percent; }

	[[nodiscard]] inline unsigned int rand_max() { return eng.max(); }

	// note: uniform_int_distribution is [a, b].
	struct Distribution {
		SeededRand* seedRand;
		std::uniform_int_distribution<unsigned int> distr;
		Distribution(SeededRand* seedRand, int from, int to) : seedRand(seedRand), distr(from, to) {}
		int operator()() { return distr(seedRand->eng); }
	};

	// note: uniform_real_distribution interval is [a, b)
	struct Distributionf {
		SeededRand* seedRand;
		std::uniform_real_distribution<float> distr;
		Distributionf(SeededRand* seedRand, float from, float to) : seedRand(seedRand), distr(from, to) {}
		int operator()() { return distr(seedRand->eng); }
	};

	Distribution distribution(int from, int to)
	{
		return Distribution(this, from, to);
	}

	Distribution distribution(int to)
	{
		return Distribution(this, 0, to);
	}

	Distributionf distributionf(float from, float to)
	{
		return Distributionf(this, from, to);
	}

	Distributionf distributionf(float to = 1.f)
	{
		return Distributionf(this, 0.f, to);
	}
};
