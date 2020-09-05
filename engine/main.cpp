#include <string>

#include "scene_manager.h"
#include "input.h"
#include "mates.h"
#include "debug.h"
#include "camera.h"

#include "../src/assets.h"
#include "../src/scene_jumpman.h"
#include "../src/scene_hellcross.h"

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

#define _FPS_COUNTER

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

void init();
void main_loop();

int main(int argc, char* argv[])
{
	init();
	
#ifdef __EMSCRIPTEN__
  	emscripten_set_main_loop(main_loop, 0, 1);
#else
	while (true) {
		main_loop();
	}
#endif

}

#if __WIN32__
#pragma comment(lib, "Shcore.lib")
#include <ShellScalingApi.h>
#endif

void init() {
#if __WIN32__
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

	//currentScene = new HellCrossScene();
	currentScene = new JumpScene();
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


#ifdef _DEBUG
	const SDL_Scancode DEBUG_FRAME_BY_FRAME = SDL_SCANCODE_F1;
	const SDL_Scancode DEBUG_FRAME_BY_FRAME_NEXT = SDL_SCANCODE_E;
	const SDL_Scancode DEBUG_MODE = SDL_SCANCODE_F2;
	const SDL_Scancode DEBUG_RELOAD_ASSETS = SDL_SCANCODE_F4;

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
		Camera::MoveCameraWithArrows(50.f, dt);
		Camera::ChangeZoomWithPlusAndMinus(1.f, dt);
	}

	if (!Debug::FrameByFrame || Keyboard::IsKeyJustPressed(DEBUG_FRAME_BY_FRAME_NEXT))
#endif
	{
#ifdef _DEBUG
		ClearDebugVecs();
#endif

		float limited_dt = dt;
		if (limited_dt > 0.06f) // less than 17 FPS
		{
			limited_dt = 0.06f; //Slow game down instead of epic jumps
			slowDown = true;
		}
		mainClock += limited_dt;
		currentScene->Update(limited_dt);
	}

	currentScene->Draw();

	//(Camera::GetBounds() * 0.99f).Draw();

#ifdef _DEBUG
	if (Debug::Draw) {
		DrawDebugVecs();
	}
#endif

	//Draw GUI
	Camera::GUI::Begin();
	
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
	Window::Draw(*txt_fps, Camera::GUI::GetBounds().TopRight() + vec(-5, 5))
		.withOrigin(txt_fps->GetSize().x, 0)
		.withScale(0.5f);
#endif

#ifdef _IMGUI
	ImGui::Render();
	SDL_GL_MakeCurrent(Window::window, Window::screenTarget->context->context);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif

	Camera::GUI::End();

	GPU_Flip(Window::screenTarget);
}

