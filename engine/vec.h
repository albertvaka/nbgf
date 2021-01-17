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
};

struct vec
{
	float x, y;
	explicit constexpr vec() : x(0.f), y(0.f) {}
	explicit constexpr vec(float xy) : x(xy), y(xy) {}
	constexpr vec(float x, float y) : x(x), y(y) {}
	constexpr vec(const veci& v) : x(v.x), y(v.y) {}

	static const vec Zero;

	//Angle to direction
	[[nodiscard]] static vec FromAngleRads(float rads, float len=1.0f) {
		return vec(cos(rads)*len,sin(rads)*len);
	}

	//Angle to direction
	[[nodiscard]] static vec FromAngleDegs(float degs, float len = 1.0f) {
		return FromAngleRads(Angles::DegsToRads(degs), len);
	}


	//returns the length of the vector
	[[nodiscard]] inline float    Length()const;

	//returns the squared length of the vector (thereby avoiding the sqrt)
	[[nodiscard]] inline float    LengthSq()const;

	inline void      Normalize();
	[[nodiscard]] inline vec      Normalized() const;

	// Dot product: returns the length of the projection of one vector onto the other (order doesn't matter, it's symmetrical)
	// It can be used to know how alligned the directions of both vectors are, assuming they are normalized:
	// Perpendicular -> dot product = 0
	// Parallel, same direction -> dot product = 1
	// Parallel, opposite direction -> dot product = -1
	[[nodiscard]] inline float    Dot(vec v2) const;

	[[nodiscard]] inline float    Cross(vec v2) const;

	void Clamp(vec minv, vec maxv)
	{
		if (x > maxv.x) x = maxv.x;
		else if (x < minv.x) x = minv.x;
		if (y > maxv.y) y = maxv.y;
		else if (y < minv.y) y = minv.y;
	}

	[[nodiscard]] std::string ToString() const {
		std::stringstream stream;
		stream << std::fixed << std::setprecision(2) << x << "," << y;
		return stream.str();
	}


	//returns positive if v2 is clockwise of this vector,
	//negative if anticlockwise (assuming the Y axis is pointing down,
	//X axis to right like a Window app)
	[[nodiscard]] inline int       Sign(vec v2) const;

	// Angle in Rads between the lines (origin to point a) and (origin to point b)
	// In range [-Pi,Pi]
	[[nodiscard]] float AngleRads(vec other = vec::Zero) const
	{
		float deltaY = other.y - y;
		float deltaX = other.x - x;
		return atan2(deltaY, deltaX);
	}

	// Angle in Degrees between the lines (origin to point a) and (origin to point b)
	// In range [-180,180]
	[[nodiscard]] float AngleDegs(vec other = vec::Zero) const
	{
		return Angles::RadsToDegs(AngleRads(other));
	}

	[[nodiscard]] vec RotatedAroundOriginRads(float rads) const
	{
		float cs = cos(rads);
		float sn = sin(rads);
		return vec(x * cs - y * sn, x * sn + y * cs);
	}

	[[nodiscard]] vec RotatedAroundOriginDegs(float degrees) 
	{
		return RotatedAroundOriginRads(Angles::DegsToRads(degrees));
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
	[[nodiscard]] inline vec  Perp() const;

	//adjusts x and y so that the length of the vector does not exceed max
	inline bool      Truncate(const float max); // affects in-place
	[[nodiscard]] inline vec      Truncated(const float max); // returns a new vec

	//returns the distance between this vector and th one passed as a parameter
	[[nodiscard]] inline float    Distance(vec v2) const;

	//squared version of above.
	[[nodiscard]] inline float    DistanceSq(vec v2) const;

	//we need some overloaded operators
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

	constexpr const vec operator-() const
	{
		return vec(-x, -y);
	}

	void DebugDrawAsArrow(vec from, uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) const
#ifdef _DEBUG
		;
#else
	{}
#endif

	void DebugDraw(uint8_t r = 255, uint8_t g = 255, uint8_t b = 255) const
#ifdef _DEBUG
	;
#else
	{}
#endif
};

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

//  returns positive if v2 is clockwise of this vector,
//  minus if anticlockwise (Y axis pointing down, X axis to right)
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

//  normalizes a 2D Vector
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

inline float Distance(vec v1, vec v2)
{
	float ySeparation = v2.y - v1.y;
	float xSeparation = v2.x - v1.x;

	return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}

inline float DistanceSq(vec v1, vec v2)
{
	float ySeparation = v2.y - v1.y;
	float xSeparation = v2.x - v1.x;

	return ySeparation*ySeparation + xSeparation*xSeparation;
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

inline constexpr vec operator-(vec lhs, vec rhs)
{
	vec result(lhs);
	result.x -= rhs.x;
	result.y -= rhs.y;

	return result;
}

inline constexpr vec operator+(vec lhs, vec rhs)
{
	vec result(lhs);
	result.x += rhs.x;
	result.y += rhs.y;

	return result;
}

inline constexpr vec operator/(vec lhs, float val)
{
	vec result(lhs);
	result.x /= val;
	result.y /= val;

	return result;
}

inline constexpr vec operator/(vec lhs, vec rhs)
{
	vec result(lhs);
	result.x /= rhs.x;
	result.y /= rhs.y;

	return result;
}

///////////////////////////////////////////////////////////////////////////////


//treats a window as a toroid
inline void WrapAround(vec &pos, int MaxX, int MaxY)
{
	if (pos.x > MaxX) {pos.x = 0.0;}
	if (pos.x < 0)    {pos.x = (float)MaxX;}
	if (pos.y < 0)    {pos.y = (float)MaxY;}
	if (pos.y > MaxY) {pos.y = 0.0;}
}

//  returns true if the target position is in the field of view of the entity
//  positioned at posFirst facing in facingFirst
inline bool IsSecondInFOVOfFirst(vec posFirst,
                                 vec facingFirst,
                                 vec posSecond,
                                 float    fov)
{
	vec toTarget = (posSecond - posFirst);
	toTarget.Normalize();
	return facingFirst.Dot(toTarget) >= cos(fov/2.0);
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

//-----------------------------------------------------------------------------
//  printing
//-----------------------------------------------------------------------------

inline std::ostream& operator<<(std::ostream& os, vec rhs)
{
	os << rhs.x << "," << rhs.y;
	return os;
}
