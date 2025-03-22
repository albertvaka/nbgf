#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "input.h"
#include "steering_behavior.h"
#include "stroke.h"

const float deceleration_coef = 0.3;
const float max_speed = 220.f;
const float timeBetweenStrokeSegments = 0.05f;

extern float mainClock;

class Ship : public Entity {
public:

    Ship() {
		SDL_Color foamColor = { Uint8(0.7 * 255), Uint8(0.8 * 255), Uint8(0.9 * 255), 255 };
		SDL_Color surfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 255 };
		SDL_Color alphaSurfaceColor = { Uint8(0.1 * 255), Uint8(0.45 * 255), Uint8(0.73 * 255), 0 };

		outerStroke.SetMaxJoints(100);
		outerStroke.SetStartInnerColor(foamColor);
		outerStroke.SetStartOuterColor(foamColor);
		outerStroke.SetEndInnerColor(alphaSurfaceColor);
		outerStroke.SetEndOuterColor(alphaSurfaceColor);
		outerStroke.SetStartThickness(0.1f);
		outerStroke.SetColorOffset(25.f);

		innerStroke.SetMaxJoints(100);
		innerStroke.SetStartInnerColor(surfaceColor);
		innerStroke.SetStartOuterColor(surfaceColor);
		innerStroke.SetEndInnerColor(alphaSurfaceColor);
		innerStroke.SetEndOuterColor(alphaSurfaceColor);
		innerStroke.SetStartThickness(0.1f);
		innerStroke.SetColorOffset(25.0f);

	}

	void Reset() {
		pos = vec::Zero;
		vel = vec::Zero;
		heading = vec(1,0);
		timer = 0.f;
		distanceSailed = 0.f;
		innerStroke.Clear();
		outerStroke.Clear();
	}

	void Update(float dt) {
		float speed = vel.Length();

		// The correct way to add friction
		float frictionImpulse = speed * deceleration_coef;
		speed -= frictionImpulse * dt;

        if (Input::IsPressed(0, GameKeys::UP)) {
			speed += 90*dt;
		}
		if (Input::IsPressed(0, GameKeys::DOWN)) {
			speed -= 90*dt;
		}
		if (Input::IsPressed(0, GameKeys::LEFT)) {
			heading = heading.RotatedAroundOriginRads(-0.0035*dt*(speed+25));
			heading.Normalize();
		}
		if (Input::IsPressed(0, GameKeys::RIGHT)) {
			heading = heading.RotatedAroundOriginRads(0.0035*dt*(speed+25));
			heading.Normalize();
		}

		vel = speed*heading;
		vel.Truncate(max_speed);
		pos += vel * dt;

		distanceSailed += vel.Length();

		innerStroke.Calculate(dt);
		outerStroke.Calculate(dt);

		timer += dt;
		if (timer > timeBetweenStrokeSegments) {
			timer -= timeBetweenStrokeSegments;
			innerStroke.AddJoint(pos, std::max(speed / max_speed, 0.3f) * 3.5f * 10.f + sin(distanceSailed * 0.008f) * 2.f);
			outerStroke.AddJoint(pos, std::max(speed / max_speed, 0.3f) * 3.5f * 14.f + sin(distanceSailed * 0.008f) * 2.f);
		}

	}

	void Draw() {
		float angle = AngleDegs(heading);
		Window::Draw(Assets::shipTexture, pos)
			.withOrigin(Assets::shipTexture->w / 2, Assets::shipTexture->h / 2)
			.withRotationDegs(angle)
			.withScale(0.5f);
	}
	
	void DrawStroke() {
		innerStroke.Draw();
		outerStroke.DrawExcluding(innerStroke);
	}

private:
	float timer;
	vec heading;
	Stroke innerStroke;
	Stroke outerStroke;
	float distanceSailed;
};
