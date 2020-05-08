#include "lava.h"

#include "assets.h"
#include "input.h"
#include "debug.h"
#include "collider.h"

const float waveAmplitude = 1.f;
const float chunkSize = 5.4f;
const float waveHeight = 2.8f;
const float speed = 3.0f;
const float distanceBetweenParticleSpawners = 15.f;

const float raiseSpeed = 15.f;

extern float mainClock;



Lava::Lava(const Bounds& b)
	: bounds(b)
	, targetY(b.Top())
	, lavaPartSys(Assets::marioTexture)
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

void Lava::Update(float dt) {
	if (targetY > bounds.top) {
		if (bounds.top - targetY > raiseSpeed * dt) {
			bounds.height += bounds.top - targetY;
			bounds.top = targetY;
		}
		else {
			bounds.top += raiseSpeed * dt;
			bounds.height -= raiseSpeed * dt;
		}
	}
	else if (targetY < bounds.top) {
		if (targetY - bounds.top > raiseSpeed * dt) {
			bounds.height += bounds.top - targetY;
			bounds.top = targetY;
		}
		else {
			bounds.top -= raiseSpeed * dt;
			bounds.height += raiseSpeed * dt;
		}
	}

	if (!Collide(Camera::GetBounds(),bounds)) {
		return;
	}

	Bounds screen = Camera::GetBounds();
	float left = Mates::MaxOf(screen.Left(), bounds.Left());
	float right = Mates::MinOf(screen.Right(), bounds.Right());
	float chunkLeft = (Mates::fastfloor(left / chunkSize)) * chunkSize;
	float chunkRight = (Mates::fastfloor(right / chunkSize)) * chunkSize;
	lavaPartSys.pos.y = bounds.Top() - 2;
	for (float x = chunkLeft; x < chunkRight; x += distanceBetweenParticleSpawners) {
		lavaPartSys.pos.x = x;
		lavaPartSys.Spawn(dt);
	}
	lavaPartSys.UpdateParticles(dt);
}

//#define USE_VAO

#ifdef USE_VAO
sf::VertexArray lavaVA(sf::Quads);
inline void AddQuad(float x, float y, float width, float height, const sf::Color& color) {
	lavaVA.append({ vec(x, y), color });
	lavaVA.append({ vec(x + width, y), color });
	lavaVA.append({ vec(x + width, y + height), color });
	lavaVA.append({ vec(x, y + height), color });
}
#endif

void Lava::Draw() const {

	if (!Collide(Camera::GetBounds(),bounds)) {
		return;
	}

	lavaPartSys.Draw();
	//lavaPartSys.DrawImGUI("LavaPartSys");

	if (Debug::Draw) {
		return;
	}

	/*
	ImGui::Begin("Waves");
	ImGui::SliderFloat("speed", &speed, 0.f, 10.f);
	ImGui::SliderFloat("height", &height, 0.f, 40.f);
	ImGui::SliderFloat("waveHeight", &waveHeight, 0.f, 10.f);
	ImGui::SliderFloat("chunkSize", &chunkSize, 0.1f, 10.f);
	ImGui::SliderFloat("waveAmplitude", &waveAmplitude, 0.f, 5.f);
	ImGui::End();
	*/

	float time = mainClock * speed;
	Bounds screen = Camera::GetBounds();


	const float heightTopLayer = 5.f;
	const float heightMiddleLayer = 1.f;
	const float heightBottomLayer = bounds.height;

	const SDL_Color colorTopLayer = { 220, 10, 10, 255 };
	const SDL_Color colorMiddleLayer = { 120, 0, 0, 255 };
	const SDL_Color colorBottomLayer = { 250, 140, 50, 255 };

#ifdef USE_VAO
	lavaVA.clear();
#else
	static Bounds topLayer(vec(chunkSize, heightTopLayer));
	static Bounds middleLayer(vec(chunkSize, heightMiddleLayer));
	static Bounds bottomLayer(vec(chunkSize, heightBottomLayer));
#endif

	float left = Mates::MaxOf(screen.Left(), bounds.Left());
	float right = Mates::MinOf(screen.Right(), bounds.Right());
	float chunkLeft = (Mates::fastfloor(left / chunkSize)) * chunkSize;
	float chunkRight = (Mates::fastfloor(right / chunkSize)) * chunkSize;
	for (float x = chunkLeft; x < chunkRight; x += chunkSize)
	{
		float y = bounds.top - waveHeight * sin(x * waveAmplitude + time);

#ifdef USE_VAO
		AddQuad(x, y, chunkSize, heightTopLayer, colorTopLayer);
		AddQuad(x, y + heightTopLayer, chunkSize, heightMiddleLayer, colorMiddleLayer);
		AddQuad(x, y + heightTopLayer + heightMiddleLayer, chunkSize, heightBottomLayer, colorBottomLayer);
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
	window.draw(lavaVA);
#endif

}
