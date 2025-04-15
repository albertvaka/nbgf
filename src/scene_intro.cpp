#include "scene_intro.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "window.h"
#include "camera.h"
#include "musicplayer.h"
#include "input.h"
#include "scene_manager.h"
#include "scene_main.h"
#include "debug.h"
#include <cmath>

extern float mainClock;

extern float kZoomSailingSlow;
extern vec kShipSpawnPos;

SceneIntro::SceneIntro()
	: titleText(Assets::font_30, Assets::font_30_outline)
	, startText(Assets::font_30)
	, creditsText(Assets::font_30)
{

	titleText.SetFillColor(0, 0, 0);
	titleText.SetOutlineColor(255, 255, 255);
	titleText.SetString("SHIP GAEM");

	startText.SetFillColor(255, 255, 255);
	startText.SetString("- Press Start -");

	creditsText.SetFillColor(255, 255, 255);
	creditsText.SetString("A game by:\n\nAlbert Vaca Cintora");
}

void SceneIntro::EnterScene()
{
	Camera::SetZoom(kZoomSailingSlow);
	Camera::SetCenter(kShipSpawnPos);
	Debug::DebugDrawScale = 3.f;
}

void SceneIntro::ExitScene()
{
}

void SceneIntro::Update(float dt)
{
	MusicPlayer::ToggleMusicWithM();
	if (Input::IsJustPressedAnyPlayer(GameKeys::START)) {
		Window::SetFullScreen(true);
		SceneManager::newScene = new SceneMain();
	}
}

void SceneIntro::Draw()
{
	Window::Clear(0.1 * 255, 0.45 * 255, 0.73 * 255);

	Assets::seaShader.Activate();
	Assets::seaShader.SetUniform("offset", Camera::Center());
	Assets::seaShader.SetUniform("iResolution", Window::GetViewportScaledResolution());
	Assets::seaShader.SetUniform("windowScale", Window::GetViewportScale());
	Assets::seaShader.SetUniform("zoom", Camera::Zoom());
	Assets::seaShader.SetUniform("iTime", mainClock);
	Window::Draw(Assets::rockTexture, Camera::Bounds()); // Texture unused by the shader
	Assets::seaShader.Deactivate();

	Camera::InScreenCoords::Begin();
	Window::Draw(titleText, vec(Camera::InScreenCoords::Center().x, Camera::InScreenCoords::Center().y - 80))
		.withOrigin(titleText.Size() / 2)
		.withRotationDegs(sin(mainClock)*15);

	bool blink = static_cast<int>(std::floor(mainClock / 0.5f)) % 2 == 0;
	if (blink) {
		Window::Draw(startText, vec(Camera::InScreenCoords::Center().x, Camera::InScreenCoords::Center().y + 80))
			.withOrigin(startText.Size() / 2)
			.withScale(0.6f);
	}
	Camera::InScreenCoords::End();
}
