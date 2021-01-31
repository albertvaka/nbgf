#include "scene_menu.h"
#include "input.h"
#include "scene_manager.h"
#include "scene_main.h"
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
	if (Input::IsPressedAnyPlayer(GameKeys::START)) {
		SceneManager::ChangeScene(new SceneMain());
	}
}

void SceneMenu::Draw() {
	Window::Clear();

}
