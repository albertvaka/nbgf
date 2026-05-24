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
	// Visual-only spring-damped offset added to head and shoulders at draw time.
	// Pulsed by foot-plant impulses so the body settles toward each newly-loaded
	// foot. Does not affect step logic — leg comfort still measures against the
	// un-wobbled headPos.
	vec headWobble;
	vec headWobbleVel;
	Leg legs[NumLegs];

	HexapodBoss(vec pos);

	void Update(float dt);
	void Draw() const;

private:
	std::optional<vec> FindFootTarget(int legIndex) const;
	bool TryStartStep(int legIndex);
	static vec ComputeKnee(vec shoulder, vec foot, float upperLen, float lowerLen, int bendSign);
	// Picks the bend sign so the knee sits on the +Y (downward) side of the
	// shoulder→foot line — i.e., the knee never points upward against gravity.
	// Used by a hanging hexapod where feet anchor above the body.
	static int DownwardBendSign(vec shoulder, vec foot);
};
