#include "enemy.h"

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "tweeny.h"
#include "collide.h"
#include "rock.h"
#include "stroke.h"
#include "particles.h"
#include "imgui.h"

extern float kParticlesSpeed;
extern float kTimeBetweenStrokeSegments;

const float kEnemyRadius = 40.f;
const float kEnemyMaxSpeed = 220.f;
const float kMaxTurnRate = Angles::Tau / 8;

extern SDL_Color foamColor;
extern SDL_Color surfaceColor;
extern SDL_Color alphaSurfaceColor;

Enemy::Enemy(vec pos) 
	: SteeringEntity(pos, kEnemyRadius, kEnemyMaxSpeed, vec(1,0))
	, steering(this)
{
	outerStroke.SetMaxJoints(100);
	outerStroke.SetJointLifetime(4.5f);
	outerStroke.SetStartInnerColor(foamColor);
	outerStroke.SetStartOuterColor(foamColor);
	outerStroke.SetEndInnerColor(alphaSurfaceColor);
	outerStroke.SetEndOuterColor(alphaSurfaceColor);
	outerStroke.SetStartThickness(0.2f);
	outerStroke.SetOffset(-0.9f);
	outerStroke.SetColorOffset(25.f);

	innerStroke.SetMaxJoints(100);
	innerStroke.SetJointLifetime(4.5f);
	innerStroke.SetStartInnerColor(surfaceColor);
	innerStroke.SetStartOuterColor(surfaceColor);
	innerStroke.SetEndInnerColor(alphaSurfaceColor);
	innerStroke.SetEndOuterColor(alphaSurfaceColor);
	innerStroke.SetStartThickness(0.2f);
	innerStroke.SetOffset(-0.9f);
	innerStroke.SetColorOffset(25.0f);
}

void Enemy::Update(float dt) {

	innerStroke.Calculate(dt);
	outerStroke.Calculate(dt);

	// CALCULATE VELOCITY
	vec obstacleAvoidance = steering.ObstacleAvoidance(Rock::GetAll(), 400.f);
	vec wander = steering.Wander(1, dt);

	vec steeringForce;
	steeringForce += obstacleAvoidance * 1000.f;
	//steeringForce += wander * 10.f;

	(50 * obstacleAvoidance).DebugDrawAsArrow(pos, 255, 0, 255);
	(50 * wander).DebugDrawAsArrow(pos, 255, 0, 0);

	steeringForce.DebugDrawAsArrow(pos, 255, 255, 255);

	vel = vel.RotatedToFacePositionRads(steeringForce, kMaxTurnRate * dt);
	vel = vel.Normalized() * (kEnemyMaxSpeed);

	pos += vel * dt;
	
	float speed = vel.Length();
	distanceSailed += speed;

	timer += dt;
	if (timer > kTimeBetweenStrokeSegments) {
		timer -= kTimeBetweenStrokeSegments;
		float thickness = std::max(speed / kEnemyMaxSpeed, 0.3f) * 3.5f;
		float wobblyness = sin(distanceSailed * 0.008f) * 2.f;
		innerStroke.AddJoint(pos, thickness * 15.f + wobblyness);
		outerStroke.AddJoint(pos, thickness * 20.f + wobblyness);
	}

	if (speed > kParticlesSpeed) {
		Particles::waterTrail.pos = pos;
		Particles::waterTrail.Spawn(dt);
	}
}

void Enemy::Draw() {
	bool hitAnim = false; // (immunityTimer > 0 && ((int)((immunityTimer) * 12)) % 3);

	if (hitAnim) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.5f, 0.5f, 0.5f);
	}

	float angle = vel.AngleDegs();
	Window::Draw(Assets::shipTexture, pos)
		.withOrigin(Assets::shipTexture->w / 2, Assets::shipTexture->h / 2)
		.withRotationDegs(angle)
		.withScale(0.5f);

	Assets::tintShader.Deactivate();
	
	Bounds().DebugDraw();
}

