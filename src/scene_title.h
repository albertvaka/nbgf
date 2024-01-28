#pragma once

#include "scene.h"
#include "assets.h"
#include "vec.h"
#include "text.h"
#include "musicplayer.h"
#include "text.h"
#include "input.h"
#include "scene_manager.h"
#include "scene_main.h"

extern float mainClock;

struct SceneTitle : Scene {

	Text startText;
	Text creditsText;

	SceneTitle()
		: startText(Assets::font_30)
		, creditsText(Assets::font_30)
	{
		startText.SetFillColor(12, 12, 12);
		startText.SetString("- Press Start -");

		creditsText.SetFillColor(12, 12, 12);
		creditsText.SetString("A game by:\n\nJohanna Krueger\nAlbert Vaca Cintora\n\nGame music by:\n\nKevin MacLeod");
	}

	void EnterScene() override
	{
		Camera::SetZoom(0.5f, false);
		MusicPlayer::SetVolume(40);
		MusicPlayer::Play(Assets::menuMusic);
	}

	void ExitScene() override
	{
	}

	void Update(float dt) override
	{
		MusicPlayer::ToggleMusicWithM();
		if (Input::IsJustPressedAnyPlayer(GameKeys::START)) {
			SceneManager::ChangeScene(new SceneMain());
		}
	}
	void Draw() override
	{
		Window::Clear(249, 249, 249);

		Window::Draw(Assets::titleTexture, vec(0, 0));

		Camera::InScreenCoords::Begin();

		bool blink = static_cast<int>(std::floor(mainClock / 0.5f)) % 2 == 0;
		if (blink) {
			Window::Draw(startText, vec(Camera::InScreenCoords::Center().x, Camera::InScreenCoords::Center().y + 150))
				.withOrigin(startText.Size() / 2)
				.withScale(0.7f, 0.8f);
		}

		Window::Draw(creditsText, vec(25, 495))
			.withScale(0.66f);

		Camera::InScreenCoords::End();

	}

};