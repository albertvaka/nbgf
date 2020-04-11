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

	static sf::RectangleShape topLayer(vec(chunkSize, 5.f));
	topLayer.setFillColor(sf::Color(220, 10, 10)); // Top layer
	static sf::RectangleShape lineLayer(vec(chunkSize, 1.f));
	lineLayer.setFillColor(sf::Color(120, 0, 0)); // Line
	static sf::RectangleShape bottomLayer(vec(chunkSize, height));
	bottomLayer.setFillColor(sf::Color(250, 140, 50)); // Bottom layer


	float left = (Mates::fastfloor(screen.Left() / chunkSize) - 1) * chunkSize;
	float right = (Mates::fastfloor(screen.Right() / chunkSize) + 1) * chunkSize;
	for (float x = left; x < right; x += chunkSize)
	{
		float y = yBottom - waveHeight * sin(x * waveAmplitude + time);

		topLayer.setPosition(vec(x, y));
		window.draw(topLayer);
		lineLayer.setPosition(vec(x, y + 5.f));
		window.draw(lineLayer);
		bottomLayer.setPosition(vec(x, y + 6.f));
		window.draw(bottomLayer);
	}

}
