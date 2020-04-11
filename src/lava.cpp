#include "lava.h"

#include "assets.h"
#include "input.h"
#include "debug.h"

const float waveAmplitude = 1.f;
const float chunkSize = 5.4f;
const float waveHeight = 2.8f;
const float height = 22.f;
const float speed = 3.0f;

extern sf::Clock mainClock;

Lava::Lava(float yBottom) 
	: posY(yBottom)
{
	lavaPartSys.AddSprite(Assets::marioTexture, sf::IntRect(5 + 16, 37, 6, 6));
	lavaPartSys.AddSprite(Assets::marioTexture, sf::IntRect(38, 37, 5, 5));

	lavaPartSys.pos.y = posY - 2;
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
	const float chunkSize = 15.f;
	Bounds screen = Camera::GetCameraBounds();
	float left = (Mates::fastfloor(screen.Left() / chunkSize) - 1) * chunkSize;
	float right = (Mates::fastfloor(screen.Right() / chunkSize) + 1) * chunkSize;
	for (float x = left; x < right; x += chunkSize) {
		lavaPartSys.pos.x = x;
		lavaPartSys.Spawn(dt);
	}
	lavaPartSys.UpdateParticles(dt);
}

#define USE_VAO

#ifdef USE_VAO
sf::VertexArray lavaVA(sf::Quads);
inline void AddQuad(float x, float y, float width, float height, const sf::Color& color) {
	lavaVA.append({ vec(x, y), color });
	lavaVA.append({ vec(x + width, y), color });
	lavaVA.append({ vec(x + width, y + height), color });
	lavaVA.append({ vec(x, y + height), color });
}
#endif

void Lava::Draw(sf::RenderTarget& window) {

	lavaPartSys.Draw(window);
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

	float time = mainClock.getElapsedTime().asSeconds() * speed;
	Bounds screen = Camera::GetCameraBounds();
	float yBottom = posY;


	const float heightTopLayer = 5.f;
	const float heightMiddleLayer = 1.f;
	const float heightBottomLayer = height;

	const sf::Color colorTopLayer(220, 10, 10);
	const sf::Color colorMiddleLayer(120, 0, 0);
	const sf::Color colorBottomLayer(250, 140, 50);

#ifdef USE_VAO
	lavaVA.clear();
#else
	static sf::RectangleShape topLayer(vec(chunkSize, heightTopLayer));
	topLayer.setFillColor(colorTopLayer);
	static sf::RectangleShape lineLayer(vec(chunkSize, heightMiddleLayer));
	lineLayer.setFillColor(colorMiddleLayer);
	static sf::RectangleShape bottomLayer(vec(chunkSize, heightBottomLayer));
	bottomLayer.setFillColor(colorBottomLayer);
#endif

	float left = (Mates::fastfloor(screen.Left() / chunkSize) - 1) * chunkSize;
	float right = (Mates::fastfloor(screen.Right() / chunkSize) + 1) * chunkSize;
	for (float x = left; x < right; x += chunkSize)
	{
		float y = yBottom - waveHeight * sin(x * waveAmplitude + time);

#ifdef USE_VAO
		AddQuad(x, y, chunkSize, heightTopLayer, colorTopLayer);
		AddQuad(x, y + heightTopLayer, chunkSize, heightMiddleLayer, colorMiddleLayer);
		AddQuad(x, y + heightTopLayer + heightMiddleLayer, chunkSize, heightBottomLayer, colorBottomLayer);
#else
		topLayer.setPosition(vec(x, y));
		window.draw(topLayer);
		lineLayer.setPosition(vec(x, y + heightTopLayer));
		window.draw(lineLayer);
		bottomLayer.setPosition(vec(x, y + heightTopLayer + heightMiddleLayer));
		window.draw(bottomLayer);
#endif
	}

#ifdef USE_VAO
	window.draw(lavaVA);
#endif


}
