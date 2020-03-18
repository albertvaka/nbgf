#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include <math.h>
#include <iosfwd>
#include <limits>
#include <fstream>

#include <SFML/System/Vector2.hpp>

#include "mates.h"
#include "rand.h"

struct vec : public sf::Vector2f
{
  vec():sf::Vector2f(0.0,0.0){}
  vec(float a, float b):sf::Vector2f(a,b){}
  vec(sf::Vector2u v):sf::Vector2f(float(v.x),float(v.y)){}
  vec(sf::Vector2i v):sf::Vector2f(float(v.x),float(v.y)){}
  vec(sf::Vector2f v):sf::Vector2f(v.x,v.y){}

  static vec Rand(float minX, float minY, float maxX, float maxY) {
      return vec(Random::rollf(minX, maxX), Random::rollf(minY, maxY));
  }
  
  bool Equals(float px, float py) 
  {
      return x == px && y == py;
  }
  //sets x and y to zero
  void Zero(){x=0.0; y=0.0;}

  //returns true if both x and y are zero
  bool isZero()const{return (x*x + y*y) < Mates::MinFloat;}

  //returns the length of the vector
  inline float    Length()const;

  //returns the squared length of the vector (thereby avoiding the sqrt)
  inline float    LengthSq()const;

  inline void      Normalize();
  inline vec      Normalized();

  inline float    Dot(const vec& v2) const;

  inline float    Cross(const vec& v2) const;

  void Clamp(const vec& minv, const vec& maxv) 
  {
      if (x > maxv.x) x = maxv.x;
      else if (x < minv.x) x = minv.x;
      if (y > maxv.y) y = maxv.y;
      else if (y < minv.y) y = minv.y;
  }

  std::string ToString() const {
      std::stringstream stream;
      stream << std::fixed << std::setprecision(2) << x << "," << y;
      return stream.str();
  }


  //returns positive if v2 is clockwise of this vector,
  //negative if anticlockwise (assuming the Y axis is pointing down,
  //X axis to right like a Window app)
  inline int       Sign(const vec& v2) const;

  float Angle(const vec& other) const
  {
    float deltaY = other.y - y;
	float deltaX = other.x - x;
    return atan2(deltaY, deltaX);
  }

  //returns the vector that is perpendicular to this one.
  inline vec  Perp() const;

  //adjusts x and y so that the length of the vector does not exceed max
  inline void      Truncate(float max);

  //returns the distance between this vector and th one passed as a parameter
  inline float    Distance(const vec &v2) const;

  //squared version of above.
  inline float    DistanceSq(const vec &v2) const;

  inline void      Reflect(const vec& norm);

  //returns the vector that is the reverse of this vector
  inline vec  GetReverse() const;


  //we need some overloaded operators
  const vec& operator+=(const vec &rhs)
  {
    x += rhs.x;
    y += rhs.y;

    return *this;
  }

  const vec& operator-=(const vec &rhs)
  {
    x -= rhs.x;
    y -= rhs.y;

    return *this;
  }

  const vec& operator*=(const float& rhs)
  {
    x *= rhs;
    y *= rhs;

    return *this;
  }

  const vec& operator/=(const float& rhs)
  {
    x /= rhs;
    y /= rhs;

    return *this;
  }

  bool operator==(const vec& rhs) const
  {
    return (Mates::isNearlyEqual(x, rhs.x) && Mates::isNearlyEqual(y,rhs.y) );
  }

  bool operator!=(const vec& rhs) const
  {
    return (x != rhs.x) || (y != rhs.y);
  }
  
};


//------------------------------------------------------------------------member functions

//  returns the length of a 2D vector
inline float vec::Length() const
{
  return sqrt(x * x + y * y);
}

//  returns the squared length of a 2D vector
inline float vec::LengthSq() const
{
  return (x * x + y * y);
}


//  calculates the dot product
inline float vec::Dot(const vec &v2) const
{
  return x*v2.x + y*v2.y;
}

//  calculates the cross product
inline float vec::Cross(const vec &v2) const
{
  return x*v2.y - y*v2.x;
}

//  returns positive if v2 is clockwise of this vector,
//  minus if anticlockwise (Y axis pointing down, X axis to right)
enum {clockwise = 1, anticlockwise = -1};

inline int vec::Sign(const vec& v2) const
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
inline float vec::Distance(const vec &v2) const
{
  float ySeparation = v2.y - y;
  float xSeparation = v2.x - x;

  return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}


//  calculates the euclidean distance squared between two vectors 
inline float vec::DistanceSq(const vec &v2) const
{
  float ySeparation = v2.y - y;
  float xSeparation = v2.x - x;

  return ySeparation*ySeparation + xSeparation*xSeparation;
}

//  truncates a vector so that its length does not exceed max
inline void vec::Truncate(float max)
{
  if (this->Length() > max)
  {
    this->Normalize();

    *this *= max;
  } 
}

//  returns the vector that is the reverse of this vector
inline vec vec::GetReverse() const
{
  return vec(-this->x, -this->y);
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
inline vec vec::Normalized()
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

inline vec Normalize(const vec &v)
{
  vec vec = v;

  float vector_length = vec.Length();

  if (vector_length > std::numeric_limits<float>::epsilon())
  {
    vec.x /= vector_length;
    vec.y /= vector_length;
  }

  return vec;
}


inline float Distance(const vec &v1, const vec &v2)
{

  float ySeparation = v2.y - v1.y;
  float xSeparation = v2.x - v1.x;

  return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
}

inline float DistanceSq(const vec &v1, const vec &v2)
{

  float ySeparation = v2.y - v1.y;
  float xSeparation = v2.x - v1.x;

  return ySeparation*ySeparation + xSeparation*xSeparation;
}

inline float Length(const vec& v)
{
  return sqrt(v.x*v.x + v.y*v.y);
}

inline float LengthSq(const vec& v)
{
  return (v.x*v.x + v.y*v.y);
}


//------------------------------------------------------------------------operator overloads
inline vec operator*(const vec &lhs, float rhs)
{
  vec result(lhs);
  result *= rhs;
  return result;
}

inline vec operator*(float lhs, const vec &rhs)
{
  vec result(rhs);
  result *= lhs;
  return result;
}

inline vec operator*(const vec &lhs, const vec &rhs)
{
	vec result(rhs);
	result.x = lhs.x * rhs.x;
	result.y = lhs.y * rhs.y;
	return result;
}

inline vec operator-(const vec &lhs, const vec &rhs)
{
  vec result(lhs);
  result.x -= rhs.x;
  result.y -= rhs.y;
  
  return result;
}

inline vec operator+(const vec &lhs, const vec &rhs)
{
  vec result(lhs);
  result.x += rhs.x;
  result.y += rhs.y;
  
  return result;
}

inline vec operator/(const vec &lhs, float val)
{
  vec result(lhs);
  result.x /= val;
  result.y /= val;

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

//returns true if the point p is not inside the region defined by top_left
//and bot_rgt
inline bool NotInsideRegion(vec p,
                            vec top_left,
                            vec bot_rgt)
{
  return (p.x < top_left.x) || (p.x > bot_rgt.x) || 
         (p.y < top_left.y) || (p.y > bot_rgt.y);
}

inline bool InsideRegion(vec p,
                         vec top_left,
                         vec bot_rgt)
{
  return !((p.x < top_left.x) || (p.x > bot_rgt.x) || 
         (p.y < top_left.y) || (p.y > bot_rgt.y));
}

inline bool InsideRegion(vec p, int left, int top, int right, int bottom)
{
  return !( (p.x < left) || (p.x > right) || (p.y < top) || (p.y > bottom) );
}

//  returns true if the target position is in the field of view of the entity
//  positioned at posFirst facing in facingFirst
inline bool isSecondInFOVOfFirst(vec posFirst,
                                 vec facingFirst,
                                 vec posSecond,
                                 float    fov)
{
  vec toTarget = Normalize(posSecond - posFirst);

  return facingFirst.Dot(toTarget) >= cos(fov/2.0);
}





//-----------------------------------------------------------------------------
//  printing
//-----------------------------------------------------------------------------

template <class T>
std::ostream& operator<<(std::ostream& os, sf::Vector2<T>& rhs) 
{
    os << rhs.x << ',' << rhs.y;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const vec& rhs)
{
    os << " " << rhs.x << " " << rhs.y;
    return os;
}

inline std::ifstream& operator>>(std::ifstream& is, vec& lhs)
{
    is >> lhs.x >> lhs.y;
    return is;
}



inline Mates::xy PosToTile(vec pos)
{
	return
	{
		int((pos.x) / 16),
		int((pos.y) / 16)
	};
}


