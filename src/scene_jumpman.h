#pragma once

#include "game_data.h"
#include "mates.h"
#include "input.h"
#include "tilemap.h"
#include "jumpman.h"
#include "scene_manager.h"
#include "transition.h"


struct JumpScene : Scene {

	TransitionSinusFadeOut transition;

	TileMap map;
	JumpMan player;

	sf::Texture texture;
	sf::Sprite sprite;
	sf::Font font;
	sf::Text text;

	JumpScene()
		: player(&map)
	{

	}

	void EnterScene() override
	{
		Camera::SetZoom(GameData::GAME_ZOOM);
		Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));

		texture.loadFromFile("data/spritesheet.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(16, 16, 16, 16));

		transition.setTime(2.0f);
		transition.setPos(0.1f* GameData::GAME_ZOOM);
		transition.goPos(GameData::GAME_ZOOM);

		player.pos = vec(170, 160);
		player.Reset();
		map.Init(time(NULL), sf::Vector2i(1000, 25), 16);

		sf::Vector2i pos = map.tilePos(vec(160, 160));
		map.set(pos.x, pos.y - 1, true);
		map.set(pos.x, pos.y + 0, false);
		map.set(pos.x, pos.y + 1, false);

	}

	void ExitScene() override {
	}

	void Update(int dtMilis) override {
		float dt = dtMilis / 1000.f;
		transition.update(dt);
		player.Update(dt);

		Camera::ChangeZoomWithPlusAndMinus(10.f, dt);
		Camera::MoveCameraWithArrows(50.f, dt);
		if (!transition.reached()) {
			Camera::SetZoom(transition.getPos());
		}
		Camera::SetCameraCenter(player.pos);

		//if (fail) EnterScene();
	}

	void Draw(sf::RenderTarget& window) override
	{
		window.clear(sf::Color(255*0.200f, 255 * 0.100f, 255 * 0.100f));

		map.Draw(sprite, window);
		player.Draw(sprite, window);
	}

};
