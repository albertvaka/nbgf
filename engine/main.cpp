#include <string>
#include <filesystem>
#include <chrono>

#include <raylib.h>

#include "scene_manager.h"
#include "input.h"
#include "mates.h"
#include "debug.h"
#include "debug.h"
#include "camera.h"
#include "window.h"

#include "../src/assets.h"
#include "../src/scene_entrypoint.h"

#ifdef _IMGUI
#include "rlImGui.h"
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

//#ifdef _DEBUG
#define _FPS_COUNTER
//#endif

bool mainClockPaused = false;
float mainClock;

#ifdef _FPS_COUNTER
int fpsCounter = 0;
float fpsClock = 0.f;
std::string fpsText;
#endif

#ifdef _DEBUG
vec fixedCamera;
float fixedZoom;
float fixedRotationDegs;
void BeforeSceneUpdate();
void BeforeSceneDraw();
void AfterSceneDraw();
#endif

void init();
void main_loop();

extern "C" void start_main_loop()
{
	// We wait until here to create the scene so the emscripten FS
	// emscripten is ready, since the scene constructor could use it
	SceneManager::newScene = new EntryPointScene();

#ifdef __EMSCRIPTEN__
	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (!WindowShouldClose()) {
		main_loop();
	}
#endif
}

int main(int argc, char* argv[])
{
	// set working dir to the binary path
	std::filesystem::path binaryPath(argv[0]);
	std::filesystem::current_path(binaryPath.parent_path());

	init();

#ifdef __EMSCRIPTEN__
	// mounting the FS is async, wait for it before we launch the main loop
	EM_ASM(
		FS.mkdir('/saves');
		FS.mount(IDBFS, {}, '/saves');
		FS.syncfs(true, function (err) {
			console.log(err ? err : "emscripten: /saves directory mounted");
			try {
				ccall('start_main_loop', null);
			} catch(e) {
				// https://github.com/emscripten-core/emscripten/issues/12814
				if (e != 'unwind') throw e;
			}
		});
	);
#else
	start_main_loop();
#endif

}

void init() {

	if (Window::Init() != 0) {
		exit(1);
	}

	srand(std::chrono::system_clock::now().time_since_epoch().count());

	Assets::LoadAll();

	Input::Init();

#ifdef _IMGUI
	rlImGuiSetup(true);
#endif

}

void main_loop() {

	BeginDrawing();

#ifdef _IMGUI
	rlImGuiBegin();
#endif

	if (SceneManager::newScene != nullptr) {
		if (SceneManager::currentScene == nullptr) {
			// This branch is only taken the first time. We have this here so
			// the first scene "transition" is as similar to the rest as possible
			SceneManager::currentScene = SceneManager::newScene;
		}
		else {
			SceneManager::currentScene->ExitScene();
			if (SceneManager::currentScene != SceneManager::newScene) {
				delete SceneManager::currentScene;
				SceneManager::currentScene = SceneManager::newScene;
			}
		}

		SceneManager::newScene = nullptr;
		GameCamera::SetZoom(1.f);
		GameCamera::SetTopLeft(0,0);
		Input::IgnoreInput(false);
		SceneManager::currentScene->EnterScene();
	}

	float uncappedDt = GetFrameTime();

	float dt = uncappedDt;
	bool slowDown = false;
	constexpr float kMinDt = 0.06f; // less than 17 FPS
	if (uncappedDt > kMinDt)
	{
		//Slow game down instead of epic jumps
		dt = kMinDt;
		slowDown = true;
	}

	//Input
	/*
#ifdef _IMGUI
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard)
#endif
	{
		//Keyboard::_UpdateInputState();

	}
#ifdef _IMGUI
	if (!io.WantCaptureMouse)
#endif
	{
		//Mouse::_UpdateInputState();
	}
	*/

	Input::Update(dt);

	if (IsKeyPressed(KeyboardKey::KEY_ENTER) && IsKeyDown(KeyboardKey::KEY_LEFT_ALT)) {
		Window::SetFullScreen(!Window::IsFullScreen());
	}

#ifdef _DEBUG
	const KeyboardKey DEBUG_FRAME_BY_FRAME = KeyboardKey::KEY_F1;
	const KeyboardKey DEBUG_FRAME_BY_FRAME_NEXT = KeyboardKey::KEY_E;
	const KeyboardKey DEBUG_MODE = KeyboardKey::KEY_F2;
	const KeyboardKey FIXED_CAMERA = KeyboardKey::KEY_F3;
	const KeyboardKey DEBUG_RELOAD_ASSETS = KeyboardKey::KEY_F4;
	const KeyboardKey DEBUG_RESTART_SCENE = KeyboardKey::KEY_F5;
	const KeyboardKey DEBUG_FAST_FORWARD = KeyboardKey::KEY_F10;

	if (IsKeyPressed(DEBUG_RELOAD_ASSETS)) {
		// Leaks all already loaded assets, but we don't care since this is just for testing
		Assets::LoadAll();
		Debug::out << "Assets reloaded";
	}

	if (IsKeyPressed(FIXED_CAMERA)) {
		Debug::CameraFixed = !Debug::CameraFixed;
		fixedCamera = GameCamera::Center();
		fixedZoom = GameCamera::Zoom();
		fixedRotationDegs = GameCamera::GetRotationDegs();
	}

	if (IsKeyPressed(DEBUG_MODE)) {
		Debug::Draw = !Debug::Draw;
	}

	if (IsKeyPressed(DEBUG_FRAME_BY_FRAME)) {
		Debug::FrameByFrame = !Debug::FrameByFrame;
	}

	if (IsKeyPressed(DEBUG_FAST_FORWARD) && IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT)) {
		Debug::FastForward = !Debug::FastForward;
	} else if (IsKeyReleased(DEBUG_FAST_FORWARD) && !IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT)) {
		Debug::FastForward = false;
	}

	if (IsKeyPressed(DEBUG_RESTART_SCENE)) {
		SceneManager::RestartScene();
	}

	if (Debug::CameraFixed) {
		 GameCamera::SetCenter(fixedCamera);
		 GameCamera::SetZoom(fixedZoom);
		 GameCamera::SetRotationDegs(fixedRotationDegs);
	}

	if (Debug::FrameByFrame && Debug::Draw) {
		float vel = IsKeyDown(KeyboardKey::KEY_LEFT_SHIFT) ? 2.f : 1.f;
		GameCamera::MoveCameraWithArrows(vel*dt);
		GameCamera::ChangeZoomWithPlusAndMinus(vel* dt);
		GameCamera::RotateWithPagUpDown(vel* dt);
		fixedCamera = GameCamera::Center();
		fixedZoom = GameCamera::Zoom();
		fixedRotationDegs = GameCamera::GetRotationDegs();
	}
	if (Debug::FastForward || IsKeyDown(DEBUG_FAST_FORWARD)) {
		dt = kMinDt;
	}

	if (!Debug::FrameByFrame || IsKeyPressed(DEBUG_FRAME_BY_FRAME_NEXT))
	{
		BeforeSceneUpdate();
#endif
		if (!mainClockPaused) {
			mainClock += dt;
		}
		SceneManager::currentScene->Update(dt);
#ifdef _DEBUG
	}

	BeginMode2D(GameCamera::camera);

	if (Debug::CameraFixed) {
		GameCamera::SetCenter(fixedCamera);
		GameCamera::SetZoom(fixedZoom);
		GameCamera::SetRotationDegs(fixedRotationDegs);
	}

	BeforeSceneDraw();
#endif

	SceneManager::currentScene->Draw();

#ifdef _DEBUG
	AfterSceneDraw();
#endif
	EndMode2D();
	
	//Draw GUI
	BeginMode2D(GuiCamera::gui_camera);
	
#ifdef _FPS_COUNTER
	fpsCounter++;
	fpsClock += uncappedDt;
	if (fpsClock > 0.5f)
	{
		// Raylib GetFPS starts at thousands for some reason, so I'm calculating it myself
		fpsText = std::to_string(int(fpsCounter / fpsClock)) + (slowDown ? "!" : "");
		slowDown = false;
		fpsCounter = 0;
		fpsClock = 0;
	}
	// Draw current FPS
	vec pos = GuiCamera::Bounds().TopRight() + vec(-50, 5);
	DrawText(fpsText.c_str(), pos.x, pos.y, 30, WHITE);
#endif

#ifdef _IMGUI
	rlImGuiEnd();
#endif

#ifdef _DEBUG
	if (Debug::CameraFixed) {
		Window::DrawPrimitive::Rectangle(5.f, 5.f, 10.f, 10.f, -1, 0, 255, 0);
	}
	if (mainClockPaused) {
		Window::DrawPrimitive::Circle(5, 5, 3, -1, 255, 0, 0);
	}
#endif

	EndMode2D();

	EndDrawing();

}
