#pragma once

#include <math.h>
#include <iosfwd>
#include <limits>
#include <iostream>

#include <SFML/Graphics.hpp>

#include "vector.h"

struct Bounds : public sf::Rect<float>
{
    explicit Bounds() : sf::Rect<float>(0,0,0,0) { }
    explicit Bounds(sf::Rect<float> r) : sf::Rect<float>(r.left, r.top, r.width, r.height) { }
    explicit Bounds(const vec& v) : sf::Rect<float>(0,0,v.x,v.y) { }
    explicit Bounds(const vec& pos, vec size, bool centered = false) : sf::Rect<float>(pos.x,pos.y,size.x,size.y) {
        if (centered) { left -= size.x/2; top -= size.y/2; }
    }
    explicit Bounds(const vec& pos, vec size, vec origin) : sf::Rect<float>(pos.x, pos.y, size.x, size.y) {
        left -= origin.x;
        top -= origin.y;
    }
    explicit Bounds(float x, float y, float w, float h) : sf::Rect<float>(x,y,w,h) { }

    //Expands arround the center by an amount
    void Expand(float x, float y)
	{
        width += x;
        left -= x/2;
        height += y;
        top -= y/2;
    }

    void Expand(const vec& v) { Expand(v.x, v.y); }

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

	void Draw(sf::RenderTarget &rt, sf::Color color = sf::Color::Red, sf::Color fillColor = sf::Color::Transparent)
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

	bool IsInside(vec point) const
	{
        if (point.x < left) return false;
        if (point.x >= left+width) return false;
        if (point.y < top) return false;
        if (point.y >= top+height) return false;
		return true;
    }

    //TODO
    //void ExpandToInclude(vec point);

};

struct CircleBounds
{
    CircleBounds(vec pos, float radius) : pos(pos), radius(radius) {}
    vec pos;
    float radius;

    void Draw(sf::RenderTarget& window, sf::Color color = sf::Color::Red, sf::Color fillColor = sf::Color::Transparent)
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
};
inline bool Collide(Bounds a, Bounds b) {

     return
            (a.left < b.left + b.width && a.left + a.width > b.left &&
                a.top < b.top + b.height && a.top + a.height > b.top);
}

inline std::ostream& operator<<(std::ostream& os, const Bounds& rhs)
{
  os << " " << rhs.left << " " << rhs.top << " " << rhs.width << " " << rhs.height;

  return os;
}
