#pragma once

#include "scene.h"
#include "assets.h"
#include "vec.h"
#include "text.h"
#include "musicplayer.h"
#include "text.h"
#include "fx.h"
#include "input.h"
#include "scene_manager.h"
#include "scene_main.h"

extern float mainClock;

struct SceneTitle : Scene {

	Text startText;
	Text creditsText;

	SceneTitle()
		: startText(Assets::funk_30)
		, creditsText(Assets::funk_30)
	{
		startText.SetFillColor(255, 255, 255);
		startText.SetString("- Press Start -");

		creditsText.SetFillColor(255, 255, 255);
		creditsText.SetMultilineAlignment(Text::MultilineAlignment::CENTER);
		creditsText.SetString("A game by:\n\nJohanna Krueger\nAlbert Vaca Cintora\n\nGame music by:\n\nKevin MacLeod");
	}

	void EnterScene() override
	{
	}

	void ExitScene() override
	{
	}

	void Update(float dt) override
	{
		MusicPlayer::ToggleMusicWithM();
		if (!Keyboard::IsKeyPressed(SDL_SCANCODE_LALT) && Input::IsJustPressedAnyPlayer(GameKeys::START)) {
			SceneManager::ChangeScene(new SceneMain());
		}
	}
	void Draw() override
	{
		Window::Clear(249, 249, 249);

		Window::Draw(Assets::titleTexture, BoxBounds(vec(0, 0), vec(Window::GAME_WIDTH, Window::GAME_HEIGHT)));

		Camera::InScreenCoords::Begin();

		bool blink = static_cast<int>(std::floor(mainClock / 0.5f)) % 2 == 0;
		if (blink) {
			Window::Draw(startText, vec(Camera::InScreenCoords::Center().x, Camera::InScreenCoords::Center().y + 60))
				.withColor(149, 222, 229)
				.withScale(0.9f)
				.withOrigin(startText.Size() / 2);
		}

		Window::Draw(creditsText, Camera::InScreenCoords::Center() + vec(0,270))
			.withOriginCentered()
			.withColor(149, 222, 229)
			.withScale(0.5f);

		Camera::InScreenCoords::End();

	}

};