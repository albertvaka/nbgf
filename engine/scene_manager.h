#pragma once

#include "scene.h"
#include <cassert>

struct SceneManager
{
	static inline Scene* newScene = nullptr;
	static inline Scene* currentScene;
	static void ChangeScene(Scene* s)
	{
		assert(!newScene);
		newScene = s;
	}
	static void RestartScene()
	{
		assert(!newScene);
		newScene = currentScene;
	}
};

