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
	}
	
	int advanceOneCharacter(int currentIndex, const std::string& targetString) {
		currentIndex++;
		if (currentIndex < targetString.size()) {
			while (targetString[currentIndex] == TextColor::MagicSeparator) { // handles TextColor
				currentIndex += TextColor::Length;
			}
			while ((targetString[currentIndex] & 0b11000000) == 0b11000000) { //handles muli-byte characters
				currentIndex++;
			}
		}
		return currentIndex;
	}

	std::string AddLineBreaksToFitWidth(const std::string& msg) {
		std::string oldStr = GetString();
		std::stringstream ss;
		std::stringstream currentLine;
		int wordBegin = 0;
		for (int i = 0; i <= msg.size(); i=advanceOneCharacter(i,msg)) {
			if (i >= msg.size() || msg[i] == ' ') {
				//footgun avoided: the second parameter is the size and not the end pos
				std::string nextWord = msg.substr(wordBegin, i-wordBegin);
				i++;
				wordBegin = i;
				std::string oldCurrentLine = currentLine.str();
				if (!oldCurrentLine.empty()) {
					currentLine << " ";
				}
				currentLine.write(&nextWord[0], nextWord.size());
				SetString(currentLine.str());
				float lineWidth = Size().x;
				if (lineWidth > maxLineWidth) {
					if (!oldCurrentLine.empty()) { // fix for words that don't fit in maxLineWidth
						ss.write(&oldCurrentLine[0], oldCurrentLine.size());
						ss << '\n';
					}
					//footgun avoided: stringstream::clear() exists but doesn't do what you expect,
					//this is the right way to clear it. 
					//bonus footgun: using currentLine::str(nextWord) didn't work either.
					currentLine.str(std::string());
					currentLine.write(&nextWord[0], nextWord.size());
				}
			}
		}
		std::string currentLineString = currentLine.str();
		ss.write(&currentLineString[0], currentLineString.size());
		SetString(oldStr);
		return ss.str();
	}

	void ShowMessage(const std::string& msg)
	{
		SetString("");
		targetString = AddLineBreaksToFitWidth(msg);
		index = 0;
		timer = 0;
	}

	bool IsFullyShown() const {
		return (index >= targetString.size());
	}

	void SkipAnimation() {
		index = targetString.size();
		SetString(targetString.substr(0,index));
	}

	bool Update(float dt) { //returns true the frame it ends
		if (index < targetString.size()) {
			timer += dt;
			if (timer > SPEED) {
				timer -= SPEED;
				index = advanceOneCharacter(index, targetString);
				SetString(targetString.substr(0,index));
				return index >= targetString.size();
			}
		}
		return false;
	}

	operator GPU_Image* () const {
		return AsImage();
	}
};


