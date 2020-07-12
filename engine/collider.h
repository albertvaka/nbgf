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
    return a.DistanceSq(b) < 0;
}

// Circle with box
inline bool Collide(const CircleBounds& a, const Bounds& b)
{
    return b.DistanceSq(a) < 0;
}
inline bool Collide(const Bounds& a, const CircleBounds& b)
{
    return b.DistanceSq(a) < 0;
}


// Entities
inline bool Collide(const BoxEntity* a, const BoxEntity* b) {
    return Collide(a->bounds(), b->bounds());
}
inline bool Collide(const CircleEntity* a, const CircleEntity* b) {
    return Collide(a->bounds(), b->bounds());
}
inline bool Collide(const CircleEntity* a, const BoxEntity* b) {
    return Collide(a->bounds(), b->bounds());
}

template <typename S, typename E, typename X, typename Y>
void CollideAll(const std::vector<S*>& setA, const std::vector<E*>& setB, void (*callback)(X*, Y*))
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

template <typename S, typename X, typename Y>
void CollideSelf(const std::vector<S*>& setA, void (*callback)(X*, Y*))
{
    size_t sa = setA.size();
    for (size_t i = 0; i < sa; ++i)
    {
        S* a = setA[i];
        for (size_t j = i+1; j < sa; ++j)
        {
            S* b = setA[j];
            if (Collide(a, b))
            {
                callback(a, b);
            }
        }
    }
}
