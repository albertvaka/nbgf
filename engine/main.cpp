#include <string>
#include <filesystem>
#include <chrono>

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
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#endif

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#ifdef _DEBUG
#define _FPS_COUNTER
#endif

bool mainClockPaused = false;
float mainClock;
int lastTicks;

#ifdef _FPS_COUNTER
#include "text.h"
Text* fpsText;
int fpsCounter = 0;
float fpsClock = 0.f;
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
	while (true) {
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

#if _WIN32
#pragma comment(lib, "Shcore.lib")
#include <ShellScalingApi.h>
#endif

void init() {
#if _WIN32
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
#endif

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
		Debug::out << SDL_GetError();
		exit(1);
	}

	if (TTF_Init() != 0) {
		Debug::out << TTF_GetError();
	}

	if (Mix_Init(MIX_INIT_OGG) == 0) {
		Debug::out << Mix_GetError();
	}
	
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) != 0) {
		Debug::out << Mix_GetError();
	}

	if (Window::Init() != 0) {
		exit(1);
	}

	srand(std::chrono::system_clock::now().time_since_epoch().count());

	Assets::LoadAll();

	Input::Init();

#ifdef _IMGUI
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(Window::window, nullptr);
	ImGui_ImplOpenGL3_Init(nullptr);
#endif

#ifdef _FPS_COUNTER
	fpsText = new Text(Assets::font_30);
	fpsText->SetString("0");
#endif

	lastTicks = SDL_GetTicks();
}

void main_loop() {

#ifdef _IMGUI
	//GPU_ActivateShaderProgram(0, NULL);
	GPU_FlushBlitBuffer(); // IMPORTANT: run GPU_FlushBlitBuffer before ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
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
		Camera::SetZoom(1.f);
		Camera::SetTopLeft(0,0);
		Input::IgnoreInput(false);
		SceneManager::currentScene->EnterScene();
	}

	int ticks = SDL_GetTicks();
	float uncappedDt = (ticks - lastTicks) / 1000.f;
	lastTicks = ticks;

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
	Mouse::scrollWheel = 0.f;
	Window::ProcessEvents();
#ifdef _IMGUI
	ImGuiIO& io = ImGui::GetIO();
	if (!io.WantCaptureKeyboard)
#endif
	{
		Keyboard::_UpdateInputState();

	}
#ifdef _IMGUI
	if (!io.WantCaptureMouse)
#endif
	{
		Mouse::_UpdateInputState();
	}
	GamePad::_UpdateInputState();
	Input::Update(dt);

	if (Keyboard::IsKeyJustPressed(SDL_SCANCODE_RETURN) && Keyboard::IsKeyPressed(SDL_SCANCODE_LALT)) {
		Window::SetFullScreen(!Window::IsFullScreen());
	}

#ifdef _DEBUG
	const SDL_Scancode DEBUG_FRAME_BY_FRAME = SDL_SCANCODE_F1;
	const SDL_Scancode DEBUG_FRAME_BY_FRAME_NEXT = SDL_SCANCODE_E;
	const SDL_Scancode DEBUG_MODE = SDL_SCANCODE_F2;
	const SDL_Scancode FIXED_CAMERA = SDL_SCANCODE_F3;
	const SDL_Scancode DEBUG_RELOAD_ASSETS = SDL_SCANCODE_F4;
	const SDL_Scancode DEBUG_RESTART_SCENE = SDL_SCANCODE_F5;
	const SDL_Scancode DEBUG_FAST_FORWARD = SDL_SCANCODE_F10;

	if (Keyboard::IsKeyJustPressed(DEBUG_RELOAD_ASSETS)) {
		// Leaks all already loaded assets, but we don't care since this is just for testing
		Assets::LoadAll();
		Debug::out << "Assets reloaded";
	}

	if (Keyboard::IsKeyJustPressed(FIXED_CAMERA)) {
		Debug::CameraFixed = !Debug::CameraFixed;
		fixedCamera = Camera::Center();
		fixedZoom = Camera::Zoom();
		fixedRotationDegs = Camera::GetRotationDegs();
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_MODE)) {
		Debug::Draw = !Debug::Draw;
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_FRAME_BY_FRAME)) {
		Debug::FrameByFrame = !Debug::FrameByFrame;
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_FAST_FORWARD) && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		Debug::FastForward = !Debug::FastForward;
	} else if (Keyboard::IsKeyJustReleased(DEBUG_FAST_FORWARD) && !Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
		Debug::FastForward = false;
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_RESTART_SCENE)) {
		SceneManager::RestartScene();
	}

	if (Debug::CameraFixed) {
		 Camera::SetCenter(fixedCamera);
		 Camera::SetZoom(fixedZoom);
		 Camera::SetRotationDegs(fixedRotationDegs);
	}

	if (Debug::FrameByFrame && Debug::Draw) {
		float vel = Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT) ? 2.f : 1.f;
		Camera::MoveCameraWithArrows(vel*dt);
		Camera::ChangeZoomWithPlusAndMinus(vel* dt);
		Camera::RotateWithPagUpDown(vel* dt);
		fixedCamera = Camera::Center();
		fixedZoom = Camera::Zoom();
		fixedRotationDegs = Camera::GetRotationDegs();
	}
	if (Debug::FastForward || Keyboard::IsKeyPressed(DEBUG_FAST_FORWARD)) {
		dt = kMinDt;
	}

	if (!Debug::FrameByFrame || Keyboard::IsKeyJustPressed(DEBUG_FRAME_BY_FRAME_NEXT))
	{
		BeforeSceneUpdate();
#endif
		if (!mainClockPaused) {
			mainClock += dt;
		}
		SceneManager::currentScene->Update(dt);
#ifdef _DEBUG
	}

	if (Debug::CameraFixed) {
		Camera::SetCenter(fixedCamera);
		Camera::SetZoom(fixedZoom);
		Camera::SetRotationDegs(fixedRotationDegs);
	}

	BeforeSceneDraw();
#endif

	SceneManager::currentScene->Draw();

#ifdef _DEBUG
	AfterSceneDraw();
#endif

	//Draw GUI
	Camera::InScreenCoords::Begin();
	
#ifdef _FPS_COUNTER
	fpsCounter++;
	fpsClock += uncappedDt;
	if (fpsClock > 0.5f)
	{
		fpsText->SetString(std::to_string(int(fpsCounter / fpsClock)) + (slowDown ? "!" : ""));
		slowDown = false;
		fpsCounter = 0;
		fpsClock = 0;
	}
	Window::Draw(*fpsText, Camera::InScreenCoords::Bounds().TopRight() + vec(-5, 5))
		.withOrigin(fpsText->Size().x, 0)
		.withScale(0.5f);
#endif

#ifdef _IMGUI
	ImGui::Render();
	SDL_GL_MakeCurrent(Window::window, Window::screenTarget->context->context);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

#ifdef _DEBUG
	if (Debug::CameraFixed) {
		Window::DrawPrimitive::Rectangle(5.f, 5.f, 10.f, 10.f, -1, 0, 255, 0);
	}
	if (mainClockPaused) {
		Window::DrawPrimitive::Circle(5, 5, 3, -1, 255, 0, 0);
	}
#endif

	Camera::InScreenCoords::End();

	GPU_Flip(Window::screenTarget);
}
