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

#include "debug.h"

void test(int line, float computed, float expected) {
	float diff = fabs(expected-computed);
	if (diff > 0.001) {
		Debug::out << "Line " << line << ": Expected: " << expected << ", computed: " << computed;
	}
}

void TestAngles() {
	vec unitX = vec(1, 0);
	vec unitY = vec(0, 1);
	vec unitNegX = vec(-1, 0);
	vec unitNegY = vec(0, -1);
	vec a45 = vec(1, 1).Normalized();
	vec aNeg45 = vec(1, -1).Normalized();
	vec a45_not_normalized = vec(10, 10);
	vec aNeg45_not_normalized = vec(10, -10);

	test(__LINE__, vec::Zero.AngleDegs(), 0);
	test(__LINE__, AngleDegsBetween(unitX, unitY), 90);
	test(__LINE__, AngleDegsBetween(unitX, unitNegY), -90);
	test(__LINE__, unitNegY.AngleDegs(), -90);
	test(__LINE__, unitY.AngleDegs(), 90);
	test(__LINE__, unitX.AngleDegs(), 0);
	test(__LINE__, AngleDegsBetween(a45, a45), 0);
	test(__LINE__, AngleDegsBetween(unitX, a45), 45);
	test(__LINE__, AngleDegsBetween(a45, unitX), -45);
	test(__LINE__, a45.AngleDegs(), 45);
	test(__LINE__, AngleDegsBetween(aNeg45, unitX), 45);
	test(__LINE__, aNeg45.AngleDegs(), -45);
	test(__LINE__, AngleDegsBetween(aNeg45, a45), 90);
	test(__LINE__, AngleDegsBetween(unitX, unitNegX), 180);
	test(__LINE__, AngleDegsBetween(a45, unitNegX), 135);
	test(__LINE__, AngleDegsBetween(unitNegX, a45), -135);
	test(__LINE__, unitNegX.AngleDegs(), 180);
	test(__LINE__, AngleDegsBetween(aNeg45, unitNegX), -135);
	test(__LINE__, aNeg45.AngleDegs(), -45);
	test(__LINE__, AngleDegsBetween(unitY, unitNegX), 90);
	test(__LINE__, unitY.AngleDegs(), 90);
	test(__LINE__, AngleDegsBetween(unitY, aNeg45), -135);
	test(__LINE__, AngleDegsBetween(a45_not_normalized, aNeg45_not_normalized), -90);
	test(__LINE__, AngleDegsBetween(aNeg45_not_normalized, a45_not_normalized), 90);
}
