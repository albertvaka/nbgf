#include "scene_intro.h"

#include "scene_manager.h"
#include "scene_entrypoint.h"
#include "lava.h"
#include "text.h"
#include "bat.h"
#include "input.h"
#include "assets.h"
#include "assets_sounds.h"
#include "parallax.h"
#include "screen.h"
#include "window.h"
#include "drawall.h"

IntroScene::IntroScene()
	: bounds(0, 0, Window::GAME_WIDTH, Window::GAME_HEIGHT)
	, lava(BoxBounds(0, Window::GAME_HEIGHT - 20, Window::GAME_WIDTH, 200))
	, parallax(bounds, Assets::forestParallaxTextures, 0.f, 1.f, -142.1f)
	, titleText(Assets::font_30, Assets::font_30_outline)
	, startText(Assets::font_30)
	, creditsText(Assets::font_30)
{
	titleText.SetFillColor(0, 0, 0);
	titleText.SetOutlineColor(255, 255, 0);
	titleText.SetString("Recoil Rider");

	startText.SetFillColor(255, 255, 255);
	startText.SetString("- Press Start -");

	creditsText.SetFillColor(255, 255, 255);
	creditsText.SetString("A game by:\n\nAlbert Vaca Cintora");
}

void IntroScene::EnterScene()
{
	MusicPlayer::SetVolume(0.3f);
	MusicPlayer::Ensure(Assets::musicHellcross);
	BoxBounds batBounds = BoxBounds(0, 0, Window::GAME_WIDTH, Window::GAME_HEIGHT - 28);
	new Bat(Rand::VecInRange(batBounds), false, true, &batBounds);
}

void IntroScene::ExitScene()
{
	Bat::DeleteAll();
}

void IntroScene::Update(float dt)
{
	MusicPlayer::ToggleMusicWithM();
	if (Input::IsJustPressedAnyPlayer(GameKeys::START)) {
		if (Keyboard::IsKeyPressed(SDL_SCANCODE_RSHIFT) && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
			SceneManager::ChangeScene(new MainScene());
		}
		else {
			SceneManager::ChangeScene(new HellCrossScene());
		}
	}

	for (Lava* l : Lava::GetAll()) {
		l->Update(dt);
	}
	for (Bat* e : Bat::GetAll()) {
		e->Update(dt);
	}
}

void IntroScene::Draw()
{
	Window::Clear(31, 36, 50);

	DrawAllInOrder(
		&parallax,
		Bat::GetAll(),
		Lava::GetAll()
	);

	Window::Draw(titleText, vec(Camera::Center().x, Camera::Center().y - 30))
		.withOrigin(titleText.Size() / 2)
		.withScale(0.8f)
		.withRotationDegs(sin(mainClock)*15);

	bool blink = static_cast<int>(std::floor(mainClock / 0.5f)) % 2 == 0;
	if (blink) {
		Window::Draw(startText, vec(Camera::Center().x, Camera::Center().y + 60))
			.withOrigin(startText.Size() / 2)
			.withScale(0.3f);
	}

	/*
	Window::Draw(creditsText, vec(15, 205))
		.withScale(0.3f);
		*/
}
