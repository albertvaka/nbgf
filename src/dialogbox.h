#pragma once

#include "SDL_gpu.h"
#include "text.h"
#include "assets.h"
#include "appearingtext.h"

extern float mainClock;

struct DialogCharacter {
	std::string name;
	GPU_Rect portrait;
	const Voice& voice;
};

struct DialogBox
{
	Text title;
	AppearingText body;
	GPU_Rect currentPortrait;
	const Voice* currentVoice;
	int voiceChannel = -1;
	float openCloseTimer;
	bool isOpen = false;
	bool isLast; // wether to show a "close" icon at the end instead of a "next" icon
	
	DialogBox();

	void ShowMessage(const DialogCharacter& character, const std::string& msg, bool isLast)
	{
		currentVoice = &character.voice;
		currentPortrait = character.portrait;
		this->isLast = isLast;
		title.SetString(character.name);
		body.ShowMessage(msg);
		if (voiceChannel >= 0)
		{
			Sound::Stop(voiceChannel);
			voiceChannel = -1;
		}
		if (!isOpen) {
			isOpen = true;
			openCloseTimer = 0;
		}
		else
		{
			voiceChannel = currentVoice->speak.Play();
		}
	}

	int selectedChoice;
	std::vector<Text> choices;

	void AddChoices(const std::vector<std::string>& choices)
	{
		selectedChoice = 0;
		for (const std::string& text : choices) {
			Text choice(Assets::font_dialog_body);
			choice.SetString(text);
			this->choices.push_back(choice);
		}
	}

	int GetSelectedChoice() const {
		return selectedChoice;
	}

	void Close()
	{
		isOpen = false;
		openCloseTimer = 0;
	}

	bool IsOpen() const;

	bool Update(float dt); // returns true when the text is fully displayed and the user presses action to close it

	void Draw() const;
};


