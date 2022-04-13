#pragma once

#include "scene.h"
#include "SDL_assert.h"

struct SceneManager
{
	static inline Scene* newScene = nullptr;
	static inline Scene* currentScene = nullptr;
	static void ChangeScene(Scene* s)
	{
		SDL_assert(!newScene);
		newScene = s;
	}
	static void RestartScene()
	{
		SDL_assert(!newScene);
		newScene = currentScene;
	}
};

