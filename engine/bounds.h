#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include "vec.h"
#include "SDL_gpu.h"

struct CircleBounds;

struct Bounds
{
    float left, top;
    float width, height;

    constexpr Bounds(float x, float y, float w, float h) : left(x), top(y), width(w), height(h) { }
    constexpr Bounds() : Bounds(-1,-1,-1,-1) { }
    constexpr Bounds(const vec& topleft, const vec& size) : Bounds(topleft.x, topleft.y, size.x, size.y) {}
    constexpr explicit Bounds(const vec& size) : Bounds(0,0,size.x,size.y) { }
    constexpr explicit Bounds(const vec& pos, const vec& size, const vec& origin) : Bounds(pos.x, pos.y, size.x, size.y) {
        left -= origin.x;
        top -= origin.y;
    }

    static constexpr Bounds fromCenter(const vec& center, const vec& size) { return Bounds(center - size/2, size); }

    GPU_Rect AsRect() {
        return GPU_Rect{ left, top, width, height };
    }

    //Expands arround the center by a factor
    Bounds operator*(float f)
	{
        vec center = Center();
        Bounds ret = *this;
        ret.width *= f;
        ret.height *= f;
        ret.SetCenter(center);
        return ret;
    }

    [[nodiscard]] constexpr vec Center() const
	{
		return vec(left + width/2, top + height/2);
	}

    [[nodiscard]] constexpr float Area() const
    {
        return width * height;
    }

    void SetCenter(float x, float y)
	{
        left = x - width/2;
        top = y - height/2;
    }

    void SetCenter(const vec& center)
	{
        SetCenter(center.x, center.y);
    }

    void SetTopLeft(float x, float y)
    {
        left = x;
        top = y;
    }

    void SetTopLeft(const vec& center)
    {
        SetTopLeft(center.x, center.y);
    }

    void Draw(uint8_t r = 255, uint8_t g = 0, uint8_t b = 0) const;

    [[nodiscard]] constexpr float Top() const
	{
        return top;
    }

    [[nodiscard]] constexpr float Bottom() const
	{
        return top + height;
    }

    [[nodiscard]] constexpr float Left() const
	{
        return left;
    }

    [[nodiscard]] constexpr float Right() const
	{
        return left + width;
    }

    [[nodiscard]] constexpr vec TopLeft() const {
        return vec(Left(), Top());
    }

    [[nodiscard]] constexpr vec TopRight() const {
        return vec(Right(), Top());
    }

    [[nodiscard]] constexpr vec BottomRight() const {
        return vec(Right(), Bottom());
    }

    [[nodiscard]] bool Contains(const vec& point) const
	{
        if (point.x < left) return false;
        if (point.x >= left+width) return false;
        if (point.y < top) return false;
        if (point.y >= top+height) return false;
		return true;
    }

    [[nodiscard]] bool Contains(const Bounds& b) const
    {
        if (b.left < left) return false;
        if (b.left+b.width >= left + width) return false;
        if (b.top < top) return false;
        if (b.top+ b.height >= top + height) return false;
        return true;
    }

    [[nodiscard]] constexpr vec Size() const {
        return vec(width, height);
    }

    [[nodiscard]] vec ClosesPointInBounds(const vec& target) const;
    [[nodiscard]] float DistanceSq(const Bounds& a) const;
    [[nodiscard]] float DistanceSq(const CircleBounds& a) const;
    [[nodiscard]] float Distance(const Bounds& a) const;
    [[nodiscard]] float Distance(const CircleBounds& a) const;

    //TODO
    //void ExpandToInclude(vec point);

};

struct CircleBounds
{
    constexpr CircleBounds(const vec& pos, float radius) : pos(pos), radius(radius) {}
    vec pos;
    float radius;

    void Draw(uint8_t r = 255, uint8_t g = 0, uint8_t b = 0) const;

    float DistanceSq(const Bounds& a) const { return a.DistanceSq(*this); };
    float Distance(const Bounds& a) const { return a.Distance(*this); }
    
    float DistanceSq(const CircleBounds& a) const {
        return a.pos.DistanceSq(this->pos) - (a.radius + this->radius) * (a.radius + this->radius);
    }
    float Distance(const CircleBounds& a) const { 
        return a.pos.Distance(this->pos) - (a.radius + this->radius);
    }
};

inline float Bounds::DistanceSq(const Bounds& a) const {
    float sqrDist = 0;
    if (a.Right() < this->left) {
        float d = a.Right() - this->left;
        sqrDist += d * d;
    } else if (a.left > this->Right()) {
        float d = a.left - this->Right();
        sqrDist += d * d;
    }
    if (a.Bottom() < this->top) {
        float d = a.Bottom() - this->top;
        sqrDist += d * d;
    }
    else if (a.top > this->Bottom()) {
        float d = a.top - this->Bottom();
        sqrDist += d * d;
    }
    return sqrDist;
}

inline vec Bounds::ClosesPointInBounds(const vec& target) const {
    vec distance = this->Center() - target;
    distance.Clamp(-this->Size() / 2, this->Size() / 2);
    vec closestPoint = this->Center() - distance;
    return closestPoint;
}

inline float Bounds::DistanceSq(const CircleBounds& a) const {
    vec closestPoint = ClosesPointInBounds(a.pos);
    //closestPoint.Debuggerino();
    return closestPoint.DistanceSq(a.pos) - (a.radius * a.radius);
}

inline float Bounds::Distance(const Bounds& a) const
{
    return sqrt(DistanceSq(a));
}

inline float Bounds::Distance(const CircleBounds& a) const
{
    return sqrt(DistanceSq(a));
}

inline std::ostream& operator<<(std::ostream& os, const Bounds& rhs)
{
    os << " " << rhs.left << " " << rhs.top << " " << rhs.width << " " << rhs.height;
    return os;
}
