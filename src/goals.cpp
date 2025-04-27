#include "goals.h"

#include "ship.h"
#include "rock.h"
#include "input.h"
#include "scene_manager.h"
#include "collide.h"
#include "window_draw.h"

const float kGoalRadius = 90.f;
const float kMinDistanceToNewGoal = 1000.f;
const float kMaxDistanceToNewGoal = 1800.f;

const float kCountdownGoalIncrease = 7.f;
const float kCountdownInitialTime = 20.f;

const float kTimeUntilRestartEnabled = 2.f;

// Particles
const float kRotationSpeed = 50.f;
const float kSegmentLength = 15.f;
const SDL_Color kActiveColor = { 255,238,140, 255 };
const SDL_Color kInactiveColor = { 238,238,238, 128};

void InitPartSys(PartSys& ps) {
	ps.AddSprite({ 64,0,32,32 });

	ps.spawn_radius = 5.f;
	ps.min_interval = 0.03f;
	ps.max_interval = 0.03f;

	ps.min_ttl = 0.9f;
	ps.max_ttl = 0.9f;

	ps.alpha = 1.0f;

	float thicnkess = 0.f;
	ps.min_vel.x = -thicnkess;
	ps.max_vel.x = thicnkess;

	ps.min_vel.y = -thicnkess;
	ps.max_vel.y = thicnkess;

	float scale = 1.0f;
	ps.min_scale = scale;
	ps.max_scale = scale;
	ps.scale_vel = 0.0f;

	ps.min_rotation = 0;
	ps.max_rotation = 360;
	ps.min_rotation_vel = -180;
	ps.max_rotation_vel = 180;
}

void UpdateParticles(PartSys& ps, float dt)
{
	vec pos = ps.pos;
	float from = Angles::ClampedDegsBetween0and360(kRotationSpeed * mainClock);
	float to = from + kSegmentLength;
	int parts = 5;
	for (float i = from; i < to; i += 4.f) {
		for (int j = 0; j < parts; j++) {
			ps.pos = pos + vec::FromAngleDegs(i + j * (360 / parts)) * kGoalRadius;
			ps.Spawn(dt);
		}
	}
	ps.UpdateParticles(dt);

	ps.pos = pos;
}

vec FindNextPos(vec pos, vec direction) {
retry:
	vec newSpawnPosition = pos + direction.RotatedAroundOriginRads(Rand::rollf(-Angles::Tau / 4, Angles::Tau / 4)) * Rand::rollf(kMinDistanceToNewGoal, kMaxDistanceToNewGoal);
	CircleBounds b = CircleBounds(newSpawnPosition, kGoalRadius);
	for (Rock* rock : Rock::GetAll()) {
		if (b.Contains(rock->pos)) {
			goto retry;
		}
	}
	return newSpawnPosition;
}

Goals::Goals()
	: CircleEntity(pos, kGoalRadius) 
	, activeGoal(Assets::particlesTexture)
	, inactiveGoal(Assets::particlesTexture)
	, countdownText(Assets::font_large, Assets::font_large_outline)
	, restartText(Assets::font_small, Assets::font_small_outline)
	, gameOverRotoText(Assets::font_large, Assets::font_large_outline)
{
	InitPartSys(activeGoal);
	InitPartSys(inactiveGoal);
	activeGoal.color = kActiveColor;
	inactiveGoal.color = kInactiveColor;
	restartText.SetString("Press start to try again");
}

void Goals::Reset() {
	Ship* ship = Ship::instance();
	activeGoal.color = kActiveColor;
	activeGoal.pos = FindNextPos(ship->pos, ship->heading);
	inactiveGoal.pos = FindNextPos(activeGoal.pos, ship->heading);
	movementEnabled = true;
	state = State::NOT_ACTIVE;
}

void DrawScreenEdgeArrow(BoxBounds& cameraBounds, vec pos, SDL_Color color) {
	cameraBounds *= 0.8f;
	vec edge = cameraBounds.ClosestPointInBounds(pos);
	Window::Draw(Assets::arrowTexture, edge)
		.withOrigin(Assets::arrowTexture->w / 2, Assets::arrowTexture->h / 2)
		.withScale(0.33f)
		.withColor(color)
		.withRotationRads(edge.AngleRads(cameraBounds.Center()));
}

void Goals::GameOver() {
	movementEnabled = false;
	gameOverRotoText.ShowMessage("Game Over");
	restartTimer = kTimeUntilRestartEnabled;
	activeGoal.color = kInactiveColor;
	state = State::GAME_OVER;
}

void Goals::GotGoal() {
	for (auto& p : activeGoal.particles) {
		p.alpha_vel = -10.f;
	}
	activeGoal.pos = inactiveGoal.pos;
	Ship* ship = Ship::instance();
	inactiveGoal.pos = FindNextPos(activeGoal.pos, (activeGoal.pos - ship->pos).Normalized());
	switch (state) {
	case State::NOT_ACTIVE:
		countdown = kCountdownInitialTime;
		state = State::ACTIVE;
		break;
	case State::ACTIVE:
		countdown += kCountdownGoalIncrease;
		countdown = (int)countdown + 1; // round up
		break;
	case State::GAME_OVER:
		SDL_assert(false);
		break;
	}
}

bool Goals::Update(float dt)
{
	bool retWantsRestart = false;

	Ship* ship = Ship::instance();
	CircleBounds shipBounds = ship->ApproxBounds();

	UpdateParticles(activeGoal, dt);
	UpdateParticles(inactiveGoal, dt);

	if (state != State::GAME_OVER) {
		CircleBounds activeBounds(activeGoal.pos, kGoalRadius);
		if (Collide(shipBounds, activeBounds)) {
			GotGoal();
		}
		if (state == State::ACTIVE) {
			countdown -= dt;
			if (countdown <= 0.f) {
				countdown = 0.f;
				GameOver();
			}
			countdownText.SetString(Mates::ToStringWithPrecision(countdown, 1));
		}
	} else { // game over
		gameOverRotoText.Update(dt);
		if (restartTimer > 0.f) {
			restartTimer -= dt;
		} else {
			if (Input::IsJustPressedAnyPlayer(GameKeys::START)) {
				retWantsRestart = true;
			}
		}
	}

	return retWantsRestart;
}

void Goals::Draw() const
{
	activeGoal.Draw();
	if (state != State::GAME_OVER) {
		inactiveGoal.Draw();

		BoxBounds cameraBounds = Camera::Bounds();
		if (!cameraBounds.Contains(activeGoal.pos)) {
			DrawScreenEdgeArrow(cameraBounds, activeGoal.pos, kActiveColor);
		}
		else if (!cameraBounds.Contains(inactiveGoal.pos)) {
			DrawScreenEdgeArrow(cameraBounds, inactiveGoal.pos, kInactiveColor);
		}
	}
}

void Goals::DrawGui() const {
	Camera::InScreenCoords::Begin();
	if (state == State::ACTIVE) {
		Window::Draw(countdownText, vec(30, 50))
			.withOrigin(0.f, countdownText.Size().y/2)
			.withScale(0.5f);
	} else if (state == State::GAME_OVER) {
		gameOverRotoText.Draw(Camera::InScreenCoords::Center() - vec(0, 100), 0.7f);
		if (restartTimer <= 0.f) {
			bool blink = static_cast<int>(std::floor(mainClock / 0.5f)) % 2 == 0;
			if (blink) {
				Window::Draw(restartText, Camera::InScreenCoords::Center() + vec(0, 100))
					.withOrigin(restartText.Size() / 2)
					.withScale(0.5f);
			}
		}
	}
	Camera::InScreenCoords::End();
}