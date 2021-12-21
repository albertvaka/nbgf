#pragma once

#include "window.h"
#include "text.h"
#include "assets.h"
#include "appearingtext.h"
#include "anim_lib.h"

struct DialogBox
{
	const float kPadding = 12;
	const float kSpaceBetweenTitleAndBody = 6;
	const float kFontScale = 0.5f;
	const float kPortraitScale = 2;
	const float kMargin = 32;
	const float kPortraitWidth = AnimLib::PORTRAIT_WARRIOR.w*kPortraitScale;
	const float kPortraitHeight = AnimLib::PORTRAIT_WARRIOR.h*kPortraitScale;
	const float kWidth = Window::GAME_WIDTH - 2 * kMargin; // Must be multiple of 8
	const float kHeight = kPortraitHeight + 2 * kPadding; // Must be multiple of 8
	const float kPosY = Window::GAME_HEIGHT - kHeight - kMargin + 10;
	const float kMaxLineWidth = kWidth - 3 * kPadding - kPortraitWidth;

	Text title;
	AppearingText body;
	GPU_Rect currentPortrait;

	int index = 0;
	float timer = 0;
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

	void ShowMessage(const GPU_Rect& portrait, const std::string& charname, const std::string& msg)
	{
		currentPortrait = portrait;
		title.SetString(charname);
		body.ShowMessage(msg);
		isOpen = true;
	}

	void Draw() const {
		if (isOpen) {

			(Camera::TopLeft()+vec(kMargin, kPosY)).DebugDraw();
			(Camera::TopLeft()+vec(kMargin+kWidth, kPosY+kHeight)).DebugDraw();

			Render9Slice(Assets::dialogFrameTexture, kMargin, kPosY, kWidth, kHeight, 8, 8, 8, 8, true);

			Window::Draw(Assets::warriorTexture, kMargin + kPadding, kPosY + kPadding)
				.withRect(currentPortrait)
				.withScale(kPortraitScale);

			vec textPos(kPadding * 2 + kMargin + kPortraitWidth, kPosY + kPadding);
			Window::Draw(title, textPos)
				.withScale(kFontScale);
			Window::Draw(body, textPos+vec(0,title.Size().y*kFontScale + kSpaceBetweenTitleAndBody))
				.withScale(kFontScale);
		}
	}

	void Update(float dt) {
		body.Update(dt);
	}


	// Code adapted from https://github.com/cxong/sdl2-9-slice (MIT license)
	void static Render9Slice(GPU_Image* s,
		int x, int y, int w, int h, int top, int bottom, int left, int right,
		bool repeat)
	{
		x += left / 2;
		y += top / 2;
		w += right / 2 + left / 2;
		h += top / 2 + bottom / 2;
		const int srcX[] = { 0, left, s->w - right };
		const int srcY[] = { 0, top, s->h - bottom };
		const int srcW[] = { left, s->w - right - left, right };
		const int srcH[] = { top, s->h - bottom - top, bottom };
		const int dstX[] = { x, x + left, x + w - right, x + w };
		const int dstY[] = { y, y + top, y + h - bottom, y + h };
		const int dstW[] = { left, w - right - left, right };
		const int dstH[] = { top, h - bottom - top, bottom };
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
						Window::Draw(s, BoxBounds(dst)*3).withRect(src);
					}
				}
			}
		}
	}
};


