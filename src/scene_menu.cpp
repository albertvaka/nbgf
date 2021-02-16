#include "scene_menu.h"
#include "input.h"
#include "anim_lib.h"
#include "scene_manager.h"
#include "scene_main.h"
#include "assets.h"
#include "debug.h"

float timermenu = 0;

SceneMenu::SceneMenu()
	: overanim(AnimLib::OVERSEER)
	, npcanim(AnimLib::NPC_1_LEFT)
{
	Camera::SetZoom(1.f);
	Camera::SetTopLeft(0,0);
}

void SceneMenu::EnterScene() 
{
}


void SceneMenu::ExitScene()
{
}

void SceneMenu::Update(float dt)
{
	timermenu+=dt;
	overanim.Update(dt);
	npcanim.Update(dt);
	if (timermenu > 2.f && Input::IsJustPressedAnyPlayer(GameKeys::START)) {
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
