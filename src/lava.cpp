#include "lava.h"

#include "assets.h"
#include "input.h"

const float waveAmplitude = 1.f;
const float chunkSize = 5.4f;
const float waveHeight = 2.8f;
const float height = 22.f;
const float speed = 3.0f;
const bool blockyWaves = true;

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

void  Lava::Update(float dt) {
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

void Lava::Draw(sf::RenderTarget& window, bool debugDraw) {

	lavaPartSys.Draw(window);
	//lavaPartSys.DrawImGUI("LavaPartSys");

	if (debugDraw) {
		return;
	}

	/*
	ImGui::Begin("Waves");
	ImGui::SliderFloat("speed", &speed, 0.f, 10.f);
	ImGui::SliderFloat("height", &height, 0.f, 40.f);
	ImGui::SliderFloat("waveHeight", &waveHeight, 0.f, 10.f);
	ImGui::SliderFloat("chunkSize", &chunkSize, 0.1f, 10.f);
	ImGui::SliderFloat("waveAmplitude", &waveAmplitude, 0.f, 5.f);
	ImGui::Checkbox("blockyWaves", &blockyWaves);
	ImGui::End();
	*/

	float time = mainClock.getElapsedTime().asSeconds() * speed;
	Bounds screen = Camera::GetCameraBounds();
	float yBottom = posY + 16;

	sf::ConvexShape poly;
	poly.setPointCount(4);

	float left = (Mates::fastfloor(screen.Left() / chunkSize) - 1) * chunkSize;
	float right = (Mates::fastfloor(screen.Right() / chunkSize) + 1) * chunkSize;
	for (float x0 = left; x0 < right; x0 += chunkSize)
	{
		float xf = x0 + chunkSize;
		float xf4sin = blockyWaves ? x0 : xf;
		//float iF = blockyWaves ? i0 : i0 + 1.f;
		poly.setPosition(vec(0, yBottom));
		poly.setPoint(0, vec(x0, 0));
		poly.setPoint(1, vec(x0, -height - waveHeight * sin(x0 * waveAmplitude + time)));
		poly.setPoint(2, vec(xf, -height - waveHeight * sin(xf4sin * waveAmplitude + time)));
		poly.setPoint(3, vec(xf, 0));

		poly.setPosition(vec(0, yBottom));
		poly.setFillColor(sf::Color(220, 10, 10)); // Top layer
		window.draw(poly);

		poly.setPosition(vec(0, yBottom + 5.f));
		poly.setFillColor(sf::Color(120, 0, 0)); // Line
		window.draw(poly);

		poly.setPosition(vec(0, yBottom + 6.f));
		poly.setFillColor(sf::Color(250, 140, 50)); // Bottom layer
		window.draw(poly);
	}

}
