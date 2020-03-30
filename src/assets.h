#pragma once

#include <iostream>
#include <vector>

#include <SFML/Graphics.hpp>

struct Assets
{
	static void LoadAll();

	static sf::Texture hospitalTexture;
	static sf::Sprite hospitalSprite;

	static sf::Texture marioTexture;
	static sf::Sprite marioSprite;

	static sf::Font font;

	static sf::Shader tintShader;

};

