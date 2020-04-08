#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <SFML/Graphics.hpp>

#include "vector.h"

struct CircleBounds;

struct Bounds : public sf::Rect<float>
{
    explicit Bounds() : sf::Rect<float>(0,0,0,0) { }
    explicit Bounds(const sf::Rect<float>& r) : sf::Rect<float>(r) { }
    explicit Bounds(const vec& size) : sf::Rect<float>(0,0,size.x,size.y) { }
    explicit Bounds(const vec& topleft, const vec& size) : sf::Rect<float>(topleft.x, topleft.y, size.x, size.y) {}
    explicit Bounds(const vec& pos, const vec& size, const vec& origin) : sf::Rect<float>(pos.x, pos.y, size.x, size.y) {
        left -= origin.x;
        top -= origin.y;
    }
    explicit Bounds(float left, float top, float width, float height) : sf::Rect<float>(left , top, width, height) { }
    static Bounds fromCenter(const vec& center, vec size) { return Bounds(center - size/2, size); }

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

    vec Center() const
	{
		return vec(left + width/2, top + height/2);
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

	void Draw(sf::RenderTarget &rt, sf::Color color = sf::Color::Red, sf::Color fillColor = sf::Color::Transparent) const
	{
		sf::RectangleShape rs;

		rs.setSize(vec(width,height));
		rs.setPosition(vec(left,top));

		rs.setFillColor(fillColor);
		rs.setOutlineColor(color);
		rs.setOutlineThickness(1);

		rt.draw(rs);
	}

    float Top() const
	{
        return top;
    }

    float Bottom() const
	{
        return top + height;
    }

    float Left() const
	{
        return left;
    }

    float Right() const
	{
        return left + width;
    }

    vec TopLeft() const {
        return vec(Left(), Top());
    }

    vec BottomRight() const {
        return vec(Right(), Bottom());
    }

	bool IsInside(vec point) const
	{
        if (point.x < left) return false;
        if (point.x >= left+width) return false;
        if (point.y < top) return false;
        if (point.y >= top+height) return false;
		return true;
    }

    vec Size() const {
        return vec(width, height);
    }

    vec ClosesPointInBounds(const vec& target) const;
    float DistanceSq(const Bounds& a) const;
    float DistanceSq(const CircleBounds& a) const;
    float Distance(const Bounds& a) const;
    float Distance(const CircleBounds& a) const;

    //TODO
    //void ExpandToInclude(vec point);

};

struct CircleBounds
{
    CircleBounds(vec pos, float radius) : pos(pos), radius(radius) {}
    vec pos;
    float radius;

    void Draw(sf::RenderTarget& window, sf::Color color = sf::Color::Red, sf::Color fillColor = sf::Color::Transparent) const
    {
        sf::CircleShape cs(radius);
        cs.setRadius(radius);
        cs.setOrigin(vec(radius, radius));
        cs.setPosition(pos);

        cs.setFillColor(fillColor);
        cs.setOutlineColor(color);
        cs.setOutlineThickness(1);

        window.draw(cs);
    }

    float DistanceSq(const Bounds& a) const { return a.DistanceSq(*this); };
    float Distance(const Bounds& a) const { return a.Distance(*this); }
    
    float DistanceSq(const CircleBounds& a) const {
        return a.pos.DistanceSq(this->pos) - (a.radius + this->radius) * (a.radius + this->radius);
    }
    float Distance(const CircleBounds& a) const { 
        return a.pos.Distance(this->pos) - (a.radius+ this->radius);
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
    //closestPoint.Debuggerino(sf::Color::Blue);
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
