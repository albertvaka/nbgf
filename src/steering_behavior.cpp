//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include "steering_behavior.h"

#include "angles.h"
#include "mates.h"
#include "rand.h"
#include "steering_entity.h"
//#include "gaemtilemap.h"

SteeringBehavior::SteeringBehavior(SteeringEntity* agent) : steeringEntity(agent)
{
	//create a vector to a target position on the wander circle
	float theta = Rand::rollf() * Angles::Tau;
	m_vWanderTarget = vec(0.37f * cos(theta), 0.37f * sin(theta));
}


//------------------------------- Forward -----------------------------------
vec SteeringBehavior::Forward()
{
	vec force = steeringEntity->Heading() * steeringEntity->max_speed;
	return force;
}

//------------------------------- Seek -----------------------------------
//
//  Given a target, this behavior returns a steering force which will
//  direct the agent towards the target
//------------------------------------------------------------------------
vec SteeringBehavior::Seek(vec TargetPos)
{
	vec DesiredVelocity = (TargetPos - steeringEntity->pos).Normalized() * steeringEntity->max_speed;

	return DesiredVelocity;
}

//----------------------------- Flee -------------------------------------
//
//  Does the opposite of Seek
//------------------------------------------------------------------------
vec SteeringBehavior::Flee(vec TargetPos)
{
	vec ToTarget = steeringEntity->pos - TargetPos;
	float distanceSq = ToTarget.LengthSq();

	// Avoid division by zero
	if (distanceSq == 0.0f)
	{
		return vec::Zero;
	}

	// Scale the fleeing force based on max_speed and inversely proportional to distance
	float scale = std::clamp((steeringEntity->max_speed * steeringEntity->max_speed) / distanceSq, 0.01f, steeringEntity->max_speed);

	vec DesiredVelocity = ToTarget.Normalized() * scale;

	return DesiredVelocity;
}

//--------------------------- Arrive -------------------------------------
//
//  This behavior is similar to seek but it attempts to arrive at the
//  target with a zero velocity
//------------------------------------------------------------------------
vec SteeringBehavior::Arrive(vec TargetPos, Deceleration deceleration)
{
	vec ToTarget = TargetPos - steeringEntity->pos;

	//calculate the distance to the target
	float dist = ToTarget.Length();

	if (dist > 5.f) // FIXME: Hardcoded
	{
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration.
		const float DecelerationTweaker = 0.3f;

		//calculate the vel required to reach the target given the desired
		//deceleration
		float vel = dist / ((float)deceleration * DecelerationTweaker);

		//make sure the velocity does not exceed the max
		vel = std::min(vel, steeringEntity->max_speed);

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		vec DesiredVelocity = ToTarget * vel / dist;

		return (DesiredVelocity - steeringEntity->vel);
	}

	return vec(0, 0);
}

//------------------------------ Pursuit ---------------------------------
//
//  this behavior creates a force that steers the agent towards the 
//  evader
//------------------------------------------------------------------------
vec SteeringBehavior::Pursuit(const Entity* evader)
{
	//if the evader is ahead and facing the agent then we can just seek
	//for the evader's current position.
	vec ToEvader = evader->pos - steeringEntity->pos;

	vec heading = steeringEntity->Heading();

	float RelativeHeading = heading.Dot(evader->vel.Normalized());

	if ((ToEvader.Dot(heading) > 0) && (RelativeHeading < -0.95f))  //acos(0.95)=18 degs
	{
		return Seek(evader->pos);
	}

	//Not considered ahead so we predict where the evader will be.

	//the lookahead time is propotional to the distance between the evader
	//and the pursuer; and is inversely proportional to the sum of the
	//agent's velocities
	float LookAheadTime = ToEvader.Length() /
		(steeringEntity->max_speed + evader->vel.Length());

	//now seek to the predicted future position of the evader
	return Seek(evader->pos + evader->vel * LookAheadTime);
}

//----------------------------- Evade ------------------------------------
//
//  similar to pursuit except the agent Flees from the estimated future
//  position of the pursuer
//------------------------------------------------------------------------
vec SteeringBehavior::Evade(const SteeringEntity* pursuer)
{
	/* Not necessary to include the check for facing direction this time */

	vec ToPursuer = pursuer->pos - steeringEntity->pos;

	//uncomment the following two lines to have Evade only consider pursuers 
	//within a 'threat range'
	const double ThreatRange = 100.0;
	if (ToPursuer.LengthSq() > ThreatRange * ThreatRange) return vec::Zero;

	//the lookahead time is propotional to the distance between the pursuer
	//and the pursuer; and is inversely proportional to the sum of the
	//agents' velocities
	double LookAheadTime = ToPursuer.Length() /
		(steeringEntity->max_speed + pursuer->vel.Length());

	//now flee away from predicted future position of the pursuer
	return Flee(pursuer->pos + pursuer->vel * LookAheadTime);
}

//--------------------------- Wander -------------------------------------
//
//  This behavior makes the agent wander about randomly
//  @WanderRad: Ganes que te de posar-se a fer cercles (the radius of the constraining circle for the wander behavior)
//  @WanderDist: Velocitat a la que va (distance the wander circle is projected in front of the agent)
//  @WanderJitterPerSec: Trompicones que dona (the maximum amount of displacement along the circle each frame)
//------------------------------------------------------------------------
vec SteeringBehavior::Wander(float WanderJitterPerSec, float dt)
{
	//first, add a small random vector to the target's position
	//Random between -1 and 1 with more weight at 0.
	float angleChange = (Rand::rollf() - Rand::rollf());
	m_vWanderTarget.RotateAroundOriginRads(angleChange * WanderJitterPerSec * dt);
	m_vWanderTarget.Normalize();

	//move the target into a position WanderDist in front of the agent
	vec Target = steeringEntity->pos + m_vWanderTarget;

	//project the target into world space
	//vec Target = PointToWorldSpace(target, steeringEntity->Heading(), steeringEntity->pos);

	//and steer towards it
	vec Origin = steeringEntity->pos;
	vec Ret = Target - Origin;

	return Ret;
}

//--------------------------- BoundsAvoidance --------------------------------
//
// This returns a steering force that will keep the agent in an area
//------------------------------------------------------------------------
vec SteeringBehavior::BoundsAvoidance(const BoxBounds& m_bounds, float frontFeelerLength, float sideFeelerLength)
{
	avoidingBounds = false;

	vec heading = steeringEntity->Heading();

	vec m_Feelers[3];
	//feeler pointing straight in front
	m_Feelers[0] = steeringEntity->pos + frontFeelerLength * heading;
	m_Feelers[0].DebugDraw();
	//feeler to left
	vec temp = heading.RotatedAroundOriginRads(Angles::Pi * -0.3f);
	m_Feelers[1] = steeringEntity->pos + sideFeelerLength * temp;
	m_Feelers[1].DebugDraw();
	//feeler to right
	temp = heading.RotatedAroundOriginRads(Angles::Pi * 0.3f);
	m_Feelers[2] = steeringEntity->pos + sideFeelerLength * temp;
	m_Feelers[2].DebugDraw();

	vec wallsv[5] = {
		vec(m_bounds.Left(), m_bounds.Top()), // 0->1 Vertical wall
		vec(m_bounds.Left(), m_bounds.Bottom()), // 1->2 Horizontal wall
		vec(m_bounds.Right(), m_bounds.Bottom()), // 2->3 Vertical wall
		vec(m_bounds.Right(), m_bounds.Top()), // 3->4 Horizontal wall
		vec(m_bounds.Left(), m_bounds.Top())
	};

	float DistToClosestVerticalIP = Mates::MaxFloat;
	float DistToClosestHorizontalIP = Mates::MaxFloat;
	vec ClosestVerticalPoint;
	vec ClosestHorizontalPoint;
	int ClosestVerticalWall = -1; //index in wallsv
	int ClosestHorizontalWall = -1; //index in wallsv

	vec SteeringForce = vec::Zero;

	for (unsigned int feeler = 0; feeler < 3; ++feeler) {

		for (int i = 0; i < 4; i++) {

			//(wallsv[i + 1] - wallsv[i]).DebugDrawAsArrow(wallsv[i]);

			float DistToThisIP;
			vec ClosestPoint;
			if (LineIntersection2D(steeringEntity->pos,
				m_Feelers[feeler],
				wallsv[i], wallsv[i + 1],
				DistToThisIP, ClosestPoint))
			{
				if (i == 0 || i == 2) {
					if (DistToThisIP < DistToClosestVerticalIP)
					{
						DistToClosestVerticalIP = DistToThisIP;
						ClosestVerticalWall = i;
						ClosestVerticalPoint = ClosestPoint;
					}
				}
				else {
					if (DistToThisIP < DistToClosestHorizontalIP)
					{
						DistToClosestHorizontalIP = DistToThisIP;
						ClosestHorizontalWall = i;
						ClosestHorizontalPoint = ClosestPoint;
					}
				}
			}
		}

		//if an intersection point has been detected, calculate a force   that will direct the agent away
		if (ClosestVerticalWall != -1)
		{
			//calculate by what distance the projected position of the agent will overshoot the wall
			vec OverShoot = m_Feelers[feeler] - ClosestVerticalPoint;

			//(wallsv[ClosestVerticalWall + 1] - wallsv[ClosestVerticalWall]).DebugDrawAsArrow(wallsv[ClosestVerticalWall], 255, 0, 0);

			vec temp = (wallsv[ClosestVerticalWall] - wallsv[ClosestVerticalWall + 1]).Normalized();
			vec normal(-temp.y, temp.x);

			//create a force in the direction of the wall normal, with a magnitude of the overshoot
			SteeringForce += normal * OverShoot.Length();
			avoidingBounds = true;
		}
		if (ClosestHorizontalWall != -1)
		{
			//calculate by what distance the projected position of the agent will overshoot the wall
			vec OverShoot = m_Feelers[feeler] - ClosestHorizontalPoint;

			//(wallsv[ClosestHorizontalWall + 1] - wallsv[ClosestHorizontalWall]).DebugDrawAsArrow(wallsv[ClosestHorizontalWall], 255, 0, 0);

			vec temp = (wallsv[ClosestHorizontalWall] - wallsv[ClosestHorizontalWall + 1]).Normalized();
			vec normal(-temp.y, temp.x);

			//create a force in the direction of the wall normal, with a magnitude of the overshoot
			SteeringForce += normal * OverShoot.Length();
			avoidingBounds = true;
		}

	}

	return SteeringForce;
}

//------------------------- GetHidingPosition ----------------------------
//
//  Given the position of a hunter, and the position and radius of
//  an obstacle, this method calculates a position DistanceFromBoundary 
//  away from its bounding radius and directly opposite the hunter
//------------------------------------------------------------------------
vec SteeringBehavior::GetHidingPosition(vec posOb,
	const float     radiusOb,
	vec posHunter)
{
	//calculate how far away the agent is to be from the chosen obstacle's
	//bounding radius
	const float DistanceFromBoundary = 30.0;
	float       DistAway = radiusOb + DistanceFromBoundary;

	//calculate the heading toward the object from the hunter
	vec ToOb = (posOb - posHunter).Normalized();

	//scale it to size and add to the obstacles position to get
	//the hiding spot.
	return (ToOb * DistAway) + posOb;
}

/*
//------------------------------- FollowPath -----------------------------
//
//  Given a series of vecs, this method produces a force that will
//  move the agent along the waypoints in order. The agent uses the
// 'Seek' behavior to move to the next waypoint - unless it is the last
//  waypoint, in which case it 'Arrives'
//------------------------------------------------------------------------
vec SteeringBehavior::FollowPath()
{
	//move to next target if close enough to current target (working in distance squared space)
	if(Vec2DDistanceSq(m_pPath->CurrentWaypoint(), steeringEntity->pos) < m_dWaypointSeekDistSq)
	{
		m_pPath->SetNextWaypoint();
	}

	if (!m_pPath->Finished())
	{
		return Seek(m_pPath->CurrentWaypoint());
	}
	else
	{
		return Arrive(m_pPath->CurrentWaypoint(), normal);
	}
}
*/

//------------------------- Offset Pursuit -------------------------------
//
//  Produces a steering force that keeps a SteeringEntity at a specified offset
//  from a leader SteeringEntity
//------------------------------------------------------------------------
vec SteeringBehavior::OffsetPursuit(const Entity* leader, float offset)
{
	vec leaderHeading = leader->vel.Normalized();
	vec displacement = leaderHeading * -offset;

	//calculate the offset's position in world space
	vec WorldOffsetPos = PointToWorldSpace(displacement,
		leaderHeading,
		leader->pos);

	vec ToOffset = WorldOffsetPos - steeringEntity->pos;

	//the lookahead time is propotional to the distance between the leader
	//and the pursuer; and is inversely proportional to the sum of both
	//agent's velocities
	float LookAheadTime = ToOffset.Length() /
		(steeringEntity->max_speed + leader->vel.Length());

	//now Arrive at the predicted future position of the offset
	return Arrive(WorldOffsetPos + leader->vel * LookAheadTime, fast);
}

/*
vec SteeringBehavior::TileMapAvoidance(GaemTileMap* map)
{
	float minDistToCenterSq = Mates::MaxFloat;
	vec closestObstacle;

	int xLeft = Tile::ToTiles(steeringEntity->pos.x - steeringEntity->radius - Tile::Size / 2);
	int xRight = Tile::ToTiles(steeringEntity->pos.x + steeringEntity->radius + Tile::Size / 2);
	int yTop = Tile::ToTiles(steeringEntity->pos.y - steeringEntity->radius - Tile::Size / 2);
	int yBottom = Tile::ToTiles(steeringEntity->pos.y + steeringEntity->radius + Tile::Size / 2);
	CircleBounds me = steeringEntity->Bounds();
	for (int y = yTop; y <= yBottom; y++) {
		for (int x = xLeft; x <= xRight; x++) {
			//vec(x * 16 + 8, y * 16 + 8).DebugDraw();
			if (map->GetTile(x, y).isSolid()) {
				BoxBounds tile = Tile::Bounds(x, y);
				if (tile.Contains(me.pos)) {
					minDistToCenterSq = 0;
					closestObstacle = tile.Center();
					goto im_inside; //exit early
				}
				vec closestPoint = tile.ClosestPointInBounds(me.pos);
				float distSq = closestPoint.DistanceSq(me.pos);
				if (distSq < minDistToCenterSq) {
					minDistToCenterSq = distSq;
					closestObstacle = closestPoint;
				}
			}
		}
	}

im_inside:

	//closestObstacle.DebugDraw();

	vec steeringForce;
	avoidingTileMap = (minDistToCenterSq < Mates::MaxFloat);
	if (avoidingTileMap) {
		float dist = sqrt(minDistToCenterSq) - me.radius;
		float multiplier;
		if (dist < 0) { //touching
			multiplier = steeringEntity->max_speed;
		}
		else {
			multiplier = (2.0f + (me.radius - dist) / dist);
		}

		steeringForce = (steeringEntity->pos - closestObstacle).Normalized() * multiplier;
		//std::cout << "dist=" << dist << " force=" << steeringForce.Length() << std::endl;
	}

	return steeringForce;
}
*/
