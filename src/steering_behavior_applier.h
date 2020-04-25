#pragma once
//Based on the work of: Mat Buckland (fup@ai-junkie.com)

#include "steering_behavior.h"

// Adds together multiple steerig behaviour forces
// Only use this if you are super lazy and don't want to use SteeringBehavior directly 
struct SteeringBehaviorApplier : SteeringBehavior {
public:
	SteeringBehaviorApplier(SteeringEntity* agent, float max_force = 10.f) : SteeringBehavior(agent), max_force(max_force){ }

	bool AccumulateForce(vec& sf, const vec& ForceToAdd);

	vec Force() const { return m_vSteeringForce; }

	//multipliers. These can be adjusted to effect strength of the  
	//appropriate behavior. Useful to get flocking the way you require
	//for example.
	float m_dWeightWander = 1;
	float m_dWeightObstacleAvoidance = 2;
	float m_dWeightBoundsAvoidance = 3;
	float m_dWeightSeek = 1;
	float m_dWeightForward = 1;
	float m_dWeightFlee = 1;
	float m_dWeightArrive = 1;
	float m_dWeightPursuit = 1;
	float m_dWeightOffsetPursuit = 0.5;
	float m_dWeightHide = 1;
	float m_dWeightTileMapAvoidance = 2;

//private:

	Entity* pursuitTarget = nullptr;
	Entity* seekTarget = nullptr;
	Entity* fleeTarget = nullptr;
	Entity* hideTarget = nullptr;
	
	sf::Rect<float> m_bounds; //For bounds avoidance
	
	std::vector<CircleEntity*>* obstacles;

	float max_force;

	//what type of method is used to sum any active behavior
	TileMap* m_tilemap;

	//the steering force created by the combined effect of all
	//the selected behaviors
	vec m_vSteeringForce;
	//any offset used for formations or offset pursuit
	float m_vOffset;

	enum behavior_type
	{
		none = 0,
		seek = 1,
		flee = 2,
		arrive = 4,
		wander = 8,
		obstacle_avoidance = 16,
		bounds_avoidance = 32,
		pursuit = 64,
		hide = 128,
		flock = 256,
		offset_pursuit = 512,
		forward = 1024,
		tilemap_avoidance = 2048,
	};
	//this function tests if a specific bit of m_iFlags is set
	bool On(behavior_type bt) const { return (m_iFlags & bt) == bt; }

	//binary flags to indicate whether or not a behavior should be active
	int m_iFlags = 0;

	//default
	Deceleration m_Deceleration = normal;

public:

	//calculates and sums the steering forces from any active behaviors
	vec CalculateWeightedSum(float dt);
	vec CalculatePrioritized(float dt);

	void ForwardOn() { m_iFlags |= forward; }
	void FleeOn(Entity* target) { fleeTarget = target; m_iFlags |= flee; }
	void SeekOn(Entity* target) { seekTarget = target; m_iFlags |= seek; }
	void ArriveOn(Entity* target) { seekTarget = target; m_iFlags |= arrive; }
	void WanderOn() { m_iFlags |= wander; }
	void PursuitOn(Entity* v) { m_iFlags |= pursuit; pursuitTarget = v; }
	void ObstacleAvoidanceOn() { m_iFlags |= obstacle_avoidance; }
	void BoundsAvoidanceOn(sf::Rect<float> bounds) { m_bounds = bounds; m_iFlags |= bounds_avoidance; }
	void HideOn(Entity* v) { m_iFlags |= hide; hideTarget = v; }
	void OffsetPursuitOn(Entity* v1, const float offset) { m_iFlags |= offset_pursuit; m_vOffset = offset; pursuitTarget = v1; }
	void TileMapAvoidanceOn(TileMap* tilemap) { m_iFlags |= tilemap_avoidance; m_tilemap = tilemap; }

	void ForwardOff() { if (On(forward)) m_iFlags ^= forward; }
	void FleeOff() { if (On(flee)) m_iFlags ^= flee; }
	void SeekOff() { if (On(seek)) m_iFlags ^= seek; }
	void ArriveOff() { if (On(arrive)) m_iFlags ^= arrive; }
	void WanderOff() { if (On(wander)) m_iFlags ^= wander; }
	void PursuitOff() { if (On(pursuit)) m_iFlags ^= pursuit; }
	void ObstacleAvoidanceOff() { if (On(obstacle_avoidance)) m_iFlags ^= obstacle_avoidance; }
	void BoundsAvoidanceOff() { if (On(bounds_avoidance)) m_iFlags ^= bounds_avoidance; }
	void HideOff() { if (On(hide)) m_iFlags ^= hide; }
	void OffsetPursuitOff() { if (On(offset_pursuit)) m_iFlags ^= offset_pursuit; }
	void TileMapAvoidanceOff() { if (On(tilemap_avoidance)) m_iFlags ^= tilemap_avoidance; }

	bool isFleeOn() const { return On(flee); }
	bool isSeekOn() const { return On(seek); }
	bool isArriveOn() const { return On(arrive); }
	bool isWanderOn() const { return On(wander); }
	bool isPursuitOn() const { return On(pursuit); }
	bool isObstacleAvoidanceOn() const { return On(obstacle_avoidance); }
	bool isBoundsAvoidanceOn() const { return On(bounds_avoidance); }
	bool isHideOn() const { return On(hide); }
	bool isOffsetPursuitOn() const { return On(offset_pursuit); }
	bool isTileMapAvoidanceOn() { return On(tilemap_avoidance); }
};


//--------------------- AccumulateForce ----------------------------------
//
//  This function calculates how much of its max steering force the 
//  SteeringEntity has left to apply and then applies that amount of the
//  force to add.
//------------------------------------------------------------------------
inline bool SteeringBehaviorApplier::AccumulateForce(vec& RunningTot, const vec& ForceToAdd)
{

	//calculate how much steering force the SteeringEntity has used so far
	float MagnitudeSoFar = RunningTot.Length();

	//calculate how much steering force remains to be used by this SteeringEntity
	float MagnitudeRemaining = max_force - MagnitudeSoFar;

	//return false if there is no more force left to use
	if (MagnitudeRemaining <= 0.0) return false;

	//calculate the magnitude of the force we want to add
	float MagnitudeToAdd = ForceToAdd.Length();

	//if the magnitude of the sum of ForceToAdd and the running total
	//does not exceed the maximum force available to this SteeringEntity, just
	//add together. Otherwise add as much of the ForceToAdd vector is
	//possible without going over the max.
	if (MagnitudeToAdd < MagnitudeRemaining)
	{
		RunningTot += ForceToAdd;
	}

	else
	{
		//add it to the steering force
		RunningTot += (ForceToAdd.Normalized() * MagnitudeRemaining);
		return false;
	}

	return true;
}


//---------------------- CalculatePrioritized ----------------------------
//
//  this method calls each active steering behavior in order of priority
//  and acumulates their forces until the max steering force magnitude
//  is reached, at which time the function returns the steering force 
//  accumulated to that  point
//------------------------------------------------------------------------
inline vec SteeringBehaviorApplier::CalculatePrioritized(float dt)
{
	//reset the steering force
	m_vSteeringForce.Zero();

	vec force;

	if (On(bounds_avoidance))
	{
		force = BoundsAvoidance(m_bounds) * m_dWeightBoundsAvoidance;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(tilemap_avoidance))
	{
		force = TileMapAvoidance(m_tilemap) * m_dWeightTileMapAvoidance;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(obstacle_avoidance))
	{
		force = ObstacleAvoidance(*obstacles) * m_dWeightObstacleAvoidance;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(flee))
	{
		assert(fleeTarget && "Flee target not assigned");
		force = Flee(fleeTarget->pos) * m_dWeightFlee;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(seek))
	{
		assert(seekTarget && "Seek target not assigned");
		force = Seek(seekTarget->pos) * m_dWeightSeek;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}


	if (On(arrive))
	{
		assert(seekTarget && "Arrive target not assigned");
		force = Arrive(seekTarget->pos, m_Deceleration) * m_dWeightArrive;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(wander))
	{
		force = Wander(dt) * m_dWeightWander;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(pursuit))
	{
		assert(pursuitTarget && "pursuit target not assigned");

		force = Pursuit(pursuitTarget) * m_dWeightPursuit;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(offset_pursuit))
	{
		assert(pursuitTarget && "pursuit target not assigned");

		force = OffsetPursuit(pursuitTarget, m_vOffset);

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(hide))
	{
		assert(hideTarget && "Hide target not assigned");

		force = Hide(hideTarget, *obstacles) * m_dWeightHide;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	if (On(forward)) {
		force = Forward() * m_dWeightForward;

		if (!AccumulateForce(m_vSteeringForce, force)) return m_vSteeringForce;
	}

	return m_vSteeringForce;
}


//---------------------- CalculateWeightedSum ----------------------------
//
//  this simply sums up all the active behaviors X their weights and 
//  truncates the result to the max available steering force before 
//  returning
//------------------------------------------------------------------------
inline vec SteeringBehaviorApplier::CalculateWeightedSum(float dt)
{
	//reset the steering force
	m_vSteeringForce.Zero();

	if (On(bounds_avoidance))
	{
		vec force = BoundsAvoidance(m_bounds) * m_dWeightBoundsAvoidance;
		m_vSteeringForce += force;
	}

	if (On(tilemap_avoidance))
	{
		vec force = TileMapAvoidance(m_tilemap) * m_dWeightTileMapAvoidance;
		m_vSteeringForce += force;
	}

	if (On(obstacle_avoidance))
	{
		vec force = ObstacleAvoidance(*obstacles) * m_dWeightObstacleAvoidance;
		m_vSteeringForce += force;
	}

	if (On(flee))
	{
		assert(fleeTarget && "Flee target not assigned");
		m_vSteeringForce += Flee(fleeTarget->pos) * m_dWeightFlee;
	}

	if (On(seek))
	{
		assert(seekTarget && "Seek target not assigned");
		m_vSteeringForce += Seek(seekTarget->pos) * m_dWeightSeek;
	}

	if (On(arrive))
	{
		assert(seekTarget && "Arrive target not assigned");
		m_vSteeringForce += Arrive(seekTarget->pos, m_Deceleration) * m_dWeightArrive;
	}

	if (On(wander))
	{
		vec force = Wander(dt) * m_dWeightWander;
		m_vSteeringForce += force;
	}

	if (On(pursuit))
	{
		assert(pursuitTarget && "pursuit target not assigned");

		vec force = Pursuit(pursuitTarget) * m_dWeightPursuit;
		m_vSteeringForce += force;
	}

	if (On(offset_pursuit))
	{
		assert(pursuitTarget && "pursuit target not assigned");

		vec force = OffsetPursuit(pursuitTarget, m_vOffset) * m_dWeightOffsetPursuit;
		m_vSteeringForce += force;
	}

	if (On(hide))
	{
		assert(hideTarget && "Hide target not assigned");

		vec force = Hide(hideTarget, *obstacles) * m_dWeightHide;
		m_vSteeringForce += force;
	}

	if (On(forward)) {
		vec force = Forward() * m_dWeightForward;
		m_vSteeringForce += force;
	}

	m_vSteeringForce.Truncate(max_force);
	return m_vSteeringForce;
}
