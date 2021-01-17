#include "vec.h"

const vec vec::Zero(0.f, 0.f);

vec vec::RotatedToFacePositionRads(vec target, float maxTurnRateRads) const
{
	vec toTarget = (target - *this).Normalized();
	vec heading = Normalized();

	//first determine the angle between the heading vector and the target
	float angle = acos(heading.Dot(toTarget));

	//return true if already facing the target
	if (angle < 0.00001 || isnan(angle)) {
		return *this;
	}

	//clamp the amount to turn to the max turn rate
	if (fabs(angle) > maxTurnRateRads) angle = maxTurnRateRads;

	return RotatedAroundOriginRads(angle * heading.Sign(toTarget));
}
