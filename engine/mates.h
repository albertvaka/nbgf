#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include <vector>
#ifndef _USE_MATH_DEFINES 
#define _USE_MATH_DEFINES
#endif
#include <math.h>
#include <limits>
#include <string>
#include <SDL_assert.h>

namespace Mates
{
	//a few useful constants
	constexpr int     MaxInt = std::numeric_limits<int>::max();
	constexpr int     MinInt = std::numeric_limits<int>::min();
	constexpr float   MaxFloat = std::numeric_limits<float>::max();
	constexpr float   MinFloat = std::numeric_limits<float>::lowest();
	constexpr float   SmallestFloat = std::numeric_limits<float>::min(); // footgun: min returns the smallest *positive* value

	//returns true is the third parameter is in the range described by the first two
	[[nodiscard]] inline bool InRange(float start, float end, float val)
	{
		if (start < end)
		{
			if ((val > start) && (val < end)) return true;
			else return false;
		}
		else
		{
			if ((val < start) && (val > end)) return true;
			else return false;
		}
	}

	template <class T>
	[[nodiscard]] inline T Map(T x, T in_min, T in_max, T out_min, T out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	[[nodiscard]] std::string ToStringWithPrecision(float a_value, int n = 2);

	[[nodiscard]] std::string ToHexa(int a_value);

	// checks if adding the time delta dt to the current time causes the total time to cross a multiple boundary of period_sec,
	// used to trigger events at regular intervals
	[[nodiscard]] inline bool EachPeriod(float period_sec, float total_time_before_dt_increment, float dt) {
		return int((total_time_before_dt_increment + dt) / period_sec) != int((total_time_before_dt_increment) / period_sec);
	}

	[[nodiscard]] inline int RoundUpToMultipleOf(int value, int multiple)
	{
		// Rounds away from zero for negative numbers
		return ((value + multiple - 1) / multiple) * multiple;
	}

	[[nodiscard]] inline float Sigmoid(float input, float response = 1.0)
	{
		return (1.0f / (1.0f + exp(-input / response)));
	}

	//clamps the first argument between the second two
	template <class T>
	inline void Clamp(T& arg, const T& minVal, const T& maxVal)
	{
		SDL_assert(minVal <= maxVal);

		if (arg < minVal)
		{
			arg = minVal;
		}

		if (arg > maxVal)
		{
			arg = maxVal;
		}
	}

	template <class T>
	T Clamped(T arg, const T& minVal, const T& maxVal)
	{
		Clamp(arg, minVal, maxVal);
		return arg;
	}

	template <class T>
	inline void ClampMax(T& arg, const T& maxVal)
	{
		if (arg > maxVal)
		{
			arg = maxVal;
		}
	}

	template <class T>
	T ClampedMax(T arg, const T& maxVal)
	{
		ClampMax(arg, maxVal);
		return arg;
	}

	template <class T>
	inline void ClampMin(T& arg, const T& minVal)
	{
		if (arg < minVal)
		{
			arg = minVal;
		}
	}

	template <class T>
	T ClampedMin(T arg, const T& minVal)
	{
		ClampMin(arg, minVal);
		return arg;
	}

	[[nodiscard]] inline float Lerp(float from, float to, float t)
	{
		return (from)+(to - from) * t;
	}


	[[nodiscard]] inline float SmoothDamp(float current, float target, float dampening) {
		return ((current * (dampening - 1)) + target) / dampening; //weighted average formula
	}

	[[nodiscard]] inline int Rounded(float val)
	{
		int    integral = (int)val;
		float mantissa = val - integral;

		if (mantissa < 0.5f)
		{
			return integral;
		}
		else
		{
			return integral + 1;
		}
	}

	//rounds up/down at the specified offset instead of 0.5
	[[nodiscard]] inline int RoundedUnderOffset(float val, float offset)
	{
		int    integral = (int)val;
		float mantissa = val - integral;

		if (mantissa < offset)
		{
			return integral;
		}
		else
		{
			return integral + 1;
		}
	}

	//compares two real numbers. Returns true if they are equal
	[[nodiscard]] inline bool IsNearlyEqual(float a, float b, float margin = 1E-6f)
	{
		return (fabs(a - b) < margin);
	}

	template <class T>
	[[nodiscard]] inline float Average(const std::vector<T>& v)
	{
		float average = 0.0;

		for (unsigned int i = 0; i < v.size(); ++i)
		{
			average += (float)v[i];
		}

		return average / (float)v.size();
	}

	[[nodiscard]] inline float StandardDeviation(const std::vector<float>& v)
	{
		float sd = 0.0;
		float average = Average(v);

		for (unsigned int i = 0; i < v.size(); ++i)
		{
			sd += (v[i] - average) * (v[i] - average);
		}

		sd = sd / v.size();

		return sqrt(sd);
	}

	[[nodiscard]] inline int FastFloor(float x) { return x > 0 ? (int)x : (int)x - 1; }

	[[nodiscard]] inline float Sign(float value)
	{
		return value >= 0? 1.f : -1.f;
	}

	[[nodiscard]] inline int DivCeil(int a, int b)
	{
		return (a + b - 1) / b;
	}

	struct Range {
		float min;
		float max;
	};
}
