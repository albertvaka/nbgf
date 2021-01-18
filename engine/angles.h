#pragma once

#include "mates.h"

namespace Angles
{
	const constexpr float   Tau = 6.28318530718f;
	const constexpr float   Pi  = 3.14159265359f;

	[[nodiscard]] inline constexpr float DegsToRads(float degs)
	{
		return Tau * (degs / 360.0f);
	}

	[[nodiscard]] inline constexpr float RadsToDegs(float rads)
	{
		return (rads * 360.0f) / Tau;
	}

	inline constexpr void KeepDegsBetween0and360(float& degs)
	{
		if (degs > 360.f) {
			degs = fmod(degs, 360.f);
		} else {
			degs = 360.f - fmod(-degs, 360.f);
		}
	}

	inline constexpr void KeepRadsBetween0andTau(float& rads)
	{
		if (rads > Tau) {
			rads = fmod(rads, Tau);
		} else {
			rads = Tau - fmod(-rads, Tau);
		}
	}
}
