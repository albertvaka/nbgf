#pragma once

#include <string>
#include <vector>

#include "scene_manager.h"
#include "collider.h"
#include "mates.h"
#include "input.h"
#include "entity.h"
#include "window_conf.h"
#include "text.h"

#include "main_scene.h"
#include "cleaner.h"

extern std::vector< std::vector<bool> > passableCleaner;

struct IntroScene : Scene {

	Text text;

	IntroScene() : text(Assets::font_30) {

	}

	void EnterScene() override
	{
		Camera::SetZoom(Window::GAME_ZOOM);
		Camera::SetCenter(vec(Window::GAME_WIDTH / (2*Window::GAME_ZOOM), Window::GAME_HEIGHT/(2*Window::GAME_ZOOM)));

		passableCleaner.resize(23, std::vector<bool>(17, false));
		for (int x = 0; x < passableCleaner.size(); x++){
			for (int y = 0; y < passableCleaner[x].size(); y++){
				bool borde = (x == 0 || y == 0 || x == passableCleaner.size()-1 || y == passableCleaner[x].size() -2);
				passableCleaner[x][y] = !borde && ((x % 3 == 0) || (y % 3 == 0));
			}
		}

		for (int i = 0; i < 50; i++) {
			Cleaner * a = new Cleaner(Rand::VecInRange(32,32,Window::GAME_WIDTH/Window::GAME_ZOOM-32,Window::GAME_HEIGHT/Window::GAME_ZOOM-32), nullptr);
			a->DecideNextAction();
			for (Cleaner* other : SelfRegister<Cleaner>::GetAll()) {
				if (a == other) continue;
				if (!other->alive) continue;
				auto xy = PosToTile(a->pos);
				if (Collision(a, other) || !passableCleaner[xy.x][xy.y]) {
					a->alive = false;
					//i--;
				}
			}
		}
		SelfRegister<Cleaner>::DeleteNotAlive();

		text.SetString("PRESS A TO START");

	}

	void ExitScene() override {
		SelfRegister<Entity>::DeleteAll();
	}

	void Update(float floatDt) override {
		int dt = int(floatDt*1000);
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

		if (Input::IsPressedAnyPlayer(GameKeys::START)) {
			SceneManager::ChangeScene(new MainScene());
			//Change scene
		}
	}

	void Draw() override
	{
		Window::Clear(0,0,0);

		//Entities
		SelfRegister<SortedDrawable>::Sort([](SortedDrawable* a, SortedDrawable* b)
		{
			return a->pos.y < b->pos.y;
		});
		for (SortedDrawable* e : SelfRegister<SortedDrawable>::GetAll())
		{
			e->Draw();
		}

		Window::Draw(text, vec(120,100)).withScale(0.3f);

	}

};
