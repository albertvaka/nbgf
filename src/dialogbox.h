#pragma once

#include "window.h"
#include "text.h"
#include "assets.h"
#include "appearingtext.h"

struct DialogBox
{
	Text title;
	AppearingText body;

	int index = 0;
	float timer = 0;
	bool isOpen = false;

	const float kPadding = 20;
	const float kMargin = 30;
	const float kWidth = Mates::RoundUpToMultipleOf(Window::GAME_WIDTH - 2 * kMargin, 8);
	const float kHeight = Mates::RoundUpToMultipleOf(180 - 2 * kMargin, 8);
	const float kMaxLineWidth = kWidth  - 2 * kPadding;

	DialogBox()
		: title(Assets::font_30)
		, body(kMaxLineWidth, Assets::font_18)
	{
		title.SetFillColor(255, 255, 255);
		title.SetOutlineColor(0, 0, 0);

		body.SetFillColor(255, 255, 255);
		body.SetOutlineColor(0, 0, 0);
	}

	void ShowMessage(const std::string& charname, const std::string& msg)
	{
		title.SetString(charname);
		body.ShowMessage(msg);
		isOpen = true;
	}

	void Draw() const {
		if (isOpen) {
			Render9Slice(Assets::dialogFrameTexture, kMargin, kMargin, 8, 8, 8, 8, kWidth, kHeight, true);
			Window::Draw(title, kPadding + kMargin, kPadding + kMargin);
			Window::Draw(body, kPadding + kMargin, kPadding + kMargin + 50);
		}
	}

	void Update(float dt) {
		body.Update(dt);
	}


	// Code adapted from https://github.com/cxong/sdl2-9-slice (MIT license)
	void static Render9Slice(GPU_Image* s,
		int x, int y, int top, int bottom, int left, int right, int w, int h,
		bool repeat)
	{
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


