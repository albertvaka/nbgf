#include "lava.h"

#include "assets.h"
#include "debug.h"
#include "collide.h"
#include "camera.h"
#include "mates.h"
#include "anim_lib.h"
#include "window_drawraw.h"
#include "player.h"

constexpr const float kWaveAmplitude = 1.f;
constexpr const float kChunkSize = 5.4f;
constexpr const float kWaveHeight = 2.8f;
constexpr const float kSpeed = 3.0f;
constexpr const float kDistanceBetweenParticleSpawners = 15.f;


Lava::Lava(const BoxBounds& b)
	: bounds(b)
	, targetY(b.Top())
	, lavaPartSys(Assets::spritesheetTexture)
{
	lavaPartSys.AddSprite({ 5 + 16, 37, 6, 6 });
	lavaPartSys.AddSprite({ 38, 37, 5, 5 });

	lavaPartSys.min_interval = 4.f;
	lavaPartSys.max_interval = 6.5f;
	lavaPartSys.min_vel = vec(-15, -55);
	lavaPartSys.max_vel = vec(15, -40);
	lavaPartSys.min_ttl = 2.f;
	lavaPartSys.max_ttl = 2.f;
	lavaPartSys.min_scale = 1.f;
	lavaPartSys.max_scale = 1.5f;
	lavaPartSys.alpha_vel = -0.5f;
	lavaPartSys.scale_vel = -0.4f;
	lavaPartSys.acc = vec(0, 60.f);
}

Mates::Range Lava::GetChunksOnScreen() const {
	BoxBounds screen = Camera::Bounds();
	float screenChunkLeft = (Mates::fastfloor(screen.Left() / kChunkSize)) * kChunkSize;
	float screenChunkRight = (Mates::fastfloor(screen.Right() / kChunkSize)) * kChunkSize;
	float boundsChunkLeft = (Mates::fastfloor(bounds.Left() / kChunkSize)) * kChunkSize;
	float boundsChunkRight = (Mates::fastfloor(bounds.Right() / kChunkSize)) * kChunkSize;
	float chunkLeft = std::max(screenChunkLeft, boundsChunkLeft);
	float chunkRight = std::min(screenChunkRight, boundsChunkRight);
	return Mates::Range{ chunkLeft,chunkRight };
}

void Lava::Update(float dt) {
	timer += dt;

	const float bottom = bounds.Bottom();
	float top = bounds.Top();
	if (top < targetY) {
		top += raiseSpeed * dt;
		if (top > targetY) {
			top = targetY;
		}
	} else if (top > targetY) {
		float camBottom = Camera::Bounds().Bottom() - 1.f;
		if (top > camBottom) {
			top = camBottom;
		}
		top -= raiseSpeed * dt;
		if (top < targetY) {
			top = targetY;
		}
	}
	bounds.SetTopAndBottom(top, bottom);

	if (!Collide(Camera::Bounds(),bounds)) {
		return;
	}

	Mates::Range chunks = GetChunksOnScreen();
	lavaPartSys.pos.y = bounds.Top() - 2;
	for (float x = chunks.min; x < chunks.max; x += kDistanceBetweenParticleSpawners) {
		lavaPartSys.pos.x = x;
		lavaPartSys.Spawn(dt);
	}
	lavaPartSys.UpdateParticles(dt);

	// Kill the player
	Player* player = Player::instance();
	const float kLavaDamageAreaOffsetFromTop = 8.5f;
	if (IsInside(player->pos - vec(0, kLavaDamageAreaOffsetFromTop))) {
		// TODO: All this should be done by a method of player
		if (!player->frozen) {
			player->anim.Ensure(AnimLib::WARRIOR_HURT, false);
			player->pos.y = CurrentLevel() + kLavaDamageAreaOffsetFromTop;
			player->frozen = true; // disable movement
			player->health -= 1;
		}
		player->invencibleTimer = 1;
		player->pos.y += 6 * dt; //sink slowly in the lava
		player->bfgPos.y = -1000;
		player->onWall = false;
		player->crouched = false;
		player->dashing = false;
		player->diving = false;
		player->attacking = false;

		// we don't need this because we never lower the lava level, but if we did we would have
		// to do something like this to prevent it lowering after killing us and suddently us not being inside
		//if (targetY > CurrentLevel()) {
			//targetY = CurrentLevel();
		//}
	}
	if (IsInside(player->pos - vec(0, 13.f))) {
		player->ToSafeGround();
		if (targetY != bounds.top) { // moving lava
			float heightBelowPlayerPos = player->SafeGroundPos().y + 4 * Tile::Size;
			if (heightBelowPlayerPos > CurrentLevel()) {
				float prev_target = targetY;
				SetLevel(heightBelowPlayerPos, true);
				SetLevel(prev_target, false);
			}
		}
	}
}

void Lava::SetLevel(float newY, bool immediate) {
	const float bottom = bounds.Bottom();
	if (newY > bottom) {
		newY = bottom;
	}
	targetY = newY;
	if (immediate) {
		bounds.SetTopAndBottom(newY, bottom);
	}
}

#define USE_VAO

void Lava::Draw() const {

	if (!Collide(Camera::Bounds(),bounds)) {
		return;
	}

	lavaPartSys.Draw();
	//lavaPartSys.DrawImGUI("LavaPartSys");

	if (Debug::Draw) {
		bounds.DebugDraw();
		return;
	}

	/*
	ImGui::Begin("Waves");
	ImGui::SliderFloat("speed", &speed, 0.f, 10.f);
	ImGui::SliderFloat("height", &height, 0.f, 40.f);
	ImGui::SliderFloat("waveHeight", &waveHeight, 0.f, 10.f);
	ImGui::SliderFloat("kChunkSize", &kChunkSize, 0.1f, 10.f);
	ImGui::SliderFloat("waveAmplitude", &waveAmplitude, 0.f, 5.f);
	ImGui::End();
	*/

	float time = timer * kSpeed;

	const float heightTopLayer = 5.f;
	const float heightMiddleLayer = 1.f;
	const float heightBottomLayer = bounds.height - heightMiddleLayer - heightMiddleLayer - kWaveHeight;

	const SDL_Color colorTopLayer = { 220, 10, 10, 255 };
	const SDL_Color colorMiddleLayer = { 120, 0, 0, 255 };
	const SDL_Color colorBottomLayer = { 250, 140, 50, 255 };

#ifndef USE_VAO
	Bounds topLayer(vec(kChunkSize, heightTopLayer));
	Bounds middleLayer(vec(kChunkSize, heightMiddleLayer));
	Bounds bottomLayer(vec(kChunkSize, heightBottomLayer));
#endif

	Mates::Range chunks = GetChunksOnScreen();
	for (float x = chunks.min; x < chunks.max; x += kChunkSize)
	{
		float heightDiff = kWaveHeight * sin(x * kWaveAmplitude + time);
		float y = bounds.top - heightDiff;

#ifdef USE_VAO
		Window::DrawRaw::BatchRGBQuad(x, y,
			kChunkSize, heightTopLayer, 
			colorTopLayer.r / 255.f, colorTopLayer.g / 255.f, colorTopLayer.b / 255.f);
		Window::DrawRaw::BatchRGBQuad(x, y + heightTopLayer,
			kChunkSize, heightMiddleLayer, 
			colorMiddleLayer.r / 255.f, colorMiddleLayer.g / 255.f, colorMiddleLayer.b / 255.f);
		Window::DrawRaw::BatchRGBQuad(x, y + heightTopLayer + heightMiddleLayer,
			kChunkSize, heightBottomLayer + heightDiff,
			colorBottomLayer.r / 255.f, colorBottomLayer.g / 255.f, colorBottomLayer.b / 255.f);
#else
		topLayer.SetTopLeft(vec(x, y));
		middleLayer.SetTopLeft(vec(x, y + heightTopLayer));
		bottomLayer.SetTopLeft(vec(x, y + heightTopLayer + heightMiddleLayer));

		Window::DrawPrimitive::Rectangle(topLayer, -1, colorTopLayer);
		Window::DrawPrimitive::Rectangle(middleLayer, -1, colorMiddleLayer);
		Window::DrawPrimitive::Rectangle(bottomLayer, -1, colorBottomLayer);
#endif
	}

#ifdef USE_VAO
	Window::DrawRaw::FlushRGBQuads();
#endif

}
