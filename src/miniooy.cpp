#include "miniooy.h"

#include "imgui.h"
#include "jumpman.h"
#include "collide.h"
#include "window_draw.h"
#include "assets.h"
#include "animation.h"
#include "rand.h"
#include "ooytear.h"
#include "particles.h"
#include "screen.h"
#include "common_enemy.h"
#include "common_tilemapcharacter.h"

constexpr const float kIntervalShoot = 1.5f;
constexpr const float kStartChasingRadius = 200.f;
constexpr const float kStopChasingRadius = 300.f;
constexpr const float kHorizontalDistanceToShoot = 40.f;
constexpr const float kTargetVerticalDistance = 120.f;
constexpr const float kVerticalDistanceToShoot = 60.f;
constexpr const float kTimeToChangeState = 1.2f;
constexpr const float kScale = 0.8f;
constexpr const float kRadius = 12.f*kScale;
constexpr const float kMaxSpeed = 110.f;

constexpr const float kTearVel = 100;
constexpr const float kRandomTearVel = 10;

constexpr const float kSteeringSeekWeightChasing = 300.f;
constexpr const float kSteeringSeekWeightIdle = 150.f;
constexpr const float kSteeringWanderWeight = 150.f;
constexpr const float kSteeringBoundsAvoidanceWeight = 600.f;
constexpr const float kSteeringTileMapAvoidanceWeight = 600.f;

constexpr const float kSteeringWanderRad = 4.8f; //Ganes que te de posar-se a fer cercles (the radius of the constraining circle for the wander behavior)
constexpr const static float kSteeringWanderDist = 6.5f; //Velocitat a la que va (distance the wander circle is projected in front of the agent)
constexpr const static float kSteeringWanderJitterPerSec = 175; //Trompicones que dona (the maximum amount of displacement along the circle each frame)

extern float mainClock;

constexpr const GPU_Rect directions[] = { AnimLib::OOY_CHASE_SE, AnimLib::OOY_CHASE_SW, AnimLib::OOY_CHASE_NW, AnimLib::OOY_CHASE_NE };

MiniOoy::MiniOoy(vec pos)
	: SteeringEntity(pos, kRadius, kMaxSpeed, vec(0, 0))
	, steering(this)
{
	screen = ScreenManager::instance()->FindScreenContaining(pos);
	bounds = ScreenManager::instance()->ScreenBounds(screen);
}

void MiniOoy::Update(float dt)
{
	if (!InSameScreenAsPlayer(screen)) {
		return;
	}

	vel += steering.BoundsAvoidance(bounds, kRadius*2, kRadius).Truncated(kSteeringTileMapAvoidanceWeight *dt);
	vel += steering.TileMapAvoidance(GaemTileMap::instance()).Truncated(kSteeringBoundsAvoidanceWeight *dt);

	JumpMan* player = JumpMan::instance();

	if (state == State::ENTER_CHASE || state == State::EXIT_CHASE) {
		timer += dt;
		if (timer >= kTimeToChangeState) {
			if (state == State::ENTER_CHASE) {
				state = State::CHASING;
				timer = 0.f;
			}
			else {
				state = State::IDLE;
			}
		}
	}

	switch (state) {
		case State::IDLE:
		{
			vel += steering.Wander(kSteeringWanderRad, kSteeringWanderDist, kSteeringWanderJitterPerSec, dt).Normalized() * kSteeringWanderWeight * dt;
			vel += steering.Seek(bounds.Center()).Normalized() * kSteeringSeekWeightIdle * dt;
			if (state == State::IDLE && player->pos.Distance(pos) < kStartChasingRadius) {
				state = State::ENTER_CHASE;
				timer = 0;
			}
		}
		break;
		case State::CHASING:
		case State::ENTER_CHASE:
		case State::EXIT_CHASE:
		{
			vec seekForce = steering.Seek(player->pos - vec(0, kTargetVerticalDistance)).Truncated(kSteeringSeekWeightChasing * dt);
			vel += seekForce;
			if (state == State::CHASING) {
				if (player->pos.Distance(pos) > kStopChasingRadius) {
					state = State::EXIT_CHASE;
					timer = 0;
				}
				else {
					if (timer <= 0) {
						if (abs(pos.x - player->pos.x) < kHorizontalDistanceToShoot && pos.y < player->pos.y - kVerticalDistanceToShoot) {
							vec vel = (player->pos - pos).Normalized() * kTearVel;
							vel += Rand::VecInRange(-vec(kRandomTearVel, kRandomTearVel), vec(kRandomTearVel, kRandomTearVel));
							new OoyTear(pos, vel);
							timer = kIntervalShoot;
						}
					}
					else {
						timer -= dt;
					}
				}
			}
		}
		break;
	}

	vel.DebugDrawAsArrow(pos);
	vel.Clamp(vec(-kMaxSpeed, -kMaxSpeed * 0.7f), vec(kMaxSpeed, kMaxSpeed * 0.7f));
	pos += vel * dt;


	if (ReceiveDamageFromPlayer(Bounds(), false)) {
		DieWithSmallExplosion(this);
		return;
	}

	DamagePlayerOnCollision(Bounds());
}

void MiniOoy::Draw() const
{
	GPU_Rect rect;
	switch (state) {
	case State::IDLE:
		rect = AnimLib::OOY_IDLE;
		CircleBounds(pos, kStartChasingRadius).DebugDraw(255, 0, 255);
		break;
	case State::ENTER_CHASE:
		if (timer > kTimeToChangeState / 2) {
			rect = AnimLib::OOY_CHANGE_STATE_2;
		}
		else {
			rect = AnimLib::OOY_CHANGE_STATE_1;
		}
		CircleBounds(pos, kStartChasingRadius).DebugDraw(255, 255, 255);
		break;
	case State::EXIT_CHASE:
	{
		if (((timer / kTimeToChangeState)) > 0.75f) {
			rect = AnimLib::OOY_CHANGE_STATE_1;
		}
		else {
			int offset = 0;
			if (JumpMan::instance()->pos.y < pos.y) {
				offset = 2;
			}
			rect = directions[offset + (int)((timer / kTimeToChangeState) * 4) % 2];
		}
		CircleBounds(pos, kStartChasingRadius).DebugDraw(255, 255, 255);
		break;
	}
	case State::CHASING:
		float degs = JumpMan::instance()->pos.AngleDegs(pos) + 180;
		rect = directions[(int)(degs / 90)];
		CircleBounds(pos, kStopChasingRadius).DebugDraw(255, 0, 255);
		break;
	}
	/*
		{
			ImGui::Begin("ooy");
			ImGui::SliderFloat("WanderRad", &WanderRad, 0, 150);
			ImGui::SliderFloat("WanderDist", &WanderDist, 0, 150);
			ImGui::SliderFloat("WanderJitterPerSec", &WanderJitterPerSec, 0, 300);
			ImGui::End();
		}
	*/

	Window::Draw(Assets::spritesheetTexture, pos)
		.withRectWithOriginCentered(rect)
		.withScale(vec(kScale) + vec(0.05f + 0.05f * sin(mainClock), 0.f));

	// Debug-only
	Bounds().DebugDraw();
}
