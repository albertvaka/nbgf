#pragma once

#include "scene.h"

struct SceneManager
{
	static Scene* currentScene;
	static Scene*& CurrentScene()
	{
		return currentScene;
	}
	static void SetScene(Scene* s)
	{
		currentScene = s;
	}
};

