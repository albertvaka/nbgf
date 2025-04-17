#pragma once

#include "vec.h"
#include "angles.h"

//-----------------------------------------------------------------------------
//  Transform used for Tiled imports
//-----------------------------------------------------------------------------

struct Transform : public vec {
	constexpr Transform(vec pos, float rotationDegs) : vec(pos), rotationDegs(rotationDegs) { Angles::ClampDegsBetween0and360(rotationDegs); }
	constexpr Transform(float x, float y, float rotationDegs) : Transform(vec(x, y), rotationDegs) {}
	float rotationDegs;

	constexpr Transform operator-() const
	{
		return Transform(-x, -y, 360 - rotationDegs);
	}

	constexpr Transform& operator+=(Transform rhs)
	{
		x += rhs.x;
		y += rhs.y;
		rotationDegs += rhs.rotationDegs;
		Angles::ClampDegsBetween0and360(rotationDegs);
		return *this;
	}

	constexpr Transform& operator+=(vec rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	constexpr Transform& operator-=(Transform rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		rotationDegs -= rhs.rotationDegs;
		Angles::ClampDegsBetween0and360(rotationDegs);
		return *this;
	}

	constexpr Transform& operator*=(const float& rhs)
	{
		x *= rhs;
		y *= rhs;
		rotationDegs *= rhs;
		Angles::ClampDegsBetween0and360(rotationDegs);
		return *this;
	}

	constexpr Transform& operator/=(const float& rhs)
	{
		x /= rhs;
		y /= rhs;
		rotationDegs /= rhs;
		Angles::ClampDegsBetween0and360(rotationDegs);
		return *this;
	}

	constexpr bool operator==(Transform rhs) const
	{
		return (x == rhs.x) && (y == rhs.y) && (rotationDegs == rhs.rotationDegs);
	}

	constexpr bool operator!=(Transform rhs) const
	{
		return (x != rhs.x) || (y != rhs.y) || (rotationDegs != rhs.rotationDegs);
	}

};

//------------------------------------------------------------------------operator overloads
inline constexpr Transform operator*(Transform lhs, float rhs)
{
	Transform result(lhs);
	result *= rhs;
	return result;
}

inline constexpr Transform operator*(float lhs, Transform rhs)
{
	Transform result(rhs);
	result *= lhs;
	return result;
}

inline constexpr Transform operator-(Transform lhs, Transform rhs)
{
	Transform result(lhs);
	result -= rhs;
	return result;
}

inline constexpr Transform operator+(Transform lhs, Transform rhs)
{
	Transform result(lhs);
	result += rhs;
	return result;
}

inline constexpr Transform operator+(Transform lhs, vec rhs)
{
	Transform result(lhs);
	result += rhs;
	return result;
}

inline constexpr Transform operator/(Transform lhs, float rhs)
{
	Transform result(lhs);
	result /= rhs;
	return result;
}
