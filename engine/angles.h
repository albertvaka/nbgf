#pragma once

namespace Angles
{
	const constexpr float   Pi = 3.141592653589793238462643383279502884f;
	const constexpr float   TwoPi = Pi * 2;
	const constexpr float   HalfPi = Pi / 2;
	const constexpr float   QuarterPi = Pi / 4;
	
	const constexpr float FullTurnInRads = TwoPi;  // Yes, I know you know this by heart.
	const constexpr float QuarterTurnInRads = HalfPi; // I just don't. But more importantly...
	const constexpr float HalfTurnInRads = Pi; // I don't want to know. Sorry to disapoint.

	[[nodiscard]] inline constexpr float DegsToRads(float degs)
	{
		return TwoPi * (degs / 360.0f);
	}

	[[nodiscard]] inline constexpr float RadsToDegs(float rads)
	{
		return (rads * 360.0f) / TwoPi;
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
