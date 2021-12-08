#pragma once

#include "window_draw.h"
#include "text.h"

struct AppearingText : Text
{
	int index = 0;
	float timer = 0;
	float maxLineWidth;
	std::string targetString = "";

	const float SPEED = 0.05f;

	AppearingText(float maxLineWidth, TTF_Font* font, TTF_Font* font_outline = nullptr) 
		: Text(font, font_outline)
		, maxLineWidth(maxLineWidth)
	{
		SetFillColor(255,255,255);
		SetOutlineColor(0,0,0);
		SetMultilineAlignment(MultilineAlignment::LEFT);
	}
	
	std::string AddLineBreaksToFitWidth(const std::string& msg) {
		std::string oldStr = GetString();
		std::stringstream ss;
		std::stringstream currentLine;
		int wordBegin = 0;
		for (int i = 0; i <= msg.size(); i++) {
			if (i == msg.size() || msg[i] == ' ') {
				//footgun avoided: the second parameter is the size and not the end pos
				std::string nextWord = msg.substr(wordBegin, i-wordBegin);
				i++;
				wordBegin = i;
				std::string oldCurrentLine = currentLine.str();
				if (!oldCurrentLine.empty()) {
					currentLine << " ";
				}
				currentLine << nextWord;
				SetString(currentLine.str());
				float lineWidth = Size().x;
				if (lineWidth > maxLineWidth) {
					if (!oldCurrentLine.empty()) { // fix for words that don't fit in maxLineWidth
						ss << oldCurrentLine << '\n';
					}
					//footgun avoided: stringstream::clear() exists but doesn't do what you expect,
					//this is the right way to clear it. 
					//bonus footgun: using ::str(nextWord) didn't work either.
					currentLine.str(std::string());
					currentLine << nextWord;
				}
			}
		}
		ss << currentLine.str();
		SetString(oldStr);
		return ss.str();
	}

	void ShowMessage(const std::string& msg)
	{
		targetString = AddLineBreaksToFitWidth(msg);
		index = 0;
		timer = 0;
	}

	void Update(float dt) {
		if (index < targetString.size()) {
			timer += dt;
			if (timer > SPEED) {
				timer -= SPEED;
				index++;
				SetString(targetString.substr(0,index));
			}
		}
	}

	operator GPU_Image* () const {
		return AsImage();
	}
};


