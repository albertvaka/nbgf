#pragma once

#include <functional>

#include "bounds.h"
#include "entity.h"
#include "selfregister.h"

// Boxes with box
inline bool Collide(const Bounds& a, const Bounds& b) {
    return (a.left < b.left + b.width &&
        a.left + a.width > b.left &&
        a.top < b.top + b.height &&
        a.top + a.height > b.top);
}
// Circle with circle
inline bool Collide(const CircleBounds& a, const CircleBounds& b)
{
    float radiuses = a.radius + b.radius;
    return a.pos.DistanceSq(b.pos) < (radiuses * radiuses);
}

// Circle with box
inline bool Collide(const CircleBounds& a, const Bounds& b)
{
    return Collide(Bounds(a.pos, vec(a.radius, a.radius), true), b);
    //TODO
    //vec distance = b.Center() - a.pos;
    //distance.Clamp(-b.Size(), b.Size());
    //vec closestPoint = b.Center() + distance;
    //return (closestPoint - a.pos).LengthSq() > (a.radius * a.radius);
}
inline bool Collide(const Bounds& a, const CircleBounds& b)
{
    return Collide(b, a);
}

// Entities
inline bool Collide(BoxEntity* a, BoxEntity* b) {
    return Collide(a->bounds(), b->bounds());
}
inline bool Collide(CircleEntity* a, CircleEntity* b) {
    return Collide(a->bounds(), b->bounds());
}
inline bool Collide(CircleEntity* a, BoxEntity* b) {
    return Collide(a->bounds(), b->bounds());
}

template <typename S, typename E, typename X, typename Y>
void collide(const std::vector<S*>& setA, const std::vector<E*>& setB, void (*callback)(X*, Y*))
{
    size_t sa = setA.size();
    for (size_t i = 0; i < sa; ++i)
    {
        S* a = setA[i];
        size_t sb = setB.size();
        for (size_t j = 0; j < sb; ++j)
        {
            E* b = setB[j];
            if ((void*)a == (void*)b) continue;
            if (Collide(a, b))
            {
                callback(a, b);
            }
        }
    }
}
