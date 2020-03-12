#pragma once

#include "game_data.h"
#include "mates.h"
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

	JumpScene();
	void EnterScene() override;
	void ExitScene() override;
	void Update(int dtMilis) override;
	void Draw(sf::RenderTarget& window) override;

};
