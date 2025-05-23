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
    constexpr BoxBounds(vec topleft, vec size) : BoxBounds(topleft.x, topleft.y, size.x, size.y) {}
    constexpr explicit BoxBounds(vec size) : BoxBounds(0, 0, size.x, size.y) { }
    constexpr explicit BoxBounds(vec pos, vec size, vec origin) : BoxBounds(pos.x, pos.y, size.x, size.y) {
        left -= origin.x;
        top -= origin.y;
    }
    template<typename T> //Works with GPU_Rect, SDL_Rect and SDL_FRect
    constexpr explicit BoxBounds(T rect) : BoxBounds(rect.x, rect.y, rect.w, rect.h) { }


    [[nodiscard]] static constexpr BoxBounds FromCenter(vec center, vec size) { return BoxBounds(center - size / 2, size); }

    [[nodiscard]] GPU_Rect AsRect() {
        return GPU_Rect{ left, top, width, height };
    }

    //Expands arround the center by a factor
    [[nodiscard]] BoxBounds operator*(float f) const
    {
        BoxBounds ret = *this;
        ret *= f;
        return ret;
    }
    BoxBounds& operator*=(float f)
    {
        vec center = Center();
        width *= f;
        height *= f;
        SetCenter(center);
        return *this;
    }

    //Expands arround the center by a value
    void Grow(float x, float y)
    {
        left -= x / 2;
        top -= y / 2;
        width += x;
        height += y;
    }

    [[nodiscard]] BoxBounds Grown(float x, float y) const
    {
        BoxBounds ret = *this;
        ret.Grow(x, y);
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

    void SetTopAndBottom(float newTop, float newBottom)
    {
        top = newTop;
        height = newBottom - newTop;
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

    void SetTopLeft(vec pos)
    {
        SetTopLeft(pos.x, pos.y);
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

    [[nodiscard]] constexpr vec Size() const {
        return vec(width, height);
    }

    [[nodiscard]] bool Contains(float x, float y) const {
        if (x < left) return false;
        if (x >= left + width) return false;
        if (y < top) return false;
        if (y >= top + height) return false;
        return true;
    }
    [[nodiscard]] bool Contains(vec point) const {
        return Contains(point.x, point.y);
    }

    // When they overlap, returns 0
    [[nodiscard]] float DistanceSq(const BoxBounds& a) const;
    // When they overlap, returns 0
    [[nodiscard]] float Distance(const BoxBounds& a) const;

    // When they overlap, returns a negative distance
    [[nodiscard]] float DistanceSq(const CircleBounds& a) const;
    // When they overlap, returns 0
    [[nodiscard]] float Distance(const CircleBounds& a) const;

    // When they overlap, returns 0
    [[nodiscard]] float DistanceSq(vec point) const {
		vec clamped = ClosestPointInBounds(point);
        vec d = point - clamped;
        return d.x * d.x + d.y * d.y;
    }
    // When they overlap, returns 0
    [[nodiscard]] float Distance(vec point) const {
		return sqrt(DistanceSq(point));
    }

    [[nodiscard]] bool Contains(const BoxBounds& b) const
    {
        if (b.left < left) return false;
        if (b.left + b.width >= left + width) return false;
        if (b.top < top) return false;
        if (b.top + b.height >= top + height) return false;
        return true;
    }

    // When they overlap, returns the target point unmodified. See also ClosestPointInBoundsEdge.
    [[nodiscard]] vec ClosestPointInBounds(vec target) const {
        return target.Clamped(TopLeft(), BottomRight());
    }

    [[nodiscard]] vec ClosestPointInBoundsEdge(vec target) const {
        vec v = target.Clamped(TopLeft(), BottomRight());
        float distLeft = fabs(v.x - Left());
        float distRight = fabs(v.x - Right());
        float distTop = fabs(v.y - Top());
        float distBottom = fabs(v.y - Bottom());
        if (distLeft < distRight && distLeft < distTop && distLeft < distBottom) {
            v.x = Left();
        }
        else if (distRight < distTop && distRight < distBottom) {
            v.x = Right();
        }
        else if (distTop < distBottom) {
            v.y = Top();
        }
        else {
            v.y = Bottom();
        }
        return v;
    }

};

struct CircleBounds
{
    constexpr CircleBounds(vec pos, float radius) : pos(pos), radius(radius) {}
    constexpr CircleBounds(float x, float y, float radius) : pos(x, y), radius(radius) {}
    vec pos;
    float radius;


    void DebugDraw(uint8_t r = 255, uint8_t g = 0, uint8_t b = 0) const
#ifdef _DEBUG
    ;
#else
    {}
#endif

    [[nodiscard]] vec Center() const { return pos; }
    [[nodiscard]] float Top() const { return pos.y - radius; }
    [[nodiscard]] float Bottom() const { return pos.y + radius; }
    [[nodiscard]] float Left() const { return pos.x - radius; }
    [[nodiscard]] float Right() const { return pos.x + radius; }

    //Expands arround the center by a factor
    [[nodiscard]] CircleBounds operator*(float f) const
    {
        CircleBounds ret = *this;
        ret *= f;
        return ret;
    }
    CircleBounds& operator*=(float f)
    {
        radius *= f;
        return *this;
    }

    // When they overlap, returns a negative distance
    [[nodiscard]] float DistanceSq(const BoxBounds& a) const { return a.DistanceSq(*this); }
    // When they overlap, returns 0
    [[nodiscard]] float Distance(const BoxBounds& a) const { return a.Distance(*this); }

    // When they overlap, returns a negative distance
    [[nodiscard]] float DistanceSq(const CircleBounds& a) const {
        return a.pos.DistanceSq(this->pos) - (a.radius + this->radius) * (a.radius + this->radius);
    }
    // When they overlap, returns a negative distance
    [[nodiscard]] float Distance(const CircleBounds& a) const { 
        return a.pos.Distance(this->pos) - (a.radius + this->radius);
    }
    // When they overlap, returns a negative distance
    [[nodiscard]] float DistanceSq(vec a) const {
        return a.DistanceSq(this->pos) - (this->radius * this->radius);
    }
    // When they overlap, returns a negative distance
    [[nodiscard]] float Distance(vec a) const {
        return a.Distance(this->pos) - (this->radius);
    }

    [[nodiscard]] bool Contains(float x, float y) const { return Contains(vec(x, y)); }
    [[nodiscard]] bool Contains(vec v) const { return (pos.DistanceSq(v) <= radius*radius); }

    // When they overlap, returns the target point unmodified. See also ClosestPointInBoundsEdge.
    [[nodiscard]] vec ClosestPointInBounds(vec target) const {
        vec direction = target - pos;
        if (direction.LengthSq() <= radius * radius) {
            return target;
        }
        return pos + direction.Normalized() * radius;
    }

    [[nodiscard]] vec ClosestPointInBoundsEdge(vec target) const {
        vec direction = target - pos;
        return pos + direction.Normalized() * radius;
    }

    [[nodiscard]] BoxBounds EnclosingBoxBounds() const { return BoxBounds::FromCenter(Center(), vec(radius*2)); }
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

inline float BoxBounds::DistanceSq(const CircleBounds& a) const {
    vec closestPoint = ClosestPointInBounds(a.pos);
    return closestPoint.DistanceSq(a.pos) - (a.radius * a.radius);
}

inline float BoxBounds::Distance(const BoxBounds& a) const
{
    return sqrt(DistanceSq(a));
}

inline float BoxBounds::Distance(const CircleBounds& a) const
{
    float distSq = DistanceSq(a);
	if (distSq <= 0) {
		return 0;
	}
    return sqrt(distSq);
}

inline std::ostream& operator<<(std::ostream& os, const BoxBounds& rhs)
{
    os << rhs.left << " " << rhs.top << " " << rhs.width << " " << rhs.height;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const CircleBounds& rhs)
{
    os << rhs.pos << " r=" << rhs.radius;
    return os;
}
