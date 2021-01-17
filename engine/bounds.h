#pragma once

#ifndef _USE_MATH_DEFINES 
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "vec.h"
#include "SDL_gpu.h"

struct CircleBounds;

struct BoxBounds
{
    float left, top;
    float width, height;

    constexpr BoxBounds(float x, float y, float w, float h) : left(x), top(y), width(w), height(h) { }
    constexpr BoxBounds() : BoxBounds(-1,-1,0,0) { }
    constexpr BoxBounds(vec topleft, vec size) : BoxBounds(topleft.x, topleft.y, size.x, size.y) {}
    constexpr explicit BoxBounds(vec size) : BoxBounds(0,0,size.x,size.y) { }
    constexpr explicit BoxBounds(vec pos, vec size, vec origin) : BoxBounds(pos.x, pos.y, size.x, size.y) {
        left -= origin.x;
        top -= origin.y;
    }

    [[nodiscard]] static constexpr BoxBounds FromCenter(vec center, vec size) { return BoxBounds(center - size/2, size); }

    [[nodiscard]] GPU_Rect AsRect() {
        return GPU_Rect{ left, top, width, height };
    }

    //Expands arround the center by a factor
    [[nodiscard]] BoxBounds operator*(float f) const
	{
        vec center = Center();
        BoxBounds ret = *this;
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

    void SetCenter(vec center)
	{
        SetCenter(center.x, center.y);
    }

    void SetTopLeft(float x, float y)
    {
        left = x;
        top = y;
    }

    void SetTopLeft(vec center)
    {
        SetTopLeft(center.x, center.y);
    }

    void DebugDraw(uint8_t r = 255, uint8_t g = 0, uint8_t b = 0) const
#ifdef _DEBUG
    ;
#else
    {}
#endif

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

    [[nodiscard]] constexpr vec BottomLeft() const {
        return vec(Left(), Bottom());
    }

    [[nodiscard]] constexpr vec BottomRight() const {
        return vec(Right(), Bottom());
    }

    [[nodiscard]] bool Contains(float x, float y) const
    {
        if (x < left) return false;
        if (x >= left + width) return false;
        if (y < top) return false;
        if (y >= top + height) return false;
        return true;
    }

    [[nodiscard]] bool Contains(vec point) const
	{
        return Contains(point.x, point.y);
    }

    [[nodiscard]] bool Contains(const BoxBounds& b) const
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

    [[nodiscard]] vec ClosestPointInBounds(vec target) const;
    [[nodiscard]] float DistanceSq(const BoxBounds& a) const;
    [[nodiscard]] float DistanceSq(const CircleBounds& a) const;
    [[nodiscard]] float Distance(const BoxBounds& a) const;
    [[nodiscard]] float Distance(const CircleBounds& a) const;

    //TODO
    //void ExpandToInclude(vec point);

};

struct CircleBounds
{
    constexpr CircleBounds(vec pos, float radius) : pos(pos), radius(radius) {}
    vec pos;
    float radius;


    void DebugDraw(uint8_t r = 255, uint8_t g = 0, uint8_t b = 0) const
#ifdef _DEBUG
    ;
#else
    {}
#endif

    [[nodiscard]] vec Center() const { return pos; }

    [[nodiscard]] float DistanceSq(const BoxBounds& a) const { return a.DistanceSq(*this); }
    [[nodiscard]] float Distance(const BoxBounds& a) const { return a.Distance(*this); }
    
    [[nodiscard]] float DistanceSq(const CircleBounds& a) const {
        return a.pos.DistanceSq(this->pos) - (a.radius + this->radius) * (a.radius + this->radius);
    }
    [[nodiscard]] float Distance(const CircleBounds& a) const { 
        return a.pos.Distance(this->pos) - (a.radius + this->radius);
    }
};

inline float BoxBounds::DistanceSq(const BoxBounds& a) const {
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

inline vec BoxBounds::ClosestPointInBounds(vec target) const {
    vec distance = this->Center() - target;
    distance.Clamp(-this->Size() / 2, this->Size() / 2);
    vec closestPoint = this->Center() - distance;
    return closestPoint;
}

inline float BoxBounds::DistanceSq(const CircleBounds& a) const {
    vec closestPoint = ClosestPointInBounds(a.pos);
    //closestPoint.DebugDraw();
    return closestPoint.DistanceSq(a.pos) - (a.radius * a.radius);
}

inline float BoxBounds::Distance(const BoxBounds& a) const
{
    return sqrt(DistanceSq(a));
}

inline float BoxBounds::Distance(const CircleBounds& a) const
{
    return sqrt(DistanceSq(a));
}

inline std::ostream& operator<<(std::ostream& os, const BoxBounds& rhs)
{
    os << " " << rhs.left << " " << rhs.top << " " << rhs.width << " " << rhs.height;
    return os;
}
