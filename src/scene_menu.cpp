#include "scene_menu.h"
#include "raw_input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "assets.h"
#include "debug.h"

SceneMenu::SceneMenu()
{
}

void SceneMenu::EnterScene() 
{
}


void SceneMenu::ExitScene()
{
}

void SceneMenu::Update(float dt)
{

#ifdef _DEBUG
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		ExitScene();
		EnterScene();
		return;
	}
#endif
}

void SceneMenu::Draw() {
	Window::Clear();

}
