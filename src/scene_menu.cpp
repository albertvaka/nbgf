#include "scene_menu.h"
#include "input.h"
#include "anim_lib.h"
#include "scene_manager.h"
#include "scene_main.h"
#include "assets.h"
#include "debug.h"

SceneMenu::SceneMenu()
	: overanim(AnimLib::OVERSEER)
	, npcanim(AnimLib::NPC_1_LEFT)
{
	Camera::SetCenter(0,0);
	Camera::SetZoom(1.f);
}

void SceneMenu::EnterScene() 
{
}


void SceneMenu::ExitScene()
{
}

void SceneMenu::Update(float dt)
{
	overanim.Update(dt);
	npcanim.Update(dt);
	if (Input::IsPressedAnyPlayer(GameKeys::START)) {
		SceneManager::ChangeScene(new SceneMain());
	}
}

void SceneMenu::Draw() {

	Window::Clear(0, 5, 20);
	
	Window::Draw(Assets::splashTexture, Camera::Bounds());

	Window::Draw(Assets::npcTexture, vec(1190,200))
		.withRect(npcanim.CurrentFrameRect())
		.withScale(0.4f);

	Window::Draw(Assets::overseerTexture, vec(110,200))
		.withRect(overanim.CurrentFrameRect())
		.withScale(0.4f);
}
