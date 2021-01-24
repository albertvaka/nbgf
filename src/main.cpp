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
#include "main_scene.h"

Scene* SceneManager::currentScene = nullptr;
sf::Clock mainClock;

int main()
{

	sf::RenderWindow window(sf::VideoMode(GameData::WINDOW_WIDTH, GameData::WINDOW_HEIGHT), GameData::GAME_TITLE);

	window.setFramerateLimit(60);
	ImGui::SFML::Init(window);
	Input::Init(window);

#ifdef _DEBUG
	sf::Font font;
	font.loadFromFile("data/PressStart2P.ttf");
	sf::Text txt_fps;
	txt_fps.setFont(font);
	txt_fps.setPosition(10, 10);
	sf::Clock fpsClock;
	int fps_counter = 0;
#endif

	sf::Clock dtClock;

	Scene* currentScene = new IntroScene();
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

		int dt = time.asMilliseconds();
		Mates::ClampMax(dt, 90); //Slow game down instead of epic jumps

		currentScene->Update(dt);
		currentScene->Draw(window);

		Camera::StartGuiDraw();

#ifdef _DEBUG
		fps_counter++;
		if (fpsClock.getElapsedTime().asSeconds() > 0.5f)
		{
			txt_fps.setString(std::to_string(int(fps_counter / fpsClock.restart().asSeconds())));
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
