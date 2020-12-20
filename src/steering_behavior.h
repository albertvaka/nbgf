#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include <vector>
#include <string>
#include <list>

#include "entity.h"
#include "vec.h"
#include "mates.h"
#include "bounds.h"
#include "steering_entity.h"

struct TileMap;

struct SteeringBehavior
{
    //Arrive makes use of these to determine how quickly a SteeringEntity should decelerate to its target
    enum Deceleration { slow = 3, normal = 2, fast = 1 };

    SteeringBehavior(SteeringEntity* agent);

    vec Forward();

    //this behavior moves the agent towards a target position
    vec Seek(vec TargetPos);

    //this behavior returns a vector that moves the agent away
    //from a target position
    vec Flee(vec TargetPos);

    //this behavior is similar to seek but it attempts to arrive 
    //at the target position with a zero velocity
    vec Arrive(vec TargetPos, Deceleration deceleration);

    //this behavior predicts where an agent will be in time T and seeks
    //towards that point to intercept it.
    vec Pursuit(const Entity* evader);

    //this behavior maintains a position, in the direction of offset
    //from the target SteeringEntity
    vec OffsetPursuit(const Entity* leader, const float offset);

    //this behavior makes the agent wander about randomly
    vec Wander(float dt);

    //this returns a steering force which will attempt to keep the agent 
    //away from any obstacles it may encounter
    template<typename T>
    vec ObstacleAvoidance(const std::vector<T*>& obstacles, float margin = 0.f);

	vec TileMapAvoidance(TileMap* map);

    //this returns a steering force which will keep the agent in its bounds
    vec BoundsAvoidance(const Bounds& m_bounds);

    //given another agent position to hide from and a list of Entitys this
    //method attempts to put an obstacle between itself and its opponent
	template<typename T>
	vec Hide(Entity* hideTarget, const std::vector<T*>& obstacles);

    //helper method for Hide. Returns a position located on the other
    //side of an obstacle to the pursuer
    vec GetHidingPosition(vec posOb, const float radiusOb, vec posHunter);

	bool avoidingTileMap = false;
	bool avoidingBounds = false;

//protected:

  //a pointer to the owner of this instance
  SteeringEntity* steeringEntity;   
      
  //the current position on the wander circle the agent is
  //attempting to steer towards
  vec m_vWanderTarget; 

};


//--------------------------- Hide ---------------------------------------
//
//------------------------------------------------------------------------
template<typename T>
vec SteeringBehavior::Hide(Entity* hideTarget, const std::vector<T*>& obstacles)
{
	float DistToClosest = Mates::MaxFloat;
	vec   BestHidingSpot;

	for (const CircleEntity* ob : obstacles)
	{
		//calculate the position of the hiding spot for this obstacle
		vec HidingSpot = GetHidingPosition(ob->pos, ob->radius, hideTarget->pos);

		//work in distance-squared space to find the closest hiding
		//spot to the agent
		float dist = HidingSpot.DistanceSq(steeringEntity->pos);

		if (dist < DistToClosest)
		{
			DistToClosest = dist;
			BestHidingSpot = HidingSpot;
		}
	}

	//if no suitable obstacles found then Evade the hunter
	if (DistToClosest == Mates::MaxFloat)
	{
		return Flee(hideTarget->pos);
	}

	//else use Arrive on the hiding spot
	return Arrive(BestHidingSpot, fast);
}


//---------------------- ObstacleAvoidance -------------------------------
//
//  Given a vector of CObstacles, this method returns a steering force
//  that will prevent the agent colliding with the closest obstacle
//------------------------------------------------------------------------
template<typename T>
vec SteeringBehavior::ObstacleAvoidance(const std::vector<T*>& obstacles, float margin)
{
	//the detection box length is proportional to the agent's velocity
	float realBoxLength = steeringEntity->radius + margin; //FIXME: Take into account current speed

	//this will keep track of the closest intersecting obstacle (CIB)
	const CircleEntity* ClosestIntersectingObstacle = nullptr;

	//this will be used to track the distance to the CIB
	float DistToClosestIP = Mates::MaxFloat;

	//this will record the transformed local coordinates of the CIB
	vec LocalPosOfClosestObstacle;

	for (const CircleEntity* obst : obstacles)
	{
		vec to = obst->pos - steeringEntity->pos;

		//the bounding radius of the other is taken into account by adding it 
		//to the range
		float range = realBoxLength + obst->radius;

		//if entity within range, tag for further consideration. (working in
		//distance-squared space to avoid sqrts)
		if ((to.LengthSq() < range * range))
		{

			//calculate this obstacle's position in local space
			vec LocalPos = PointToLocalSpace(obst->pos, steeringEntity->Heading(), steeringEntity->pos);

			//if the local position has a negative x value then it must lay
			//behind the agent. (in which case it can be ignored)
			if (LocalPos.x >= 0)
			{
				//if the distance from the x axis to the object's position is less
				//than its radius + half the width of the detection box then there
				//is a potential intersection.
				float ExpandedRadius = obst->radius + steeringEntity->radius;

				/*if (fabs(LocalPos.y) < ExpandedRadius)
				{*/
				//now to do a line/circle intersection test. The center of the 
				//circle is represented by (cX, cY). The intersection points are 
				//given by the formula x = cX +/-sqrt(r^2-cY^2) for y=0. 
				//We only need to look at the smallest positive value of x because
				//that will be the closest point of intersection.
				float cX = LocalPos.x;
				float cY = LocalPos.y;

				//we only need to calculate the sqrt part of the above equation once
				float SqrtPart = sqrt(ExpandedRadius * ExpandedRadius - cY * cY);

				float ip = cX - SqrtPart;

				if (ip <= 0.0)
				{
					ip = cX + SqrtPart;
				}

				//test to see if this is the closest so far. If it is keep a
				//record of the obstacle and its local coordinates
				if (ip < DistToClosestIP)
				{
					DistToClosestIP = ip;
					ClosestIntersectingObstacle = obst;
					LocalPosOfClosestObstacle = LocalPos;
				}
			}
			//}
		}
	}

	//if we have found an intersecting obstacle, calculate a steering 
	//force away from it
	vec SteeringForce;

	if (ClosestIntersectingObstacle)
	{
		//the closer the agent is to an object, the stronger the 
		//steering force should be
		float multiplier = 1.0f + (realBoxLength - LocalPosOfClosestObstacle.x) / realBoxLength;

		//calculate the lateral force
		SteeringForce.y = (ClosestIntersectingObstacle->radius - LocalPosOfClosestObstacle.y) * multiplier;

		//apply a braking force proportional to the obstacles distance from
		//the SteeringEntity. 
		const float BrakingWeight = 0.2f;

		SteeringForce.x = (ClosestIntersectingObstacle->radius - LocalPosOfClosestObstacle.x) * BrakingWeight;
	}

	//finally, convert the steering vector from local to world space
	return VectorToWorldSpace(SteeringForce, steeringEntity->Heading());
}

