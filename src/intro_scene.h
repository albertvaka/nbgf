#pragma once

#include <string>
#include <vector>

#include "scene_manager.h"
#include "game_data.h"
#include "collider.h"
#include "mates.h"
#include "input.h"
#include "entity.h"

#include "main_scene.h"
#include "cleaner.h"

extern std::vector< std::vector<bool> > passableCleaner;

struct IntroScene : Scene {

	sf::Texture texture;
	sf::Sprite sprite;
	sf::Font font;
	sf::Text text;

	void EnterScene() override
	{
		Camera::SetZoom(GameData::GAME_ZOOM);
		Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));

		texture.loadFromFile("data/spritesheet.png");
		sprite.setTexture(texture);
		sprite.setTextureRect(sf::IntRect(16, 16, 16, 16));

		passableCleaner.resize(23, std::vector<bool>(17, false));
		for (int x = 0; x < passableCleaner.size(); x++){
			for (int y = 0; y < passableCleaner[x].size(); y++){
				bool borde = (x == 0 || y == 0 || x == passableCleaner.size()-1 || y == passableCleaner[x].size() -2);
				passableCleaner[x][y] = !borde && ((x % 3 == 0) || (y % 3 == 0));
			}
		}

		for (int i = 0; i < 50; i++) {
			Cleaner * a = new Cleaner(vec::Rand(32,32,GameData::WINDOW_WIDTH/GameData::GAME_ZOOM-32,GameData::WINDOW_HEIGHT/GameData::GAME_ZOOM-32), nullptr);
			a->DecideNextAction();
			for (Cleaner* other : EntS<Cleaner>::getAll()) {
				if (a == other) continue;
				if (!other->alive) continue;
				auto xy = PosToTile(a->pos);
				if (Collide(a, other) || !passableCleaner[xy.x][xy.y]) {
					a->alive = false;
					//i--;
				}
			}
		}
		EntS<Cleaner>::deleteNotAlive();

		font.loadFromFile("data/PressStart2P.ttf");
		text.setFont(font);
		text.setPosition(100, 100);
		text.setString("PRESS A TO START");
		text.setScale(0.3f, 0.3f);

	}

	void ExitScene() override {
		EntS<Cleaner>::deleteAll();
	}

	void Update(float dt) override {
		UpdateInt(dt * 1000);
	}

	void UpdateInt(int dt) {
		// Update all entities
		if (EntS<Entity>::getAll().size() * 2 > EntS<Entity>::getAll().capacity()) {
			// Hack to make sure the vector of Entities doesn't grow while we iterate it
			EntS<Entity>::getAll().reserve(EntS<Entity>::getAll().size() * 2);
		}
		for (HospitalEntity* e : EntS<HospitalEntity>::getAll())
		{
			e->Update(dt);
			e->UpdateAnim(dt);
		}

		if (Keyboard::IsKeyJustPressed(GameKeys::START) || GamePad::IsButtonJustPressed(0, GamePad::Button::A)) {
			SceneManager::SetScene(new MainScene());
			//Change scene
		}
	}

	void Draw(sf::RenderTarget& window) override
	{
		window.clear(sf::Color::Black);

		//Entities
		sprite.setOrigin(8, 8);
		EntS<SortedDrawable>::sort([](SortedDrawable* a, SortedDrawable* b)
		{
			return a->pos.y < b->pos.y;
		});
		for (SortedDrawable* e : EntS<SortedDrawable>::getAll())
		{
			e->Draw(sprite, window);
		}

		window.draw(text);

	}

};
