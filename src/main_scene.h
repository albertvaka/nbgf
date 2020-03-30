#pragma once

#include "game_data.h"
#include "mates.h"
#include "input.h"
#include "hospital_entity.h"
#include "main_scene_collider.h"
#include "main_scene_tile.h"

#include "player.h"
#include "cadaver.h"
#include "cinta.h"
#include "cleaner.h"
#include "spawner.h"
#include "lever.h"
#include "mesa.h"
#include "door.h"
#include "assets.h"

struct MainScene : Scene {

	void EnterScene() override
	{
		Camera::SetZoom(GameData::GAME_ZOOM);
		Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));

		LoadMap();

		for (Collector* c : EntS<Collector>::getAll())
		{
			for (Mesa* m : EntS<Mesa>::getAll())
			{
				if (m->type == c->type)
				{
					m->collector = c;
					c->mesa = m;
				}
			}
		}
		loadExtremityMap();

	#ifdef _DEBUG
		new Cadaver(vec(100, 120));
	#endif
	}

	void ExitScene() override {
		EntS<HospitalEntity>::deleteAll();
		EntS<Cintable>::deleteAll();
	}

	void Update(int dt) override {

		//Collisions computed in main_scene_collider.h
		main_scene_collisions();

		// Update all entities
		if (EntS<HospitalEntity>::getAll().size() * 2 > EntS<HospitalEntity>::getAll().capacity()) {
			// Hack to make sure the vector of Entities doesn't grow while we iterate it
			EntS<HospitalEntity>::getAll().reserve(EntS<HospitalEntity>::getAll().size() * 2);
		}
		for (HospitalEntity* e : EntS<HospitalEntity>::getAll())
		{
			e->Update(dt);
			e->UpdateAnim(dt);
		}
		for (Taca* e : EntS<Taca>::getAll())
		{
			e->Update(dt);
		}

		for (Bullet* e : EntS<Bullet>::getAll())
		{
			e->Update(dt/1000.f);
		}


		//Spawn roombas
		int num_tacs = EntS<Taca>::getAll().size();
		int num_rumbs = EntS<Cleaner>::getAll().size();
		if (num_tacs/(num_rumbs + 1) > 1300 && num_rumbs < 3) {
			int spawners_count = EntS<CleanerSpawner>::getAll().size();

			int sp = Random::roll(0, spawners_count-1);

			EntS<CleanerSpawner>::getAll()[sp]->TreuElGos();
		}

		// Do the deletes
		EntS<Taca>::deleteNotAlive();
		EntS<HospitalEntity>::deleteNotAlive();
		EntS<Bullet>::deleteNotAlive();

		if (Keyboard::IsKeyJustPressed(GameKeys::RESTART)) {
			SceneManager::SetScene(new MainScene());
		}
	}

	void DrawImgui()
	{
	#ifdef _DEBUG
		ImGui::Begin(GameData::GAME_TITLE.c_str());

		ImGui::Text(EntS<Player>::getAll()[0]->pos.ToString().c_str());
		if (ImGui::Button("SPAWN CADAVER"))
		{
			for (Spawner* s : EntS<Spawner>::getAll()) {
				s->spawn();
			}
		}

		ImGui::Checkbox("Blood", &withTaca);

		ImGui::Text(std::to_string(EntS<Cadaver>::getAll().size()).c_str());

		ImGui::Text(std::to_string(EntS<Taca>::getAll().size()).c_str());

		if (ImGui::Button("SPAWN ROOMBA"))
		{
			int spawners_count = EntS<CleanerSpawner>::getAll().size();

			int sp = Random::roll(0, spawners_count - 1);

			EntS<CleanerSpawner>::getAll()[sp]->TreuElGos();

		}

		ImGui::End();
	#endif
	}

	void Draw(sf::RenderTarget& window, bool debug) override
	{
		sf::Sprite& sprite = Assets::hospitalSprite;

		window.clear(sf::Color(64, 64, 64));

		//Tilemap
		sprite.setOrigin(0, 0);
		for (int i = 0; i < mapita.size(); ++i)
		{
			for (int j = 0; j < mapita[i].size(); ++j)
			{
				drawTile(sprite, window, i, j);
			}
		}

		//Entities
		sprite.setOrigin(8, 8);
		for (Taca* e : EntS<Taca>::getAll())
		{
			e->Draw(sprite, window);
		}
		EntS<SortedDrawable>::sort([](SortedDrawable* a, SortedDrawable* b)
		{
			return a->pos.y < b->pos.y;
		});
		for (SortedDrawable* e : EntS<SortedDrawable>::getAll())
		{
			e->Draw(sprite, window);
		}

		for (Bullet* e : EntS<Bullet>::getAll())
		{
			e->Draw(window);
		}

		for (TextMolest* e : EntS<TextMolest>::getAll())
		{
			e->Draw(sprite, window);
		}

		// GUI
		DrawImgui();
	}

};
