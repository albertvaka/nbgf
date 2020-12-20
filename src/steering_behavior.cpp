//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include "steering_behavior.h"

#include "angles.h"
#include "rand.h"
#include "steering_entity.h"
#include "tilemap.h"

// TODO: pass these as params to Wander
// Valors putamare pel ratpenat
const float WanderRad    = 0.37f; //Ganes que te de posar-se a fer cercles (the radius of the constraining circle for the wander behavior)
const float WanderDist   = 8.2f; //Velocitat a la que va (distance the wander circle is projected in front of the agent)
const float WanderJitterPerSec = 45.f; //Trompicones que dona (the maximum amount of displacement along the circle each frame)

SteeringBehavior::SteeringBehavior(SteeringEntity* agent) : steeringEntity(agent)
{
	//create a vector to a target position on the wander circle
	float theta = Rand::rollf() * Angles::Tau;
	m_vWanderTarget = vec(WanderRad * cos(theta), WanderRad * sin(theta));
}


//------------------------------- Forward -----------------------------------
vec SteeringBehavior::Forward()
{
	vec force = steeringEntity->Heading()*steeringEntity->max_speed;
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

	//return (DesiredVelocity - steeringEntity->vel);
	return DesiredVelocity; // FIXME: Since we are overriding the velocity, we can't use the calculation above
}

//----------------------------- Flee -------------------------------------
//
//  Does the opposite of Seek
//------------------------------------------------------------------------
vec SteeringBehavior::Flee(vec TargetPos)
{
	//only flee if the target is within 'panic distance'. Work in distance
	//squared space.
	/* const float PanicDistanceSq = 100.0f * 100.0;
	if (Vec2DDistanceSq(steeringEntity->pos, target) > PanicDistanceSq)
	{
	return vec(0,0);
	}
	*/

	vec DesiredVelocity = (steeringEntity->pos - TargetPos).Normalized() * steeringEntity->max_speed;

	//return (DesiredVelocity - steeringEntity->vel);
	return DesiredVelocity; // FIXME: Since we are overriding the velocity, we can't use the calculation above
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

	if (dist > 5.f)
	{
		//because Deceleration is enumerated as an int, this value is required
		//to provide fine tweaking of the deceleration..
		const float DecelerationTweaker = 0.3f;

		//calculate the vel required to reach the target given the desired
		//deceleration
		float vel =  dist / ((float)deceleration * DecelerationTweaker);     

		//make sure the velocity does not exceed the max
        vel = std::min(vel, steeringEntity->max_speed);

		//from here proceed just like Seek except we don't need to normalize 
		//the ToTarget vector because we have already gone to the trouble
		//of calculating its length: dist. 
		vec DesiredVelocity =  ToTarget * vel / dist;

		return (DesiredVelocity - steeringEntity->vel);
	}

	return vec(0,0);
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

	if ( (ToEvader.Dot(heading) > 0) && (RelativeHeading < -0.95))  //acos(0.95)=18 degs
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

//--------------------------- Wander -------------------------------------
//
//  This behavior makes the agent wander about randomly
//------------------------------------------------------------------------
vec SteeringBehavior::Wander(float dt)
{ 
	//this behavior is dependent on the update rate, so this line must
	//be included when using time independent framerate.
	float JitterThisTimeSlice = WanderJitterPerSec * dt;

	//Random between -1 and 1 with more weight at 0.
	float x = (Rand::rollf() - Rand::rollf());
	float y = (Rand::rollf() - Rand::rollf());

	//first, add a small random vector to the target's position
	m_vWanderTarget += vec(x * JitterThisTimeSlice, y * JitterThisTimeSlice);

	//reproject this new vector back on to a unit circle
	m_vWanderTarget.Normalize();

	//increase the length of the vector to the same as the radius
	//of the wander circle
	m_vWanderTarget *= WanderRad;

	//move the target into a position WanderDist in front of the agent
	vec target = m_vWanderTarget + vec(WanderDist, 0);

	//project the target into world space
	vec Target = PointToWorldSpace(target, steeringEntity->Heading(), steeringEntity->pos);

	//and steer towards it
	vec Origin = steeringEntity->pos;
	vec Ret = Target - Origin;

	(steeringEntity->pos + m_vWanderTarget * 20).Debuggerino(0,255,0);

	return Ret;
}

//--------------------------- BoundsAvoidance --------------------------------
//
//  This returns a steering force that will keep the agent in an area
//------------------------------------------------------------------------
vec SteeringBehavior::BoundsAvoidance(const Bounds& m_bounds)
{
	vec m_Feelers[3];
	const float m_dWallDetectionFeelerLength = 20; //Front feeler length. Lateral feelers will be half as long.

	vec heading = steeringEntity->Heading();

	//feeler pointing straight in front
	m_Feelers[0] = steeringEntity->pos + m_dWallDetectionFeelerLength * heading;
	m_Feelers[0].Debuggerino();

	//feeler to left
	vec temp = heading.RotatedAroundOriginRads(Angles::Pi * -0.3f);
	m_Feelers[1] = steeringEntity->pos + m_dWallDetectionFeelerLength/2.0f * temp;
	//m_Feelers[1].Debuggerino();

	//feeler to right
	temp = heading.RotatedAroundOriginRads(Angles::Pi * 0.3f);
	m_Feelers[2] = steeringEntity->pos + m_dWallDetectionFeelerLength/2.0f * temp;
	//m_Feelers[2].Debuggerino();

	vec wallsv[5] = {vec(m_bounds.left, m_bounds.top),
		vec(m_bounds.left, m_bounds.top+m_bounds.height),
		vec(m_bounds.left+m_bounds.width, m_bounds.top+m_bounds.height),
		vec(m_bounds.left+m_bounds.width, m_bounds.top),
		vec(m_bounds.left, m_bounds.top)
	};

	float DistToClosestIP = Mates::MaxFloat;
	vec ClosestPoint;
	int ClosestWall = -1; //index in wallsv

	vec SteeringForce;
	avoidingBounds = false;

	//examine each feeler in turn
	for (unsigned int flr=0; flr < 3; ++flr) {

		//run through each wall checking for any intersection points
		for (int i = 0; i < 4; i++) {

			float DistToThisIP;
			vec point;
			if (LineIntersection2D(steeringEntity->pos,
				m_Feelers[flr],
				wallsv[i],
				wallsv[i+1],
				DistToThisIP,
				point))
			{
				//is this the closest found so far? If so keep a record
				if (DistToThisIP < DistToClosestIP)
				{
					DistToClosestIP = DistToThisIP;
					ClosestWall = i;
					ClosestPoint = point;
				}
			}
		}//next wall


		//if an intersection point has been detected, calculate a force  
		//that will direct the agent away
		if (ClosestWall != -1)
		{
			//calculate by what distance the projected position of the agent
			//will overshoot the wall
			vec OverShoot = m_Feelers[flr] - ClosestPoint;

			vec temp = (wallsv[ClosestWall] - wallsv[ClosestWall+1]).Normalized();
			vec normal (-temp.y,temp.x);

			//create a force in the direction of the wall normal, with a 
			//magnitude of the overshoot
			SteeringForce = normal * OverShoot.Length();
			avoidingBounds = true;
		}

	}//next feeler

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
vec SteeringBehavior::OffsetPursuit(const Entity*  leader, const float offset)
{
	vec leaderHeading = leader->vel.Normalized();
	vec displacement = leaderHeading * - offset;

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


vec SteeringBehavior::TileMapAvoidance(TileMap* map)
{
	float minDistToCenterSq = Mates::MaxFloat;
	vec closestObstacle;

	int xLeft = map->toTiles(steeringEntity->pos.x - steeringEntity->radius - Tile::size / 2);
	int xRight = map->toTiles(steeringEntity->pos.x + steeringEntity->radius + Tile::size /2);
	int yTop = map->toTiles(steeringEntity->pos.y - steeringEntity->radius - Tile::size / 2);
	int yBottom = map->toTiles(steeringEntity->pos.y + steeringEntity->radius + Tile::size / 2);
	CircleBounds me = steeringEntity->bounds();
	for (int y = yTop; y <= yBottom; y++) {
		for (int x = xLeft; x <= xRight; x++) {
			//vec(x * 16 + 8, y * 16 + 8).Debuggerino(sf::Color::White);
			if (map->getTile(x, y).isSolid()) {
				Bounds tile = map->getTileBounds(x, y);
				if (tile.Contains(me.pos)) {
					minDistToCenterSq = 0;
					closestObstacle = tile.Center();
					goto im_inside; //exit early
				}
				vec closestPoint = tile.ClosesPointInBounds(me.pos);
				float distSq = closestPoint.DistanceSq(me.pos);
				if (distSq < minDistToCenterSq) {
					minDistToCenterSq = distSq;
					closestObstacle = closestPoint;
				}
			}
		}
	}

im_inside:

	//closestObstacle.Debuggerino(sf::Color::Red);

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
