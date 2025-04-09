#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include <iosfwd>
#include <limits>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#ifndef _USE_MATH_DEFINES 
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "angles.h"

struct veci {
	int x, y;
	constexpr veci(int a, int b) : x(a), y(b) {}
	constexpr bool operator==(veci rhs) const
	{
		return (x == rhs.x) && (y == rhs.y);
	}
};

struct vec
{
	float x, y;
	explicit constexpr vec() : x(0.f), y(0.f) {}
	explicit constexpr vec(float xy) : x(xy), y(xy) {}
	constexpr vec(float x, float y) : x(x), y(y) {}
	constexpr vec(veci v) : x(v.x), y(v.y) {}

	static const vec Zero;

	// Angle to direction
	[[nodiscard]] static vec FromAngleRads(float rads, float len = 1.0f) {
		return vec(cos(rads) * len, sin(rads) * len);
	}

	// Angle to direction
	[[nodiscard]] static vec FromAngleDegs(float degs, float len = 1.0f) {
		return FromAngleRads(Angles::DegsToRads(degs), len);
	}

	// Returns the length of the vector
	[[nodiscard]] inline float Length() const;

	// Returns the squared length of the vector (thereby avoiding the sqrt)
	[[nodiscard]] inline float LengthSq() const;

	inline void Normalize();
	[[nodiscard]] inline vec Normalized() const;

	// Returns the length of the projection of one vector onto the other (order doesn't matter, it's symmetrical).
	// For normalize vectors, it can be used to know how alligned the directions of both vectors are:
	// Perpendicular -> dot product = 0
	// Parallel, same direction -> dot product = 1
	// Parallel, opposite direction -> dot product = -1
	// For normalized vectors, the result is the cosine of the angle between the two vectors,
	// but to get the angle prefer using AngleRadsBetween than the more expensive acos.
	[[nodiscard]] inline float Dot(vec v2) const;

	// In 2d the cross product is a scalar, not a perpendicular vector like in 3d.
	// For normalized vectors, the result is the sine of the angle between the two vectors,
	// but to get the angle prefer using AngleRadsBetween than the more expensive asin.
	// The sign of the cross product tells the direction of rotation from a vector to another.
	[[nodiscard]] inline float Cross(vec v2) const;

	void Clamp(vec minv, vec maxv)
	{
		if (x > maxv.x) x = maxv.x;
		else if (x < minv.x) x = minv.x;
		if (y > maxv.y) y = maxv.y;
		else if (y < minv.y) y = minv.y;
	}

	[[nodiscard]] vec Clamped(vec minv, vec maxv)
	{
		vec ret = *this;
		ret.Clamp(minv, maxv);
		return ret;
	}

	[[nodiscard]] vec Mirrored(bool mirror_x, bool mirror_y) const {
		return vec(mirror_x ? -x : x, mirror_y ? -y : y);
	}

	void Mirror(bool mirror_x, bool mirror_y) {
		*this = Mirrored(mirror_x, mirror_y);
	}

	// Returns positive if v2 is clockwise of this vector,
	// minus if anticlockwise (Y axis pointing down, X axis to right)
	[[nodiscard]] inline int Sign(vec v2) const;

	// Angle in radians between bettween this and the positive X axis from the specified origin, in the range [-180, 180]. Doesn't need normalized vectors.
	[[nodiscard]] float AngleRads(vec origin = vec::Zero) const
	{
		return std::atan2(y - origin.y, x - origin.x);
	}

	// Angle in degrees between bettween this and the positive X axis from the specified origin, in the range [-Tau/2, Tau/2]. Doesn't need normalized vectors.
	[[nodiscard]] float AngleDegs(vec origin = vec::Zero) const
	{
		return Angles::RadsToDegs(AngleRads(origin));
	}

	// Smallest angle between the two vectors, in radians. Signed, in range [-Tau/2, Tau/2]. Needs normalized vectors or vectors of the same length.
	[[nodiscard]] float AngleRadsBetween(vec other) const
	{
		// works because atan2(sin(angle), cos(angle)) = angle
		return std::atan2(Cross(other), Dot(other));
	}

	// Smallest angle between the two vectors, in degrees, always positive in range [-180, 180]. Needs normalized vectors or vectors of the same length.
	[[nodiscard]] float AngleDegsBetween(vec other) const
	{
		return Angles::RadsToDegs(AngleRadsBetween(other));
	}

	[[nodiscard]] vec RotatedAroundOriginRads(float rads) const
	{
		float cs = cos(rads);
		float sn = sin(rads);
		return vec(x * cs - y * sn, x * sn + y * cs);
	}

	[[nodiscard]] vec RotatedAroundOriginDegs(float degrees) const
	{
		return RotatedAroundOriginRads(Angles::DegsToRads(degrees));
	}

	void RotateAroundOriginRads(float rads)
	{
		*this = RotatedAroundOriginRads(rads);
	}

	void RotateAroundOriginDegs(float degrees)
	{
		*this = RotatedAroundOriginDegs(degrees);
	}

	// Note: If specified, maxTurnRate should to be multiplied by dt
	[[nodiscard]] vec RotatedToFacePositionRads(vec target, float maxTurnRateRads = std::numeric_limits<float>::max()) const;
	[[nodiscard]] inline vec RotatedToFacePositionDegs(vec target) const {
		return RotatedToFacePositionRads(target);
	}
	[[nodiscard]] inline vec RotatedToFacePositionDegs(vec target, float maxTurnRateDegs) const {
		return RotatedToFacePositionRads(target, Angles::DegsToRads(maxTurnRateDegs));
	}
	//returns the vector that is perpendicular to this one.
	[[nodiscard]] inline vec Perp() const;

	//adjusts x and y so that the length of the vector does not exceed max
	inline bool Truncate(float max); // affects in-place
	[[nodiscard]] inline vec Truncated(float max); // returns a new vec

	//returns the distance between this vector and th one passed as a parameter
	[[nodiscard]] inline float Distance(vec v2) const;

	//squared version of above.
	[[nodiscard]] inline float DistanceSq(vec v2) const;

	[[nodiscard]] inline vec ManhattanDistance(vec v2) const;

	constexpr vec operator+=(vec rhs)
	{
		x += rhs.x;
		y += rhs.y;

		return *this;
	}

	constexpr vec operator-=(vec rhs)
	{
		x -= rhs.x;
		y -= rhs.y;

		return *this;
	}

	constexpr vec operator*=(const float& rhs)
	{
		x *= rhs;
		y *= rhs;

		return *this;
	}

	// Component-wise (aka dot) product, like in GLSL
	constexpr vec operator*=(const vec& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;

		return *this;
	}

	constexpr vec operator/=(const float& rhs)
	{
		x /= rhs;
		y /= rhs;

		return *this;
	}

	constexpr vec operator/=(const vec& rhs)
	{
		x /= rhs.x;
		y /= rhs.y;

		return *this;
	}

	constexpr bool operator==(vec rhs) const
	{
		return (x == rhs.x) && (y == rhs.y);
	}

	constexpr bool operator!=(vec rhs) const
	{
		return (x != rhs.x) || (y != rhs.y);
	}

	constexpr vec operator-() const
	{
		return vec(-x, -y);
	}

	void DebugDrawAsArrow(vec from, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) const
#ifdef _DEBUG
		;
#else
	{}
#endif

	template<typename T> void DebugDrawAsArrow(vec from, T c) const { 
		DebugDrawAsArrow(from, c.r, c.g, c.b); 
	}

	void DebugDraw(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) const
#ifdef _DEBUG
	;
#else
	{}
#endif

	template<typename T> void DebugDraw(T c) const {
		DebugDraw(c.r, c.g, c.b); 
	}
};

inline constexpr vec vec::Zero = vec(0,0);

inline float vec::Length() const
{
	return sqrt(x * x + y * y);
}

inline float vec::LengthSq() const
{
	return (x * x + y * y);
}

inline float vec::Dot(vec v2) const
{
	return x*v2.x + y*v2.y;
}

inline float vec::Cross(vec v2) const
{
	return x*v2.y - y*v2.x;
}

enum {clockwise = 1, anticlockwise = -1};
inline int vec::Sign(vec v2) const
{
	if (y*v2.x > x*v2.y)
	{
		return anticlockwise;
	}
	else
	{
		return clockwise;
	}
}

//  Returns a vector perpendicular to this vector
inline vec vec::Perp() const
{
	return vec(-y, x);
}

//  calculates the euclidean distance between two vectors
inline float vec::Distance(vec v2) const
{
	float ySeparation = v2.y - y;
	float xSeparation = v2.x - x;

	return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}


//  calculates the euclidean distance squared between two vectors
inline float vec::DistanceSq(vec v2) const
{
	float ySeparation = v2.y - y;
	float xSeparation = v2.x - x;

	return ySeparation*ySeparation + xSeparation*xSeparation;
}

inline vec vec::ManhattanDistance(vec v2) const
{
	return vec(
		fabs(v2.x - x),
		fabs(v2.y - y)
	);
}

//  truncates a vector so that its length does not exceed max
inline bool vec::Truncate(float max)
{
	if (this->Length() > max)
	{
		this->Normalize();

		*this *= max;
		return true;
	}
	return false;
}

inline vec vec::Truncated(float max)
{
	if (this->Length() > max)
	{
		vec ret = this->Normalized();

		ret *= max;
		return ret;
	}
	return *this;
}

inline void vec::Normalize()
{
	float vector_length = this->Length();

	if (vector_length > std::numeric_limits<float>::epsilon())
	{
		this->x /= vector_length;
		this->y /= vector_length;
	}
}
inline vec vec::Normalized() const
{
	float vector_length = this->Length();

	vec res;

	if (vector_length > std::numeric_limits<float>::epsilon())
	{
		res.x = this->x / vector_length;
		res.y = this->y / vector_length;
	}

	return res;
}


//------------------------------------------------------------------------non member functions

[[nodiscard]] inline float Dot(vec v1, vec v2)
{
	return v1.x*v2.x + v1.y*v2.y;
}
[[nodiscard]] inline float Cross(vec v1, vec v2)
{
	return v1.x*v2.y - v1.y*v2.x;
}

[[nodiscard]] inline float Distance(vec v1, vec v2)
{
	float ySeparation = v2.y - v1.y;
	float xSeparation = v2.x - v1.x;

	return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}

[[nodiscard]] inline float DistanceSq(vec v1, vec v2)
{
	float ySeparation = v2.y - v1.y;
	float xSeparation = v2.x - v1.x;

	return ySeparation*ySeparation + xSeparation*xSeparation;
}

[[nodiscard]] inline float AngleRadsBetween(vec v1, vec v2)
{
	return v1.AngleRadsBetween(v2);
}

[[nodiscard]] inline float AngleDegsBetween(vec v1, vec v2)
{
	return v1.AngleDegsBetween(v2);
}

//------------------------------------------------------------------------operator overloads
inline constexpr vec operator*(vec lhs, float rhs)
{
	vec result(lhs);
	result *= rhs;
	return result;
}

inline constexpr vec operator*(float lhs, vec rhs)
{
	vec result(rhs);
	result *= lhs;
	return result;
}

// Component-wise product (like in GLSL)
inline constexpr vec operator*(vec lhs, vec rhs)
{
	return vec(lhs.x * rhs.x, lhs.y * rhs.y);
}

inline constexpr vec operator-(vec lhs, vec rhs)
{
	vec result(lhs);
	result -= rhs;
	return result;
}

inline constexpr vec operator+(vec lhs, vec rhs)
{
	vec result(lhs);
	result += rhs;
	return result;
}

inline constexpr vec operator/(vec lhs, float rhs)
{
	vec result(lhs);
	result /= rhs;
	return result;
}

inline constexpr vec operator/(vec lhs, vec rhs)
{
	vec result(lhs);
	result /= rhs;
	return result;
}

///////////////////////////////////////////////////////////////////////////////

inline constexpr veci operator+(veci lhs, veci rhs)
{
	veci result(lhs);
	result.x += rhs.x;
	result.y += rhs.y;

	return result;
}

inline constexpr veci operator-(veci lhs, veci rhs)
{
	veci result(lhs);
	result.x -= rhs.x;
	result.y -= rhs.y;

	return result;
}

///////////////////////////////////////////////////////////////////////////////


//treats a window as a toroid
inline void WrapAround(vec &pos, float MaxX, float MaxY)
{
	if (pos.x > MaxX) {pos.x = 0.f;}
	if (pos.x < 0.f)  {pos.x = MaxX;}
	if (pos.y < 0.f)  {pos.y = MaxY;}
	if (pos.y > MaxY) {pos.y = 0.f;}
}

//  returns true if the target position is in the field of view of the entity
//  positioned at posFirst facing in facingFirst
inline bool IsSecondInFOVOfFirst(vec posFirst,
								 vec facingFirst,
								 vec posSecond,
								 float	fov)
{
	vec toTarget = (posSecond - posFirst);
	toTarget.Normalize();
	return facingFirst.Dot(toTarget) >= cos(fov/2.0f);
}

//-------------------- LineIntersection2D-------------------------
//
//	Given 2 lines in 2D space AB, CD this returns true if an
//	intersection occurs and sets dist to the distance the intersection
//  occurs along AB. Also sets the 2d vector point to the point of
//  intersection
//-----------------------------------------------------------------
inline bool LineIntersection2D(vec A, vec B, vec C, vec D, float& dist, vec& point)
{

	float rTop = (A.y-C.y)*(D.x-C.x)-(A.x-C.x)*(D.y-C.y);
	float rBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);

	float sTop = (A.y-C.y)*(B.x-A.x)-(A.x-C.x)*(B.y-A.y);
	float sBot = (B.x-A.x)*(D.y-C.y)-(B.y-A.y)*(D.x-C.x);

	if ( (rBot == 0) || (sBot == 0))
	{
		//lines are parallel
		return false;
	}

	float r = rTop/rBot;
	float s = sTop/sBot;

	if( (r > 0) && (r < 1) && (s > 0) && (s < 1) )
	{
		dist = A.Distance(B) * r;
		point = A + r * (B - A);
		return true;
	}
	else
	{
		dist = 0;
		return false;
	}
}

// Lerp for vecs

namespace Mates {
	[[nodiscard]] inline vec Lerp(vec from, vec to, float t)
	{
		return vec(Lerp(from.x, to.x, t), Lerp(from.y, to.y, t));
	}
}

//-----------------------------------------------------------------------------
//  printing
//-----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& os, vec rhs)
{
	os << rhs.x << "," << rhs.y;
	return os;
}

//-----------------------------------------------------------------------------
//  Transform used for Tiled imports
//-----------------------------------------------------------------------------

struct Transform : public vec {
	constexpr Transform(vec pos, float rotationDegs) : vec(pos), rotationDegs(rotationDegs) { Angles::ClampDegsBetween0and360(rotationDegs); }
	constexpr Transform(float x, float y, float rotationDegs) : Transform(vec(x, y), rotationDegs) { }
	float rotationDegs;

	constexpr Transform operator-() const
	{
		return Transform(-x, -y, 360-rotationDegs);
	}

	constexpr Transform operator+=(Transform rhs)
	{
		x += rhs.x;
		y += rhs.y;
		rotationDegs += rhs.rotationDegs;
		Angles::ClampDegsBetween0and360(rotationDegs);
		return *this;
	}

	constexpr Transform operator+=(vec rhs)
	{
		x += rhs.x;
		y += rhs.y;
		return *this;
	}

	constexpr Transform operator-=(Transform rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		rotationDegs -= rhs.rotationDegs;
		Angles::ClampDegsBetween0and360(rotationDegs);
		return *this;
	}

	constexpr Transform operator*=(const float& rhs)
	{
		x *= rhs;
		y *= rhs;
		rotationDegs *= rhs;
		Angles::ClampDegsBetween0and360(rotationDegs);
		return *this;
	}

	constexpr Transform operator/=(const float& rhs)
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
		return (x != rhs.x) || (y != rhs.y) || (rotationDegs == rhs.rotationDegs);
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
