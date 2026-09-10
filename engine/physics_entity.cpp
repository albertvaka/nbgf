#include "physics_entity.h"

#include <vector>

namespace {

b2Vec2 ToB2(vec value)
{
	return { value.x, value.y };
}

vec FromB2(b2Vec2 value)
{
	return vec(value.x, value.y);
}

b2ShapeDef ShapeDefinition(void* userData)
{
	b2ShapeDef definition = b2DefaultShapeDef();
	definition.userData = userData;
	definition.density = 1.0f;
	definition.material.friction = 0.0f;
	definition.material.restitution = 0.3f;
	return definition;
}

} // namespace

PhysicsEntity::PhysicsEntity(vec position, b2BodyType bodyType, PhysicsCategory category)
	: category(category)
{
	b2BodyDef definition = b2DefaultBodyDef();
	definition.type = bodyType;
	definition.position = ToB2(position);
	definition.gravityScale = 0.0f;
	definition.fixedRotation = true;
	definition.enableSleep = false;
	definition.isBullet = bodyType == b2_dynamicBody;
	body = b2CreateBody(PhysicsWorld::Instance().Id(), &definition);
}

PhysicsEntity::~PhysicsEntity()
{
	if (b2Body_IsValid(body)) {
		b2DestroyBody(body);
	}
}

vec PhysicsEntity::Position() const
{
	return FromB2(b2Body_GetPosition(body));
}

void PhysicsEntity::SetPosition(vec position)
{
	b2Body_SetTransform(body, ToB2(position), b2Body_GetRotation(body));
}

vec PhysicsEntity::Velocity() const
{
	return FromB2(b2Body_GetLinearVelocity(body));
}

void PhysicsEntity::SetVelocity(vec velocity)
{
	b2Body_SetLinearVelocity(body, ToB2(velocity));
}

float PhysicsEntity::RotationRads() const
{
	b2Rot rotation = b2Body_GetRotation(body);
	return atan2f(rotation.s, rotation.c);
}

void PhysicsEntity::SetRotationRads(float angle)
{
	b2Body_SetTransform(body, b2Body_GetPosition(body), b2MakeRot(angle));
}

void PhysicsEntity::SetAngularVelocity(float radiansPerSecond)
{
	b2Body_SetAngularVelocity(body, radiansPerSecond);
}

void PhysicsEntity::SetAngularDamping(float damping)
{
	b2Body_SetAngularDamping(body, damping);
}

void PhysicsEntity::SetFixedRotation(bool fixed)
{
	b2Body_SetFixedRotation(body, fixed);
}

bool PhysicsEntity::IsTouching(const PhysicsEntity& other) const
{
	int capacity = b2Body_GetContactCapacity(body);
	if (capacity == 0) {
		return false;
	}

	std::vector<b2ContactData> contacts(capacity);
	int count = b2Body_GetContactData(body, contacts.data(), capacity);
	for (int i = 0; i < count; ++i) {
		void* first = b2Shape_GetUserData(contacts[i].shapeIdA);
		void* second = b2Shape_GetUserData(contacts[i].shapeIdB);
		if (first == &other || second == &other) {
			return true;
		}
	}
	return false;
}

const PhysicsEntity* PhysicsEntity::FindTouching(PhysicsCategory desiredCategory) const
{
	int capacity = b2Body_GetContactCapacity(body);
	if (capacity == 0) {
		return nullptr;
	}

	std::vector<b2ContactData> contacts(capacity);
	int count = b2Body_GetContactData(body, contacts.data(), capacity);
	for (int i = 0; i < count; ++i) {
		PhysicsEntity* first = static_cast<PhysicsEntity*>(b2Shape_GetUserData(contacts[i].shapeIdA));
		PhysicsEntity* second = static_cast<PhysicsEntity*>(b2Shape_GetUserData(contacts[i].shapeIdB));
		PhysicsEntity* other = first == this ? second : first;
		if (other != this && other != nullptr && other->category == desiredCategory) {
			return other;
		}
	}
	return nullptr;
}

void PhysicsEntity::AddCircle(vec localCenter, float radius)
{
	b2Circle circle = { ToB2(localCenter), radius };
	b2ShapeDef definition = ShapeDefinition(this);
	b2CreateCircleShape(body, &definition, &circle);
}

void PhysicsEntity::AddCapsule(vec localPoint1, vec localPoint2, float radius)
{
	b2Capsule capsule = { ToB2(localPoint1), ToB2(localPoint2), radius };
	b2ShapeDef definition = ShapeDefinition(this);
	b2CreateCapsuleShape(body, &definition, &capsule);
}

PhysicsCircleEntity::PhysicsCircleEntity(vec position, float radius, b2BodyType bodyType, PhysicsCategory category)
	: PhysicsEntity(position, bodyType, category)
	, radius(radius)
{
	AddCircle(vec::Zero, radius);
}

PhysicsCapsuleEntity::PhysicsCapsuleEntity(vec position, vec localPoint1, vec localPoint2, float radius, b2BodyType bodyType, PhysicsCategory category)
	: PhysicsEntity(position, bodyType, category)
{
	AddCapsule(localPoint1, localPoint2, radius);
}
