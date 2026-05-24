#pragma once

#include <optional>

#include "vec.h"
#include "selfregister.h"

struct HexapodBoss : SelfRegister<HexapodBoss>
{
	static constexpr int NumLegs = 6;
	static constexpr int MaxConcurrentSteps = 4;

	struct Leg {
		vec shoulderOffset;
		vec footRestOffset;
		vec stepStart;
		vec stepTarget;
		vec footPos;
		float stepProgress;
		float stepDuration;
		float stepArcHeight;
		float stepCooldown; // seconds since this leg's last step ended
		float upperLen;
		float lowerLen;
		int bendSign;       // +1 / −1: which side of the shoulder→foot line the knee bends to. Latched at step start.
		bool isPlanted;     // briefly false only between losing a foothold and starting the next step
	};

	vec headPos;
	vec headVel;
	vec moveTarget;
	Leg legs[NumLegs];

	HexapodBoss(vec pos);

	void Update(float dt);
	void Draw() const;

private:
	std::optional<vec> FindFootTarget(int legIndex) const;
	bool TryStartStep(int legIndex);
	static vec ComputeKnee(vec shoulder, vec foot, float upperLen, float lowerLen, int bendSign);
	static int NaturalBendSign(vec shoulder, vec foot, vec head);
};
