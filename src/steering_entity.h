#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include "entity.h"
#include "vec.h"

struct SteeringBehavior;

struct SteeringEntity : public CircleEntity
{
    float max_speed;

    // NOTE: velocity is used by steering behaviours to know the direction of this entity, make sure it's initialized and not 0!
    SteeringEntity(vec position, float radius, float max_speed, vec velocity)
        : CircleEntity(position, radius)
        , max_speed(max_speed)
    {
        vel = velocity;
    }

    // Potential optimization: compute those once per frame and store
    vec Heading() const { return vel.Normalized(); }
    float Speed() const { return vel.Length(); }

};
