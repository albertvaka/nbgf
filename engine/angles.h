#pragma once

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

	inline constexpr void KeepDegreesBetween0and360(float& degs)
	{
		while (degs >= 360) {
			degs -= 360;
		}
		while (degs < 0) {
			degs += 360;
		}
	}
}
