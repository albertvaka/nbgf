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
	constexpr int     MaxInt = (std::numeric_limits<int>::max)();
	constexpr int     MinInt = (std::numeric_limits<int>::min)();
	constexpr double  MaxDouble = (std::numeric_limits<double>::max)();
	constexpr double  MinDouble = (std::numeric_limits<double>::min)();
	constexpr float   MaxFloat = (std::numeric_limits<float>::max)();
	constexpr float   MinFloat = (std::numeric_limits<float>::min)();

	//returns true if the parameter is equal to zero
	[[nodiscard]] inline bool IsZero(float val)
	{
		return (-MinFloat < val) && (val < MinFloat);
	}

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

	[[nodiscard]] inline float map(float x, float in_min, float in_max, float out_min, float out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	[[nodiscard]] std::string to_string_with_precision(float a_value, int n = 2);

	[[nodiscard]] std::string to_hexa(int a_value);

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
	template <class T, class U, class V>
	inline void Clamp(T& arg, const U& minVal, const V& maxVal)
	{
		SDL_assert(minVal <= maxVal);

		if (arg < (T)minVal)
		{
			arg = (T)minVal;
		}

		if (arg > (T)maxVal)
		{
			arg = (T)maxVal;
		}
	}

	template <class T, class U, class V>
	T Clamped(T arg, const U& minVal, const V& maxVal)
	{
		Clamp(arg, minVal, maxVal);
		return arg;
	}

	template <class T, class V>
	inline void ClampMax(T& arg, const V& maxVal)
	{
		if (arg > (T)maxVal)
		{
			arg = (T)maxVal;
		}
	}

	template <class T, class V>
	inline void ClampMin(T& arg, const V& minVal)
	{
		if (arg < (T)minVal)
		{
			arg = (T)minVal;
		}
	}

	[[nodiscard]] inline float Lerp(float from, float to, float t)
	{
		return (from)+(to - from) * t;
	}


	[[nodiscard]] inline float SmoothDamp(float current, float target, float dampening) {
		return ((current * (dampening - 1)) + target) / dampening; //weighted average formula
	}

	//rounds a float up or down depending on its value
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

	//rounds a double up or down depending on whether its
	//mantissa is higher or lower than offset
	[[nodiscard]] inline int RoundUnderOffset(float val, float offset)
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
	[[nodiscard]] inline bool IsNearlyEqual(float a, float b, float margin = 1E-12f)
	{
		return (fabs(a - b) < margin);
	}

	[[nodiscard]] inline bool IsNearlyEqual(double a, double b, double margin = 1E-12)
	{
		return (fabs(a - b) < margin);
	}


	template <class T>
	[[nodiscard]] inline double Average(const std::vector<T>& v)
	{
		double average = 0.0;

		for (unsigned int i = 0; i < v.size(); ++i)
		{
			average += (double)v[i];
		}

		return average / (double)v.size();
	}

	[[nodiscard]] inline double StandardDeviation(const std::vector<double>& v)
	{
		double sd = 0.0;
		double average = Average(v);

		for (unsigned int i = 0; i < v.size(); ++i)
		{
			sd += (v[i] - average) * (v[i] - average);
		}

		sd = sd / v.size();

		return sqrt(sd);
	}

	[[nodiscard]] inline int fastfloor(float x) { return x > 0 ? (int)x : (int)x - 1; }

	struct Range {
		float min;
		float max;
	};

	[[nodiscard]] inline Range SortTwo(float a, float b) {
		if (a > b)
		{
			return Range{ b, a };
		}
		else
		{
			return Range{ a, b };
		}
	}

	[[nodiscard]] inline float Sign(float value)
	{
		return value >= 0? 1.f : -1.f;
	}

	[[nodiscard]] inline float Norm(float value)
	{
		return value * Sign(value);
	}

	[[nodiscard]] inline int DivCeil(int a, int b)
	{
		return (a + b - 1) / b;
	}

}
