#include <string>
#include <filesystem>

#include "scene_manager.h"
#include "input.h"
#include "mates.h"
#include "debug.h"
#include "text.h"
#include "debug.h"
#include "camera.h"

#include "../src/assets.h"
#include "../src/scene_entrypoint.h"

#ifdef _IMGUI
#include "imgui.h"
#include "imgui_impl_sdl.h"
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

Scene* currentScene;
Scene* SceneManager::currentScene = nullptr;

float mainClock;
int last_ticks;

bool slowDown = false;

#ifdef _FPS_COUNTER
Text* txt_fps;
int fps_counter = 0;
float fpsClock = 0.f;
#endif

#ifdef _DEBUG
void BeforeSceneUpdate();
void BeforeSceneDraw();
void AfterSceneDraw();
#endif

void init();
void main_loop();

int main(int argc, char* argv[])
{
#if !(MACOS_VER_MAJOR==10 && MACOS_VER_MINOR<=14)
	// set working dir to the binary path
	std::filesystem::path binaryPath(argv[0]);
	std::filesystem::current_path(binaryPath.parent_path());
#endif

	init();
	
#ifdef __EMSCRIPTEN__
  	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (true) {
		main_loop();
	}
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

	Input::Init();

#ifdef _IMGUI
	ImGui::CreateContext();
	ImGui_ImplSDL2_InitForOpenGL(Window::window, nullptr);
	ImGui_ImplOpenGL3_Init(nullptr);
#endif

	Assets::LoadAll();

	srand(time(NULL));

#ifdef _FPS_COUNTER
	txt_fps= new Text(Assets::font_30);
	txt_fps->SetString("0");
#endif

	last_ticks = SDL_GetTicks();

	// Start with the both buffers fully black
	Window::Clear(0, 0, 0);
	GPU_Flip(Window::screenTarget);
	Window::Clear(0, 0, 0);

	currentScene = new EntryPointScene();
	SceneManager::SetScene(currentScene);
	currentScene->EnterScene();
}


void main_loop() {

	if (SceneManager::CurrentScene() != currentScene) {
		currentScene->ExitScene();
		delete currentScene;
		currentScene = SceneManager::CurrentScene();
		currentScene->EnterScene();
	}

#ifdef _IMGUI
	//GPU_ActivateShaderProgram(0, NULL);
	GPU_FlushBlitBuffer(); // IMPORTANT: run GPU_FlushBlitBuffer before ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(Window::window);
	ImGui::NewFrame();
#endif

	int ticks = SDL_GetTicks();
	float dt = (ticks - last_ticks) / 1000.f;
	last_ticks = ticks;

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
	const SDL_Scancode DEBUG_RELOAD_ASSETS = SDL_SCANCODE_F4;
	const SDL_Scancode DEBUG_FAST_FORWARD = SDL_SCANCODE_F10;

	if (Keyboard::IsKeyJustPressed(DEBUG_RELOAD_ASSETS)) {
		// Leaks all already loaded assets, but we don't care since this is just for testing
		Assets::LoadAll();
		Debug::out << "Assets reloaded";
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_MODE)) {
		Debug::Draw = !Debug::Draw;
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_FRAME_BY_FRAME)) {
		Debug::FrameByFrame = !Debug::FrameByFrame;
	}

	if (Debug::FrameByFrame && Debug::Draw) {
		Camera::MoveCameraWithArrows(dt);
		Camera::ChangeZoomWithPlusAndMinus(dt);
		Camera::RotateWithPagUpDown(dt);
	}

	if (!Debug::FrameByFrame || Keyboard::IsKeyJustPressed(DEBUG_FRAME_BY_FRAME_NEXT))
#endif
	{
		float limited_dt = dt;
		if (limited_dt > 0.06f) // less than 17 FPS
		{
			limited_dt = 0.06f; //Slow game down instead of epic jumps
			slowDown = true;
		}

#ifdef _DEBUG
		if (Keyboard::IsKeyPressed(DEBUG_FAST_FORWARD)) {
			limited_dt *= 3;
		}

		BeforeSceneUpdate();
#endif

		mainClock += limited_dt; // TODO: Should not increase when fxmanager->IsTheWorldStopped()
		currentScene->Update(limited_dt);
	}

#ifdef _DEBUG
	BeforeSceneDraw();
#endif

	currentScene->Draw();

#ifdef _DEBUG
	AfterSceneDraw();
#endif

	//Draw GUI
	Camera::InScreenCoords::Begin();
	
#ifdef _FPS_COUNTER
	fps_counter++;
	fpsClock += dt;
	if (fpsClock > 0.5f)
	{
		txt_fps->SetString(std::to_string(int(fps_counter / fpsClock)) + (slowDown ? "!" : ""));
		slowDown = false;
		fps_counter = 0;
		fpsClock = 0;
	}
	Window::Draw(*txt_fps, Camera::InScreenCoords::Bounds().TopRight() + vec(-5, 5))
		.withOrigin(txt_fps->GetSize().x, 0)
		.withScale(0.5f);
#endif

#ifdef _IMGUI
	ImGui::Render();
	SDL_GL_MakeCurrent(Window::window, Window::screenTarget->context->context);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

	Camera::InScreenCoords::End();

	GPU_Flip(Window::screenTarget);
}

