#pragma once

#include "entity.h"
#include "noncopyable.h"
#include "physics_world.h"

enum class PhysicsCategory
{
	Generic,
	Ship,
	Rock,
};

// Owns one Box2D body and exposes its simulation state without maintaining a
// second position or velocity.
class PhysicsEntity : public Lifecycle, NonCopyable
{
public:
	PhysicsEntity(vec position, b2BodyType bodyType, PhysicsCategory category);
	virtual ~PhysicsEntity();

	[[nodiscard]] vec Position() const;
	void SetPosition(vec position);

	[[nodiscard]] vec Velocity() const;
	void SetVelocity(vec velocity);

	[[nodiscard]] float RotationRads() const;
	void SetRotationRads(float angle);
	void SetAngularVelocity(float radiansPerSecond);
	void SetAngularDamping(float damping);
	void SetFixedRotation(bool fixed);

	[[nodiscard]] bool IsTouching(const PhysicsEntity& other) const;
	[[nodiscard]] const PhysicsEntity* FindTouching(PhysicsCategory category) const;

protected:
	void AddCircle(vec localCenter, float radius);
	void AddCapsule(vec localPoint1, vec localPoint2, float radius);

private:
	b2BodyId body;
	PhysicsCategory category;
};

class PhysicsCircleEntity : public PhysicsEntity
{
public:
	PhysicsCircleEntity(vec position, float radius, b2BodyType bodyType, PhysicsCategory category = PhysicsCategory::Generic);

	[[nodiscard]] float Radius() const { return radius; }
	[[nodiscard]] CircleBounds Bounds() const { return CircleBounds(Position(), radius); }

private:
	float radius;
};

class PhysicsCapsuleEntity : public PhysicsEntity
{
public:
	PhysicsCapsuleEntity(vec position, vec localPoint1, vec localPoint2, float radius, b2BodyType bodyType, PhysicsCategory category = PhysicsCategory::Generic);
};
