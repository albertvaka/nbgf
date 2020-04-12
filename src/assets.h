#pragma once

#include <SFML/Graphics.hpp>
#include <array>

struct Assets
{
	static void LoadAll();

	static sf::Texture hospitalTexture;
	static sf::Sprite hospitalSprite;

	static sf::Texture marioTexture;
	static sf::Sprite marioSprite;

	static sf::Font font;

	static sf::Shader tintShader;

	static std::array<sf::Texture, 4> forestParallaxTextures;

};

