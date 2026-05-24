#include "hexapod_boss.h"

#include <math.h>
#include <algorithm>
#include <limits>

#include "gaemtilemap.h"
#include "tile.h"
#include "window_drawprimitive.h"
#include "mates.h"
#include "rand.h"
#include "player.h"
#include "debug.h"
#include "camera.h"

// --- Geometry. Head stays small; legs are very long so the boss spans a large
// part of the screen and can reach platforms across a wide sweep.
constexpr float kHeadRadius = 14.f;
constexpr float kFootRadius = 3.f;
constexpr float kLegThickness = 2.f;
constexpr float kUpperLen = 90.f;
constexpr float kLowerLen = 130.f;             // total reach = 220
constexpr float kMaxReachFactor = 0.95f;       // ~209
constexpr float kMinReachFactor = 0.18f;       // ~40  — much wider range than before

// A planted leg requests a step early — before it hits the constraint — so the
// head is never held in place by a leg waiting for discomfort to build up.
constexpr float kStretchLiftoffFactor = 0.88f;
constexpr float kCompressionLiftoffFactor = 1.15f;

// Feet should stay spread out — a candidate tile is rejected if any other foot
// (current or planned) is within this distance.
constexpr float kMinFootSeparation = 32.f;
constexpr float kMinFootSeparationSq = kMinFootSeparation * kMinFootSeparation;

// --- Head motion: the head moves at a constant speed toward its target. Legs
// do NOT pull the head — they just accommodate, re-stepping when they run
// out of reach.
constexpr float kHeadSpeed = 60.f;             // px/s — deliberate, threatening pace

// --- Stepping
constexpr float kStepSpeed = 300.f;            // px/s — constant foot speed regardless of step length
constexpr float kMinStepDuration = 0.12f;      // sanity cap so tiny steps don't snap instantly
constexpr float kMaxStepDuration = 0.6f;       // sanity cap so very long steps don't drag forever
constexpr float kBaseStepArcHeight = 18.f;
constexpr float kStepDiscomfortThreshold = 12.f;
constexpr float kStepMinImprovement = 4.f;
constexpr int kFootSearchRange = 12;           // ~192px sweep per leg — easily across the screen
constexpr float kStepLookaheadTime = 0.15f;
constexpr float kStepCooldown = 0.2f;          // minimum dwell after a step before a comfort-driven re-step
                                                // (urgent stretch/compression triggers bypass this)

// --- Layout
constexpr vec kShoulderOffsets[HexapodBoss::NumLegs] = {
	vec(-12.f, -7.f),
	vec(-13.f,  5.f),
	vec( -6.f, 12.f),
	vec(  6.f, 12.f),
	vec( 13.f,  5.f),
	vec( 12.f, -7.f),
};

// Wide rest offsets — adjacent rest positions are >= kMinFootSeparation apart so
// the default stance is naturally spread.
constexpr vec kFootRestOffsets[HexapodBoss::NumLegs] = {
	vec(-100.f,  75.f),
	vec( -65.f, 110.f),
	vec( -25.f, 125.f),
	vec(  25.f, 125.f),
	vec(  65.f, 110.f),
	vec( 100.f,  75.f),
};

// ----------------------------------------------------------------------------
// Foot stepping
// ----------------------------------------------------------------------------

std::optional<vec> HexapodBoss::FindFootTarget(int legIndex) const
{
	const Leg& leg = legs[legIndex];
	vec shoulder = headPos + leg.shoulderOffset;
	vec rest = headPos + leg.footRestOffset + headVel * kStepLookaheadTime;

	GaemTileMap* map = GaemTileMap::instance();
	if (!map) return std::nullopt;

	int restTx = Tile::ToTiles(rest.x);
	int headTy = Tile::ToTiles(headPos.y);
	// Reach is ~13 tiles; sweep symmetrically up and down so ceilings and
	// floors are equally findable.
	constexpr int kSearchHalfHeight = 14;
	int startTy = std::max(0, headTy - kSearchHalfHeight);
	int endTy = std::min(map->Height(), headTy + kSearchHalfHeight);

	float maxReach = (leg.upperLen + leg.lowerLen) * kMaxReachFactor;
	float minReach = (leg.upperLen + leg.lowerLen) * kMinReachFactor;
	float maxReachSq = maxReach * maxReach;
	float minReachSq = minReach * minReach;

	float bestDistSq = std::numeric_limits<float>::max();
	bool foundAny = false;
	vec best(0.f, 0.f);

	for (int dx = -kFootSearchRange; dx <= kFootSearchRange; dx++) {
		int tx = restTx + dx;
		if (tx < 0 || tx >= map->Width()) continue;
		for (int ty = startTy; ty < endTy; ty++) {
			if (!map->GetTile(tx, ty).isSolid()) continue;

			// Every solid tile offers up to two footholds: stand on top (if
			// there's air above) or hang from underneath (if there's air
			// below). With both available the boss can always plant a foot
			// somewhere — no "tucked" / dangling state.
			bool hasAirAbove = (ty == 0) || !map->GetTile(tx, ty - 1).isSolid();
			bool hasAirBelow = (ty + 1 >= map->Height()) || !map->GetTile(tx, ty + 1).isSolid();

			for (int side = 0; side < 2; side++) {
				if (side == 0 && !hasAirAbove) continue;
				if (side == 1 && !hasAirBelow) continue;
				float yOffset = (side == 0) ? 0.f : float(Tile::Size);
				vec candidate(Tile::Left(tx) + Tile::Size * 0.5f, Tile::Top(ty) + yOffset);

				if (candidate.DistanceSq(leg.footPos) < 1.f) continue; // already here
				float dsq = candidate.DistanceSq(shoulder);
				if (dsq < minReachSq || dsq > maxReachSq) continue;     // out of leg range

				// Reject candidates too close to another foot (keeps the
				// stance spread), or that would cross another leg (legs are
				// indexed 0..5 left to right and must stay in that order).
				bool rejected = false;
				for (int j = 0; j < NumLegs; j++) {
					if (j == legIndex) continue;
					vec otherFoot = (legs[j].stepProgress < 1.f) ? legs[j].stepTarget : legs[j].footPos;
					if (candidate.DistanceSq(otherFoot) < kMinFootSeparationSq) {
						rejected = true;
						break;
					}
					if (j < legIndex && candidate.x <= otherFoot.x) { rejected = true; break; }
					if (j > legIndex && candidate.x >= otherFoot.x) { rejected = true; break; }
				}
				if (rejected) continue;

				float d = candidate.DistanceSq(rest);
				// Anchor preference: a tile above the head is naturally hung
				// from (use its bottom), a tile below the head is naturally
				// stood on (use its top). Penalise the "wrong" anchor so it's
				// only picked when the natural one is out of reach. The 4×
				// factor on squared distance ≈ 2× linear, so the natural
				// anchor wins as long as it's within twice the rest-distance
				// of the unnatural one.
				bool unnatural = (ty < headTy && side == 0) || (ty > headTy && side == 1);
				if (unnatural) d *= 4.f;
				if (d < bestDistSq) {
					bestDistSq = d;
					best = candidate;
					foundAny = true;
				}
			}
		}
	}

	// The map's upper edge is a hangable surface too — the boss can dangle
	// from the world roof even where no tile is present. One virtual candidate
	// per column, located at the top of the map bounds.
	BoxBounds mapBounds = map->BoundsInWorld();
	float ceilingY = mapBounds.Top();
	for (int dx = -kFootSearchRange; dx <= kFootSearchRange; dx++) {
		int tx = restTx + dx;
		if (tx < 0 || tx >= map->Width()) continue;
		vec candidate(Tile::Left(tx) + Tile::Size * 0.5f, ceilingY);
		if (candidate.DistanceSq(leg.footPos) < 1.f) continue;
		float dsq = candidate.DistanceSq(shoulder);
		if (dsq < minReachSq || dsq > maxReachSq) continue;

		bool rejected = false;
		for (int j = 0; j < NumLegs; j++) {
			if (j == legIndex) continue;
			vec otherFoot = (legs[j].stepProgress < 1.f) ? legs[j].stepTarget : legs[j].footPos;
			if (candidate.DistanceSq(otherFoot) < kMinFootSeparationSq) { rejected = true; break; }
			if (j < legIndex && candidate.x <= otherFoot.x) { rejected = true; break; }
			if (j > legIndex && candidate.x >= otherFoot.x) { rejected = true; break; }
		}
		if (rejected) continue;

		// The world ceiling sits above any possible head position, so it's
		// always the natural ceiling anchor — no unnatural penalty needed.
		float d = candidate.DistanceSq(rest);
		if (d < bestDistSq) {
			bestDistSq = d;
			best = candidate;
			foundAny = true;
		}
	}

	if (foundAny) return best;
	return std::nullopt;
}

bool HexapodBoss::TryStartStep(int legIndex)
{
	Leg& leg = legs[legIndex];
	auto tile = FindFootTarget(legIndex);
	if (!tile) return false;       // no reachable foothold this frame — try again next

	if (leg.isPlanted) {
		// Only step if the candidate is materially better, OR if the leg is
		// already at the edge of its reach and *must* lift.
		vec rest = headPos + leg.footRestOffset;
		vec shoulder = headPos + leg.shoulderOffset;
		float currentDist = leg.footPos.Distance(rest);
		float newDist = tile->Distance(rest);
		bool improvement = (newDist <= currentDist - kStepMinImprovement);
		if (!improvement) {
			float D = leg.footPos.Distance(shoulder);
			float maxReach = (leg.upperLen + leg.lowerLen) * kMaxReachFactor;
			float minReach = (leg.upperLen + leg.lowerLen) * kMinReachFactor;
			bool atLimit = (D > maxReach * kStretchLiftoffFactor || D < minReach * kCompressionLiftoffFactor);
			if (!atLimit) return false;
		}
	}

	leg.stepStart = leg.footPos;
	leg.stepTarget = *tile;
	leg.stepProgress = 0.f;
	// Latch the bend direction for this step based on the target so the elbow
	// doesn't flip while the foot is sweeping through the arc. (It can still
	// update from one step to the next, which is what lets ceiling vs floor
	// inversion still work.)
	{
		vec stepShoulder = headPos + leg.shoulderOffset;
		leg.bendSign = NaturalBendSign(stepShoulder, leg.stepTarget, headPos);
	}
	// Constant-speed steps: duration scales with distance so a short hop and a
	// long stretch move the foot at the same px/s. ±8% jitter for organic variety.
	float stepDist = leg.footPos.Distance(*tile);
	float duration = std::clamp(stepDist / kStepSpeed, kMinStepDuration, kMaxStepDuration);
	leg.stepDuration = duration * Rand::rollf(0.92f, 1.08f);
	leg.stepArcHeight = kBaseStepArcHeight + Rand::rollf(-4.f, 6.f);
	return true;
}

// ----------------------------------------------------------------------------
// Lifecycle / update / draw
// ----------------------------------------------------------------------------

HexapodBoss::HexapodBoss(vec pos)
	: headPos(pos)
	, headVel(0.f, 0.f)
	, moveTarget(pos)
{
	// If we happen to spawn inside a solid tile, walk outward to find air. The
	// boss has no map of its own anymore — it just queries the tilemap.
	GaemTileMap* map = GaemTileMap::instance();
	if (map) {
		veci spawnTile = Tile::ToTiles(headPos);
		if (map->InBounds(spawnTile.x, spawnTile.y) && map->GetTile(spawnTile.x, spawnTile.y).isSolid()) {
			for (int r = 1; r < 50; r++) {
				bool found = false;
				for (int dy = -r; dy <= r && !found; dy++) {
					for (int dx = -r; dx <= r && !found; dx++) {
						if (std::abs(dx) != r && std::abs(dy) != r) continue;
						int tx = spawnTile.x + dx;
						int ty = spawnTile.y + dy;
						if (map->InBounds(tx, ty) && !map->GetTile(tx, ty).isSolid()) {
							headPos = vec(Tile::Left(tx) + Tile::Size * 0.5f, Tile::Top(ty) + Tile::Size * 0.5f);
							found = true;
						}
					}
				}
				if (found) break;
			}
		}
	}

	for (int i = 0; i < NumLegs; i++) {
		legs[i].shoulderOffset = kShoulderOffsets[i];
		legs[i].footRestOffset = kFootRestOffsets[i];
		legs[i].upperLen = kUpperLen;
		legs[i].lowerLen = kLowerLen;
		legs[i].stepProgress = 1.f;
		legs[i].stepDuration = kMinStepDuration; // placeholder; actual durations are computed per-step
		legs[i].stepArcHeight = kBaseStepArcHeight;
		legs[i].stepCooldown = kStepCooldown;    // start "rested" so initial steps aren't blocked

		auto initialTile = FindFootTarget(i);
		if (initialTile) {
			legs[i].footPos = *initialTile;
			legs[i].isPlanted = true;
		} else {
			// No reachable tile right now — sit at natural rest and let the
			// step trigger find one once the boss moves into terrain.
			legs[i].footPos = headPos + legs[i].footRestOffset;
			legs[i].isPlanted = false;
		}
		legs[i].stepStart = legs[i].footPos;
		legs[i].stepTarget = legs[i].footPos;
		vec spawnShoulder = headPos + legs[i].shoulderOffset;
		legs[i].bendSign = NaturalBendSign(spawnShoulder, legs[i].footPos, headPos);
	}
	moveTarget = headPos;

	Debug::out << "HexapodBoss spawned at (" << headPos.x << "," << headPos.y << ")";
}

void HexapodBoss::Update(float dt)
{
	// --- Target = the player's position. The head tries to get there directly.
	Player* p = Player::instance();
	vec playerPos = p ? p->CenterPos() : headPos;
	moveTarget = playerPos;

	// Keep moveTarget inside the map so the head isn't asked to leave the level.
	GaemTileMap* map = GaemTileMap::instance();
	if (map) {
		BoxBounds bounds = map->BoundsInWorld();
		moveTarget.x = std::clamp(moveTarget.x, bounds.Left() + kHeadRadius, bounds.Right() - kHeadRadius);
		moveTarget.y = std::clamp(moveTarget.y, bounds.Top()  + kHeadRadius, bounds.Bottom() - kHeadRadius);
	}

	// --- Head motion: constant-speed kinematic. The head sets the pace; legs
	// just accommodate. Legs do not apply force to the head.
	vec toTarget = moveTarget - headPos;
	float dist = toTarget.Length();
	float step = kHeadSpeed * dt;
	if (dist <= step || dist < 0.0001f) {
		headPos = moveTarget;
		headVel = vec(0.f, 0.f);
	} else {
		vec dir = toTarget / dist;
		headPos += dir * step;
		headVel = dir * kHeadSpeed;
	}

	// Hard clamp: head never leaves the level bounds (top/bottom/left/right).
	if (map) {
		BoxBounds bounds = map->BoundsInWorld();
		headPos.x = std::clamp(headPos.x, bounds.Left() + kHeadRadius, bounds.Right() - kHeadRadius);
		headPos.y = std::clamp(headPos.y, bounds.Top()  + kHeadRadius, bounds.Bottom() - kHeadRadius);
	}

	// --- Tile-validity check: a foothold can be on top of a tile (floor),
	// underneath one (ceiling), or hung from the map's upper edge (world
	// ceiling). If none of those is the case, the leg loses footing and
	// immediately re-steps.
	if (map) {
		float worldCeilingY = map->BoundsInWorld().Top();
		for (int i = 0; i < NumLegs; i++) {
			Leg& leg = legs[i];
			if (!leg.isPlanted) continue;
			if (leg.stepProgress < 1.f) continue;
			int footTx = Tile::ToTiles(leg.footPos.x);
			int floorTy = Tile::ToTiles(leg.footPos.y + 1.f); // tile just below the foot
			int ceilTy  = Tile::ToTiles(leg.footPos.y - 1.f); // tile just above the foot
			bool hasFloor   = map->InBounds(footTx, floorTy) && map->GetTile(footTx, floorTy).isSolid();
			bool hasCeiling = map->InBounds(footTx, ceilTy)  && map->GetTile(footTx, ceilTy).isSolid();
			bool onWorldCeiling = (leg.footPos.y <= worldCeilingY + 1.f);
			if (!hasFloor && !hasCeiling && !onWorldCeiling) {
				leg.isPlanted = false;
				leg.stepCooldown = kStepCooldown;
				TryStartStep(i);
			}
		}
	}

	// --- Advance stepping legs; hidden legs keep their foot at the shoulder
	// (they don't draw, but this gives a sensible start point for the next unfold step).
	float footMinX = 0.f, footMaxX = 0.f, footMinY = 0.f, footMaxY = 0.f;
	bool clampFeet = (map != nullptr);
	if (clampFeet) {
		BoxBounds bounds = map->BoundsInWorld();
		footMinX = bounds.Left();
		footMaxX = bounds.Right();
		footMinY = bounds.Top();
		footMaxY = bounds.Bottom();
	}

	int activeSteps = 0;
	for (int i = 0; i < NumLegs; i++) {
		Leg& leg = legs[i];
		leg.stepCooldown += dt;
		if (leg.stepProgress < 1.f) {
			leg.stepProgress += dt / leg.stepDuration;
			if (leg.stepProgress > 1.f) leg.stepProgress = 1.f;
			float t = leg.stepProgress;
			vec pp = Mates::Lerp(leg.stepStart, leg.stepTarget, t);
			// The arc lifts the foot *off its surface*: up when the step lies
			// below the shoulder (walking on floors), down when it lies above
			// (hanging from ceilings). Without this a ceiling step would
			// scoop the foot through the tile.
			vec shoulder = headPos + leg.shoulderOffset;
			float midY = (leg.stepStart.y + leg.stepTarget.y) * 0.5f;
			float arcSign = (midY > shoulder.y) ? -1.f : +1.f;
			pp.y += arcSign * sinf(t * float(M_PI)) * leg.stepArcHeight;
			leg.footPos = pp;
			if (leg.stepProgress >= 1.f) {
				leg.isPlanted = true;
				leg.stepCooldown = 0.f;
			} else {
				activeSteps++;
			}
		}

		// Feet must stay inside the level on all sides.
		if (clampFeet) {
			leg.footPos.x = std::clamp(leg.footPos.x, footMinX, footMaxX);
			leg.footPos.y = std::clamp(leg.footPos.y, footMinY, footMaxY);
		}
	}

	// --- Step trigger.
	// Planted leg is a candidate when discomfort is high (soft) or when it's
	// approaching its stretch/compression limit (urgent — head would get
	// pinned). Soft triggers respect kStepCooldown so a freshly-landed foot
	// commits to its tile for a bit. Urgent triggers bypass the cooldown so
	// the head is never held in place.
	// Dangling legs also wait kStepCooldown before trying to plant.
	int candidates[NumLegs];
	float candidatePriority[NumLegs];
	int numCandidates = 0;
	for (int i = 0; i < NumLegs; i++) {
		if (legs[i].stepProgress < 1.f) continue;
		vec rest = headPos + legs[i].footRestOffset;
		float discomfort = legs[i].footPos.Distance(rest);
		bool offCooldown = (legs[i].stepCooldown >= kStepCooldown);
		if (legs[i].isPlanted) {
			vec shoulder = headPos + legs[i].shoulderOffset;
			float D = legs[i].footPos.Distance(shoulder);
			float maxReach = (legs[i].upperLen + legs[i].lowerLen) * kMaxReachFactor;
			float minReach = (legs[i].upperLen + legs[i].lowerLen) * kMinReachFactor;
			float stretchUrgency = D > maxReach * kStretchLiftoffFactor
				? (D - maxReach * kStretchLiftoffFactor) + 40.f
				: 0.f;
			float compressUrgency = D < minReach * kCompressionLiftoffFactor
				? (minReach * kCompressionLiftoffFactor - D) + 40.f
				: 0.f;
			bool urgent = (stretchUrgency > 0.f || compressUrgency > 0.f);
			bool soft = (discomfort > kStepDiscomfortThreshold && offCooldown);
			if (urgent || soft) {
				candidates[numCandidates] = i;
				candidatePriority[numCandidates] = std::max({ discomfort, stretchUrgency, compressUrgency });
				numCandidates++;
			}
		} else {
			// Briefly unplanted — either spawning in or just lost a tile. Top
			// priority so the leg re-plants on the very next step slot.
			candidates[numCandidates] = i;
			candidatePriority[numCandidates] = 1.0e6f;
			numCandidates++;
		}
	}
	for (int i = 1; i < numCandidates; i++) {
		int ki = candidates[i];
		float kd = candidatePriority[i];
		int j = i - 1;
		while (j >= 0 && candidatePriority[j] < kd) {
			candidates[j + 1] = candidates[j];
			candidatePriority[j + 1] = candidatePriority[j];
			j--;
		}
		candidates[j + 1] = ki;
		candidatePriority[j + 1] = kd;
	}
	for (int idx = 0; idx < numCandidates && activeSteps < MaxConcurrentSteps; idx++) {
		if (TryStartStep(candidates[idx])) {
			activeSteps++;
		}
	}
}

int HexapodBoss::NaturalBendSign(vec shoulder, vec foot, vec head)
{
	// Knee should sit on the side of the shoulder→foot line away from the
	// head — so a leg standing on the floor curls one way and a leg hanging
	// from the ceiling curls the other. The 2D cross between (foot − shoulder)
	// and (head − shoulder) tells which side the head is on.
	float headSide = (foot.x - shoulder.x) * (head.y - shoulder.y)
	               - (foot.y - shoulder.y) * (head.x - shoulder.x);
	return (headSide > 0.f) ? -1 : +1;
}

vec HexapodBoss::ComputeKnee(vec shoulder, vec foot, float upperLen, float lowerLen, int bendSign)
{
	vec delta = foot - shoulder;
	float D = delta.Length();
	const float minD = fabsf(upperLen - lowerLen) + 0.01f;
	const float maxD = upperLen + lowerLen - 0.01f;
	float clampedD = std::clamp(D, minD, maxD);

	vec dir = (D > 0.0001f) ? (delta / D) : vec(0.f, 1.f);

	float cosAlpha = (upperLen * upperLen + clampedD * clampedD - lowerLen * lowerLen) / (2.f * upperLen * clampedD);
	cosAlpha = std::clamp(cosAlpha, -1.f, 1.f);
	float sinAlpha = sqrtf(1.f - cosAlpha * cosAlpha);

	vec perp = dir.Perp();
	// bendSign is latched at step start so the elbow can't snap mid-arc
	// when the foot crosses the (otherwise dynamic) flip line.
	if (bendSign < 0) perp = -perp;
	return shoulder + upperLen * (cosAlpha * dir + sinAlpha * perp);
}

void HexapodBoss::Draw() const
{
	for (int i = 0; i < NumLegs; i++) {
		const Leg& leg = legs[i];
		// A leg is only skipped during the brief unplanted interval (spawn or
		// tile-just-destroyed) before its re-step starts — no permanent
		// "tucked" state.
		if (!leg.isPlanted && leg.stepProgress >= 1.f) continue;
		vec shoulder = headPos + leg.shoulderOffset;
		vec foot = leg.footPos;
		vec knee = ComputeKnee(shoulder, foot, leg.upperLen, leg.lowerLen, leg.bendSign);
		Window::DrawPrimitive::Line(shoulder, knee, kLegThickness, 120, 40, 50, 255);
		Window::DrawPrimitive::Line(knee, foot, kLegThickness, 120, 40, 50, 255);
		Window::DrawPrimitive::Circle(knee, 2.f, -1.f, 80, 20, 30, 255);
		Window::DrawPrimitive::Circle(foot, kFootRadius, -1.f, 40, 40, 40, 255);
	}

	Window::DrawPrimitive::Circle(headPos, kHeadRadius, -1.f, 180, 50, 50, 255);
	Window::DrawPrimitive::Circle(headPos, kHeadRadius, 1.5f, 60, 10, 20, 255);
	Window::DrawPrimitive::Circle(headPos + vec( 4.f, -2.f), 2.f, -1.f, 240, 240, 240, 255);
	Window::DrawPrimitive::Circle(headPos + vec(-4.f, -2.f), 2.f, -1.f, 240, 240, 240, 255);
	Window::DrawPrimitive::Circle(headPos + vec( 4.f, -2.f), 1.f, -1.f, 20, 20, 20, 255);
	Window::DrawPrimitive::Circle(headPos + vec(-4.f, -2.f), 1.f, -1.f, 20, 20, 20, 255);

#ifdef _DEBUG
	if (Debug::Draw) {
		// Move target + line from head.
		Window::DrawPrimitive::Circle(moveTarget, 3.f, -1.f, 0, 255, 255, 255);
		Window::DrawPrimitive::Line(headPos, moveTarget, 1.f, 0, 200, 200, 200);

		// Planted feet get a purple ring.
		for (int i = 0; i < NumLegs; i++) {
			if (legs[i].isPlanted) {
				Window::DrawPrimitive::Circle(legs[i].footPos, 5.f, 1.f, 200, 0, 200, 200);
			}
		}

		// Each leg's stretch state: a small ring around the shoulder shaded by D/maxReach
		// (so we can see at a glance which legs are pinning the head).
		for (int i = 0; i < NumLegs; i++) {
			const Leg& leg = legs[i];
			vec shoulder = headPos + leg.shoulderOffset;
			float D = leg.footPos.Distance(shoulder);
			float maxReach = (leg.upperLen + leg.lowerLen) * kMaxReachFactor;
			float t = std::clamp(D / maxReach, 0.f, 1.2f);
			uint8_t r = (uint8_t)(std::clamp(t, 0.f, 1.f) * 255);
			uint8_t g = (uint8_t)((1.f - std::clamp(t, 0.f, 1.f)) * 255);
			Window::DrawPrimitive::Circle(shoulder, 4.f, 1.f, r, g, 0, 220);
		}
	}
#endif
}
