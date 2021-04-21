#pragma once

#include "input.h"
#include "anim_lib.h"
#include "scene_manager.h"
#include "scene_main.h"
#include "assets.h"
#include "debug.h"
#include "scene.h"
#include "text.h"
#include "backgroundelement.h"

extern float mainClock;

struct MenuScene : Scene {

	Text timerText;
	Text startText;
	Text creditsText;

	MenuScene()
		: timerText(Assets::font_30, Assets::font_30_outline)
		, startText(Assets::font_30)
		, creditsText(Assets::font_30)
	{
		timerText.SetFillColor(0, 0, 0);
		timerText.SetOutlineColor(255, 255, 0);
		timerText.SetString("EVIL MUTANT NINJA\nALIENS FROM SPACE");

		startText.SetFillColor(255, 255, 0);
		startText.SetString("- Press Start -");

		creditsText.SetFillColor(255, 255, 0);
		creditsText.SetString("A game by:\n\nAlbert Martinez\nAlbert Vaca Cintora\n\nGame music by:\n\nKevin MacLeod");
	}

	void EnterScene() override 
	{ 
		for (int i = 0; i < BackgroundElement::kNumBackgroundElements; i++) {
			new BackgroundElement();
		}
		MusicPlayer::Play(Assets::menuMusic);
	}
	void ExitScene() override
	{
		BackgroundElement::DeleteAll();
	}

	void Update(float dt) override 
	{
		MusicPlayer::ToggleMusicWithM();
		if (Input::IsJustPressedAnyPlayer(GameKeys::START)) {
			SceneManager::ChangeScene(new MainScene());
		}
		for (BackgroundElement* a : BackgroundElement::GetAll()) {
			a->Update(dt);
		}
	}
	void Draw() override 
	{
		Window::Clear(15, 15, 15);
		for (const BackgroundElement* a : BackgroundElement::GetAll()) {
			a->Draw();
		}

		Window::Draw(timerText, vec(Camera::Center().x, Camera::Center().y - 30))
			.withOrigin(timerText.Size() / 2)
			.withScale(0.6f)
			.withRotationDegs(Mates::map(sin(mainClock), -1, 1, -18, 18));

		bool blink = static_cast<int>(std::floor(mainClock / 0.5f)) % 2 == 0;
		if (blink) {
			Window::Draw(startText, vec(Camera::Center().x, Camera::Center().y + 50))
				.withOrigin(startText.Size() / 2)
				.withScale(0.3f);
		}

		Window::Draw(creditsText, vec(15, 205))
			.withScale(0.3f);
	}

};

