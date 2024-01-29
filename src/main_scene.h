#pragma once

#include "mates.h"
#include "input.h"
#include "entity.h"
#include "main_scene_collider.h"
#include "main_scene_tile.h"
#include "imgui.h"

#include "player.h"
#include "cadaver.h"
#include "cinta.h"
#include "cleaner.h"
#include "spawner.h"
#include "lever.h"
#include "mesa.h"
#include "door.h"

struct MainScene : Scene {


	void EnterScene() override
	{
		Camera::SetZoom(Window::GAME_ZOOM);
		Camera::SetCenter(vec(Window::GAME_WIDTH / (2*Window::GAME_ZOOM), Window::GAME_HEIGHT/(2*Window::GAME_ZOOM)));

		LoadMap();

		for (Collector* c : SelfRegister<Collector>::GetAll())
		{
			for (Mesa* m : SelfRegister<Mesa>::GetAll())
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
		SelfRegister<Entity>::DeleteAll();
		SelfRegister<Cintable>::DeleteAll();
	}

	void Update(float floatDt) override {
		int dt = int(floatDt*1000);

		//Collisions computed in main_scene_collider.h
		main_scene_collisions();

		// Update all entities
		if (SelfRegister<Entity>::GetAll().size() * 2 > SelfRegister<Entity>::GetAll().capacity()) {
			// Hack to make sure the vector of Entities doesn't grow while we iterate it
			SelfRegister<Entity>::GetAll().reserve(SelfRegister<Entity>::GetAll().size() * 2);
		}
		for (Entity* e : SelfRegister<Entity>::GetAll())
		{
			e->Update(dt);
			if (e->anim.anim_type != AnimationType::NADA) {
				e->anim.Update(dt);
			}
		}
		for (Taca* e : SelfRegister<Taca>::GetAll())
		{
			e->Update(dt);
		}

		//Spawn roombas
		int num_tacs = SelfRegister<Taca>::GetAll().size();
		int num_rumbs = SelfRegister<Cleaner>::GetAll().size();
		if (num_tacs/(num_rumbs + 1) > 1300 && num_rumbs < 3) {
			int spawners_count = SelfRegister<CleanerSpawner>::GetAll().size();

			int sp = Rand::roll(0, spawners_count);

			SelfRegister<CleanerSpawner>::GetAll()[sp]->TreuElGos();
		}

		// Do the deletes
		SelfRegister<Taca>::DeleteNotAlive();
		SelfRegister<Entity>::DeleteNotAlive();
	}

	void DrawImgui()
	{
	#ifdef _DEBUG
		ImGui::Begin(Window::WINDOW_TITLE);

		ImGui::Text(SelfRegister<Player>::GetAll()[0]->pos.ToString().c_str());
		if (ImGui::Button("SPAWN CADAVER"))
		{
			for (Spawner* s : SelfRegister<Spawner>::GetAll()) {
				s->spawn();
			}
		}

		ImGui::Checkbox("Blood", &withTaca);

		ImGui::Text(std::to_string(SelfRegister<Cadaver>::GetAll().size()).c_str());

		ImGui::Text(std::to_string(SelfRegister<Taca>::GetAll().size()).c_str());

		if (ImGui::Button("SPAWN ROOMBA"))
		{
			int spawners_count = SelfRegister<CleanerSpawner>::GetAll().size();

			int sp = Rand::roll(0, spawners_count);

			SelfRegister<CleanerSpawner>::GetAll()[sp]->TreuElGos();

		}

		ImGui::End();
	#endif
	}

	void Draw() override
	{
		Window::Clear(64, 64, 64);

		//Tilemap
		for (int i = 0; i < mapita.size(); ++i)
		{
			for (int j = 0; j < mapita[i].size(); ++j)
			{
				drawTile(i, j);
			}
		}

		//Entities
		for (Taca* e : SelfRegister<Taca>::GetAll())
		{
			e->Draw();
		}
		SelfRegister<SortedDrawable>::Sort([](SortedDrawable* a, SortedDrawable* b)
		{
			return a->pos.y < b->pos.y;
		});
		for (SortedDrawable* e : SelfRegister<SortedDrawable>::GetAll())
		{
			e->Draw();
		}

		for (Bullet* e : SelfRegister<Bullet>::GetAll())
		{
			e->Draw();
		}

		for (TextMolest* e : SelfRegister<TextMolest>::GetAll())
		{
			e->Draw();
		}

		// GUI
		DrawImgui();
	}

};
