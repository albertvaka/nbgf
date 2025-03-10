#pragma once

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "input.h"
#include "steering_behavior.h"
//#include "Stroke.hpp"

const float deceleration_coef = 0.3;
const float max_speed = 220.f;

class Ship : public Entity {
public:

    Ship() {
		/*stroke = new Stroke();
		stroke->SetMaxJoints(20);
		stroke->SetInnerColor(sf::Color(25,255,255,0));
		stroke->SetEndThickness(0.0f);
		timer = 0;
		*/
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

		Debug::out << speed;

		vel = speed*heading;
		vel.Truncate(max_speed);
		pos += vel * dt;

		//stroke->Calculate(dt);

		timer += dt;
		if (timer > 0.15) {
			timer -= 0.15;
			//stroke->AddJoint(pos ,9.f);
		}

	}

	void Draw() {
		float angle = vec(0, 0).AngleDegs(heading);
		Window::Draw(Assets::shipTexture, pos)
			.withOrigin(Assets::shipTexture->w / 2, Assets::shipTexture->h / 2)
			.withRotationDegs(angle)
			.withScale(0.5f);
	}
	
	void DrawStroke() {
		//rt.draw(*stroke);
	}

private:
	//Stroke* stroke;
	float timer = 0;
	vec heading = vec(1,0);
};
