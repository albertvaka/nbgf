#pragma once

#include "mates.h"

namespace Angles
{
	constexpr float   Tau = 6.28318530718f;
	constexpr float   Pi  = 3.14159265359f;

	[[nodiscard]] inline constexpr float DegsToRads(float degs)
	{
		return Tau * (degs / 360.0f);
	}

	[[nodiscard]] inline constexpr float RadsToDegs(float rads)
	{
		return (rads * 360.0f) / Tau;
	}

	inline constexpr void KeepDegsBetween0and360(float& degs) // [0, 360)
	{
		if (degs >= 360.f) {
			degs = fmod(degs, 360.f);
		} else if (degs < 0.f) {
			degs = 360.f - fmod(-degs, 360.f);
		}
	}

	inline constexpr void KeepRadsBetween0andTau(float& rads) // [0, Tau)
	{
		if (rads >= Tau) {
			rads = fmod(rads, Tau);
		} else if (rads < 0.f) {
			rads = Tau - fmod(-rads, Tau);
		}
	}
}
