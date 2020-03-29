#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <cassert>

namespace Mates
{
	struct xy
	{
		int x = 0;
		int y = 0;
		xy() {}
		xy(int _x_, int _y_)
		{
			x = _x_;
			y = _y_;
		}
	};


	//a few useful constants
	const int     MaxInt = (std::numeric_limits<int>::max)();
	const int     MinInt = (std::numeric_limits<int>::min)();
	const double  MaxDouble = (std::numeric_limits<double>::max)();
	const double  MinDouble = (std::numeric_limits<double>::min)();
	const float   MaxFloat = (std::numeric_limits<float>::max)();
	const float   MinFloat = (std::numeric_limits<float>::min)();

	const float   Pi = 3.141592653589793238462643383279502884f;
	const float   TwoPi = Pi * 2;
	const float   HalfPi = Pi / 2;
	const float   QuarterPi = Pi / 4;

	inline float DegsToRads(float degs)
	{
		return TwoPi * (degs / 360.0f);
	}

	inline float RadsToDegs(float rads)
	{
		return (rads * 360.0f) / TwoPi;
	}

	//returns true if the parameter is equal to zero
	inline bool IsZero(float val)
	{
	return ((-MinFloat < val) && (val < MinFloat));
	}

	//returns true is the third parameter is in the range described by the
	//first two
	inline bool InRange(float start, float end, float val)
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


	//----------------------------------------------------------------------------
	//  some random number functions.
	//----------------------------------------------------------------------------

	//returns a random integer between x and y
	inline int   RandInt(int min, int max)
	{
	assert(max >= min && "<RandInt>: max is less than min");
	return rand() % (max - min + 1) + min;
	}

	//returns a random float between zero and 1
	inline float RandFloat() { return ((rand()) / (RAND_MAX + 1.0f)); }

	inline float RandInRange(float x, float y)
	{
	return x + RandFloat() * (y - x);
	}

	//returns a random bool
	inline bool   RandBool()
	{
	if (RandFloat() > 0.5) return true;

	else return false;
	}

	//returns a random float in the range -1 < n < 1
	inline float RandomClamped() { return RandFloat() - RandFloat(); }


	//returns a random number with a normal distribution. See method at
	//http://www.taygeta.com/random/gaussian.html
	inline float RandGaussian(float mean = 0.0, float standard_deviation = 1.0)
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
				x1 = 2.0f * RandFloat() - 1.0f;
				x2 = 2.0f * RandFloat() - 1.0f;
				w = x1 * x1 + x2 * x2;
			} while (w >= 1.0f);

			w = sqrt((-2.0f * log(w)) / w);
			y1 = x1 * w;
			y2 = x2 * w;
			use_last = 1;
		}

		return(mean + y1 * standard_deviation);
	}



	//-----------------------------------------------------------------------
	//  
	//  some handy little functions
	//-----------------------------------------------------------------------


	inline float Sigmoid(float input, float response = 1.0)
	{
		return (1.0f / (1.0f + exp(-input / response)));
	}


	//returns the maximum of two values
	template <class T>
	inline T MaxOf(const T& a, const T& b)
	{
		if (a > b) return a;
		return b;
	}

	//returns the minimum of two values
	template <class T>
	inline T MinOf(const T& a, const T& b)
	{
		if (a < b) return a;
		return b;
	}


	//clamps the first argument between the second two
	template <class T, class U, class V>
	inline void Clamp(T& arg, const U& minVal, const V& maxVal)
	{
#if _DEBUG
		assert(((double)minVal < (double)maxVal) && "<Clamp>MaxVal < MinVal!");
#endif

		if (arg < (T)minVal)
		{
			arg = (T)minVal;
		}

		if (arg > (T)maxVal)
		{
			arg = (T)maxVal;
		}
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

	inline float Lerp(float from, float to, float t)
	{
		if (t > 1) t = 1;
		if (t < -1) t = -1;
		return (from)+(to - from) * t;
	}

	//rounds a float up or down depending on its value
	inline int Rounded(float val)
	{
		int    integral = (int)val;
		float mantissa = val - integral;

		if (mantissa < 0.5)
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
	inline int RoundUnderOffset(float val, float offset)
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
	inline bool isNearlyEqual(float a, float b, float margin = 1E-12)
	{
		if (fabs(a - b) < margin)
		{
			return true;
		}

		return false;
	}

	inline bool isNearlyEqual(double a, double b, double margin = 1E-12)
	{
		if (fabs(a - b) < margin)
		{
			return true;
		}

		return false;
	}


	template <class T>
	inline double Average(const std::vector<T>& v)
	{
		double average = 0.0;

		for (unsigned int i = 0; i < v.size(); ++i)
		{
			average += (double)v[i];
		}

		return average / (double)v.size();
	}


	inline double StandardDeviation(const std::vector<double>& v)
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
}