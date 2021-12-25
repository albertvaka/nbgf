#pragma once

#include "window.h"
#include "text.h"
#include "assets.h"
#include "input.h"
#include "appearingtext.h"
#include "anim_lib.h"

extern float mainClock;

struct DialogCharacter {
	std::string name;
	GPU_Rect portrait;
	const Voice& voice;
};

struct DialogBox
{
	const float kPadding = 12;
	const float kSpaceBetweenTitleAndBody = 6;
	const float kFontScale = 0.5f;
	const float kPortraitScale = 2;
	const float kMarginSides = 32;
	const float kMarginBottom = 22;
	const float kBottomRightIconMargin = 20;
	const float kPortraitWidth = AnimLib::PORTRAIT_WARRIOR.w*kPortraitScale;
	const float kPortraitHeight = AnimLib::PORTRAIT_WARRIOR.h*kPortraitScale;
	const float kWidth = Window::GAME_WIDTH - 2 * kMarginSides;
	const float kHeight = kPortraitHeight + 2 * kPadding;
	const float kPosY = Window::GAME_HEIGHT - kHeight - kMarginBottom;
	const float kMaxLineWidth = kWidth - 3 * kPadding - kPortraitWidth;
	const float kTimeToOpenClose = 0.2f;

	Text title;
	AppearingText body;
	GPU_Rect currentPortrait;
	const Voice* currentVoice;
	int voiceChannel = -1;
	bool isLast;

	int index = 0;
	float openCloseTimer = kTimeToOpenClose;
	bool isOpen = false;

	DialogBox()
		: title(Assets::font_dialog_title)
		, body(kMaxLineWidth*2, Assets::font_dialog_body)
	{
		title.SetFillColor(255, 255, 255);
		title.SetOutlineColor(0, 0, 0);

		body.SetFillColor(255, 255, 255);
		body.SetOutlineColor(0, 0, 0);
	}

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
	std::vector<std::string> choices;

	void AddChoices(const std::vector<std::string>& choices)
	{
		selectedChoice = 0;
		this->choices = choices;
	}

	int GetSelectedChoice() {
		return selectedChoice;
	}

	void Close()
	{
		isOpen = false;
		openCloseTimer = 0;
	}

	bool IsOpen() const
	{
		return isOpen || openCloseTimer < kTimeToOpenClose;
	}

	void Draw() const {
		if (openCloseTimer < kTimeToOpenClose) {
			float from = isOpen? kWidth/2.f : 0;
			float to = isOpen? 0 : kWidth/2.f;
			float diff = Mates::Lerp(from, to, openCloseTimer/kTimeToOpenClose);
			Render9Slice(Assets::dialogFrameTexture, kMarginSides + diff, kPosY, kWidth-2*diff, kHeight, 8, 8, 8, 8);
		} else if (isOpen) {
			(Camera::TopLeft()+vec(kMarginSides, kPosY)).DebugDraw();
			(Camera::TopLeft()+vec(kMarginSides+kWidth, kPosY+kHeight)).DebugDraw();

			Render9Slice(Assets::dialogFrameTexture, kMarginSides, kPosY, kWidth, kHeight, 8, 8, 8, 8);

			Window::Draw(Assets::warriorTexture, kMarginSides + kPadding, kPosY + kPadding)
				.withRect(currentPortrait)
				.withScale(kPortraitScale);

			vec textPos(kPadding * 2 + kMarginSides + kPortraitWidth, kPosY + kPadding);
			Window::Draw(title, textPos)
				.withScale(kFontScale);
			Window::Draw(body, textPos+vec(0,title.Size().y*kFontScale + kSpaceBetweenTitleAndBody))
				.withScale(kFontScale);

			if (body.IsFullyShown()) {
				GPU_Rect icon = isLast ? AnimLib::UI_ICON_CLOSE : AnimLib::UI_ICON_NEXT;
				Window::Draw(Assets::spritesheetTexture, kMarginSides + kWidth - kBottomRightIconMargin, kPosY + kHeight - kBottomRightIconMargin)
					.withRectWithOriginCentered(icon)
					.withScale(1+abs(std::sin(mainClock*4.f))/4.f);
			}
		}
	}

	bool Update(float dt) { // returns true when the text is fully displayed and the user presses action to close it
		if (openCloseTimer < kTimeToOpenClose) {
			openCloseTimer += dt;
			if (isOpen && openCloseTimer >= kTimeToOpenClose) {
				voiceChannel = currentVoice->speak.Play();
			}
		}
		bool justFinishedTyping = body.Update(dt);
		if (!body.IsFullyShown()) {
			if (Input::IsJustPressed(0, GameKeys::START)) {
				body.SkipAnimation();
				justFinishedTyping = true;
			}
		}
		else {
			if (Input::IsJustPressed(0, GameKeys::ACTION)) {
				return true;
			}
		}
		if (justFinishedTyping) {
			if (voiceChannel >= 0) {
				Sound::Stop(voiceChannel);
				voiceChannel = -1;
			}
			currentVoice->end.Play();
		}
		return false;
	}


	// Code adapted from https://github.com/cxong/sdl2-9-slice (MIT license)
	void static Render9Slice(GPU_Image* s,
		float x, float y, float w, float h, float top, float bottom, float left, float right,
		bool repeat=false) // whether to tile or stretch
	{
		const float srcX[] = { 0, left, s->w - right };
		const float srcY[] = { 0, top, s->h - bottom };
		const float srcW[] = { left, s->w - right - left, right };
		const float srcH[] = { top, s->h - bottom - top, bottom };
		const float dstX[] = { x, x + left, x + w - right, x + w };
		const float dstY[] = { y, y + top, y + h - bottom, y + h };
		const float dstW[] = { left, w - right - left, right };
		const float dstH[] = { top, h - bottom - top, bottom };
		GPU_Rect src;
		GPU_Rect dst;
		for (int i = 0; i < 3; i++)
		{
			src.x = srcX[i];
			src.w = srcW[i];
			dst.w = repeat ? srcW[i] : dstW[i];
			for (dst.x = dstX[i]; dst.x < dstX[i + 1]; dst.x += dst.w)
			{
				if (dst.x + dst.w > dstX[i + 1])
				{
					src.w = dst.w = dstX[i + 1] - dst.x;
				}
				for (int j = 0; j < 3; j++)
				{
					src.y = srcY[j];
					src.h = srcH[j];
					dst.h = repeat ? srcH[j] : dstH[j];
					for (dst.y = dstY[j]; dst.y < dstY[j + 1]; dst.y += dst.h)
					{
						if (dst.y + dst.h > dstY[j + 1])
						{
							src.h = dst.h = dstY[j + 1] - dst.y;
						}
						//TODO: I think that magic 3 actually depends on the scale
						Window::Draw(s, BoxBounds(dst.x, dst.y, dst.w*3, dst.h*3)).withRect(src);
					}
				}
			}
		}
	}
};


