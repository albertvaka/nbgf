#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include <cassert>
#include <vector>
#include <string>

#include "entity.h"
#include "matrix2d.h"
#include "vec.h"

struct SteeringBehavior;

struct SteeringEntity : public CircleEntity
{
    float max_speed;

    // NOTE: velocity is used by steering behaviours to know the direction of this entity, make sure it's initialized and not 0!
    SteeringEntity(const vec& position, float radius, float max_speed, const vec& velocity)
        : CircleEntity(position, radius)
        , max_speed(max_speed)
    {
        vel = velocity;
    }

    vec Heading() const { return vel.Normalized(); } // Optimization: compute once per frame and store

};
