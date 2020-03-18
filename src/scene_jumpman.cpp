#pragma once

#include "scene_jumpman.h"
#include "input.h"
#include "imgui.h"

JumpScene::JumpScene()
	: map(sf::Vector2i(1000, 25), 16)
	, player(&map)
{

}

void JumpScene::EnterScene() 
{
	Camera::SetZoom(GameData::GAME_ZOOM);
	Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));

	texture.loadFromFile("data/spritesheet.png");
	sprite.setTexture(texture);
	
	marioTexture.loadFromFile("data/mario.png");
	marioSprite.setTexture(marioTexture);

	transition.setTime(2.0f);
	transition.setPos(0.6f* GameData::GAME_ZOOM);
	transition.goPos(GameData::GAME_ZOOM);

	player.pos = vec(160, 160);
	player.Reset();
	map.Randomize(time(NULL));

	sf::Vector2i pos = map.tilePos(player.pos);
	map.set(pos.x - 1, pos.y + 1, true);
	map.set(pos.x    , pos.y + 1, true);
	map.set(pos.x - 1, pos.y    , false);
	map.set(pos.x    , pos.y    , false);
	map.set(pos.x - 1, pos.y - 1, false);
	map.set(pos.x    , pos.y - 1, false);

	//Setup per debugar fallo raro al colisionar
	map.set(pos.x-1, pos.y - 4, true);
	map.set(pos.x-2, pos.y - 1, true);

}

void JumpScene::ExitScene()  
{
}

void JumpScene::Update(int dtMilis) {
	float dt = dtMilis / 1000.f;
	transition.update(dt);
	player.Update(dt);

	Camera::ChangeZoomWithPlusAndMinus(10.f, dt);
	if (!transition.reached()) {
		Camera::SetZoom(transition.getPos());
	}
	//Camera::MoveCameraWithArrows(50.f, dt);
	vec camPos = player.pos;
	float minY = (Camera::GetCameraBounds().height / 2.f) - (1 * 16);
	float maxY = ((25 + 1) * 16) - (Camera::GetCameraBounds().height / 2.f);
	if (maxY < minY) {
		minY = maxY - 1;
	}
	Mates::Clamp(camPos.y, minY, maxY);
	float minX = (Camera::GetCameraBounds().width / 2.f) - (1 * 16);
	float maxX = ((1000 + 1) * 16) - (Camera::GetCameraBounds().width / 2.f);
	if (maxX < minX) {
		minX = maxX - 1;
	}
	Mates::Clamp(camPos.x, minX, maxX);
	Camera::SetCameraCenter(camPos);

	if (Keyboard::IsKeyJustPressed(GameKeys::RESTART) || (map.tilePos(player.pos + vec(0.01f, 0)).y >= map.sizes.y)) {
		EnterScene();
	}

	if (Mouse::IsPressed(sf::Mouse::Button::Left) || Mouse::IsPressed(sf::Mouse::Button::Right)) {
		bool what_to_set = Mouse::IsPressed(sf::Mouse::Button::Left);
		vec pos = Mouse::GetPositionInWorld();
		sf::Vector2i tile = map.tilePos(pos);
		map.set(tile.x, tile.y, what_to_set);
	}


}

void JumpScene::Draw(sf::RenderTarget& window) 
{
	window.clear(sf::Color(255*0.200f, 255 * 0.100f, 255 * 0.100f));

	map.Draw(sprite, window);

	player.Draw(marioSprite, window);
	//player.bounds().Draw(window);
	//Bounds(player.pos, vec(1, 1)).Draw(window, sf::Color::White);




	ImGui::Begin(GameData::GAME_TITLE.c_str());
	ImGui::SliderFloat("x", &player.pos.x, 0.f, 10 * 16.f);
	ImGui::SliderFloat("y", &player.pos.y, 0.f, 25 * 16.f);

	ImGui::End();
}

