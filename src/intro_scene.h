#pragma once

#include <string>
#include <array>

#include "scene_manager.h"
#include "mates.h"
#include "input.h"
#include "doggo.h"
#include "assets.h"
#include "text.h"
#include "scene_jumpman.h"

extern float mainClock;

struct IntroScene : Scene {
	
	Text credits;
	Text text;
	RotoText rototext;

	IntroScene() : text(Assets::font_30, Assets::font_30_outline), credits(Assets::font_30, Assets::font_30_outline) { }

	void EnterScene() override
	{
		Assets::soundDoggo1.SetVolume(0.f);
		Assets::soundDoggo2.SetVolume(0.f);
		Assets::soundDoggo3.SetVolume(0.f);

		new Doggo();

		rototext.ShowMessage("Who let the doggos in?");
		rototext.Update(100);

		text.SetString("PRESS START");
		
		credits.SetString("A game by:\n\nAlbert Martinez\nAlbert Vaca Cintora\n\nMusic by:\n\nVeuskemini");

	}

	void ExitScene() override {
		Doggo::DeleteAll();
		Assets::soundDoggo1.SetVolume(1.f);
		Assets::soundDoggo2.SetVolume(1.f);
		Assets::soundDoggo3.SetVolume(1.f);
	}

	void Update(float dt) override {
		rototext.Update(dt);
		for (Doggo* doggo : Doggo::GetAll()) {
			doggo->Update(dt);
			if (doggo->wantFood) {
				doggo->anim.current_frame = (int(mainClock*1000)/200)%2;
				doggo->wantFood = false;
			}
		}

		for (int player = 0; player < Input::kMaxPlayers; ++player) {
			if (Input::IsJustPressed(player, GameKeys::START)) {
				SceneManager::ChangeScene(new JumpScene(player));
			}
		}
	}

	void Draw() override
	{
		Window::Clear(34, 32, 52);

		Window::Draw(Assets::casaTexture, vec(0,0));

		for (Doggo* doggo : Doggo::GetAll()) {
			doggo->Draw();
		}

		rototext.Draw();
		
		if ((int(mainClock*1000)/350)%2) {
			Window::Draw(text, Camera::Center()+vec(0, 90))
				.withScale(0.65f)
				.withOrigin(text.Size().x/2,0);
		}

		Window::Draw(credits, vec(15, 300)).withScale(0.39f, 0.45f);
	}

};
