#include "vec.h"

vec vec::RotatedToFacePositionRads(vec target, float maxTurnRateRads) const
{
	vec toTarget = (target - *this).Normalized();
	vec heading = Normalized();

	//first determine the angle between the heading vector and the target
	float angle = acos(heading.Dot(toTarget));

	//return true if already facing the target
	if (angle < 0.00001f || isnan(angle)) {
		return *this;
	}

	//clamp the amount to turn to the max turn rate
	if (fabs(angle) > maxTurnRateRads) angle = maxTurnRateRads;

	return RotatedAroundOriginRads(angle * heading.Sign(toTarget));
}
