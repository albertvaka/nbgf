#include <map>

#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <iostream>
#include <string>

#include "game_data.h"
#include "scene_manager.h"
#include "input.h"
#include "mates.h"
#include "intro_scene.h"
#include "scene_jumpman.h"
#include "main_scene.h"
#include "assets.h"

Scene* SceneManager::currentScene = nullptr;
sf::Clock mainClock;

#define _FPS_COUNTER

int main()
{

	sf::RenderWindow window(sf::VideoMode(GameData::WINDOW_WIDTH, GameData::WINDOW_HEIGHT), GameData::GAME_TITLE);

	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);
	Input::Init(window);

	Assets::LoadAll();

#ifdef _FPS_COUNTER
	sf::Text txt_fps;
	txt_fps.setFont(Assets::font);
	txt_fps.setPosition(10, 10);
	sf::Clock fpsClock;
	int fps_counter = 0;
	bool slowDown = false;
#endif

	sf::Clock dtClock;

	//Scene* currentScene = new IntroScene();
	Scene* currentScene = new JumpScene();
	SceneManager::SetScene(currentScene);
	currentScene->EnterScene();
	while (window.isOpen())
	{
		if (SceneManager::CurrentScene() != currentScene) {
			currentScene->ExitScene();
			delete currentScene;
			currentScene = SceneManager::CurrentScene();
			currentScene->EnterScene();
		}

		sf::Time time = dtClock.restart();

		Input::Update(time);

#ifdef _DEBUG
		static bool frameByFrame = false;
		if (Keyboard::IsKeyJustPressed(DEBUG_FRAME_BY_FRAME)) {
			frameByFrame = !frameByFrame;
		}

		static bool debugDraw = false;
		if (Keyboard::IsKeyJustPressed(DEBUG_BOUNDS)) {
			debugDraw = !debugDraw;
		}

		if (!frameByFrame || Keyboard::IsKeyJustPressed(DEBUG_FRAME_BY_FRAME_NEXT) || Keyboard::IsKeyJustPressed(RESTART))
#endif
		{
#ifdef _DEBUG
			ClearDebugVecs();
#endif
			int dt = time.asMilliseconds();
			if (dt> 60) // less than 17 FPS
			{
				dt = 60; //Slow game down instead of epic jumps
				slowDown = true;
			}
			currentScene->Update(dt);
		}


#ifdef _DEBUG
		currentScene->Draw(window, debugDraw);
		if (debugDraw) DrawDebugVecs(&window);
#else
		currentScene->Draw(window, false);
#endif

		Camera::StartGuiDraw();

#ifdef _FPS_COUNTER
		fps_counter++;
		if (fpsClock.getElapsedTime().asSeconds() > 0.5f)
		{
			txt_fps.setString(std::to_string(int(fps_counter / fpsClock.restart().asSeconds())) + (slowDown ? "!" : ""));
			slowDown = false;
			fps_counter = 0;
		}
		window.draw(txt_fps);
#endif

		ImGui::SFML::Render(window);
		Camera::EndGuiDraw();

		window.display();
	}

	ImGui::SFML::Shutdown();
}
