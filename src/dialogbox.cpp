#include "dialogbox.h"

#include "anim_lib.h"
#include "window.h"
#include "input.h"
#include "camera.h"

constexpr const static float kPadding = 12;
constexpr const static float kSpaceBetweenTitleAndBody = 6;
constexpr const static float kFontScale = 0.5f;
constexpr const static float kPortraitScale = 2;
constexpr const static float kMarginSides = 32;
constexpr const static float kMarginBottom = 22;
constexpr const static float kBottomRightIconMargin = 20;
constexpr const static float kPortraitWidth = AnimLib::PORTRAIT_WARRIOR.w * kPortraitScale;
constexpr const static float kPortraitHeight = AnimLib::PORTRAIT_WARRIOR.h * kPortraitScale;
constexpr const static float kWidth = Window::GAME_WIDTH - 2 * kMarginSides;
constexpr const static float kHeight = kPortraitHeight + 2 * kPadding;
constexpr const static float kPosY = Window::GAME_HEIGHT - kHeight - kMarginBottom;
constexpr const static float kMaxLineWidth = kWidth - 3 * kPadding - kPortraitWidth;
constexpr const static float kTimeToOpenClose = 0.2f;

void Render9Slice(GPU_Image* s,	float x, float y, float w, float h, float top, float bottom, float left, float right, bool repeat = false);

DialogBox::DialogBox()
	: title(Assets::font_dialog_title)
	, body(kMaxLineWidth * 2, Assets::font_dialog_body)
	, openCloseTimer(kTimeToOpenClose)
{
	title.SetFillColor(255, 255, 255);
	title.SetOutlineColor(0, 0, 0);

	body.SetFillColor(255, 255, 255);
	body.SetOutlineColor(0, 0, 0);
}

bool DialogBox::IsOpen() const
{
	return isOpen || openCloseTimer < kTimeToOpenClose;
}

bool DialogBox::Update(float dt) { // returns true when the text is fully displayed and the user presses action to close it
	if (openCloseTimer < kTimeToOpenClose) {
		openCloseTimer += dt;
		if (Input::IsJustPressed(0, GameKeys::START)) {
			body.SkipAnimation();
		}
		if (isOpen && openCloseTimer >= kTimeToOpenClose) {
			if (body.IsFullyShown()) {
				voiceChannel = currentVoice->end.Play();
			}
			else {
				voiceChannel = currentVoice->speak.Play();
			}
		}
	} else if (isOpen) {
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
			voiceChannel = currentVoice->end.Play();
		}
	}
	return false;
}

void DialogBox::Draw() const {
	if (openCloseTimer < kTimeToOpenClose) {
		float from = isOpen? kWidth/2.f - 8 : 8;
		float to = isOpen? 8 : kWidth/2.f - 8;
		float diff = Mates::Lerp(from, to, openCloseTimer/kTimeToOpenClose);
		Render9Slice(Assets::dialogFrameTexture, kMarginSides + diff, kPosY, kWidth-2*diff, kHeight, 8, 8, 8, 8);
	} else if (isOpen) {
		(Camera::TopLeft()+vec(kMarginSides, kPosY)).DebugDraw();
		(Camera::TopLeft()+vec(kMarginSides+kWidth, kPosY+kHeight)).DebugDraw();

		Render9Slice(Assets::dialogFrameTexture, kMarginSides, kPosY, kWidth, kHeight, 8, 8, 8, 8);

		Window::Draw(Assets::warriorTexture, kMarginSides + kPadding, kPosY + kPadding)
			.withRect(currentPortrait)
			.withScale(kPortraitScale);

		vec titlePos = vec(kPadding * 2 + kMarginSides + kPortraitWidth, kPosY + kPadding);
		Window::Draw(title, titlePos)
			.withScale(kFontScale);
		vec bodyPos = titlePos+vec(0, title.Size().y * kFontScale + kSpaceBetweenTitleAndBody);
		Window::Draw(body, bodyPos)
			.withScale(kFontScale);

		if (body.IsFullyShown()) {
			if (choices.empty()) {
				GPU_Rect icon = isLast ? AnimLib::UI_ICON_CLOSE : AnimLib::UI_ICON_NEXT;
				Window::Draw(Assets::spritesheetTexture, kMarginSides + kWidth - kBottomRightIconMargin, kPosY + kHeight - kBottomRightIconMargin)
					.withRectWithOriginCentered(icon)
					.withScale(1 + abs(sin(mainClock * 4.f)) / 4.f);
			}
			else {
				float xIncrease = kWidth / (choices.size()+2);
				vec choicesPos(bodyPos.x + xIncrease, bodyPos.y + body.Size().y * kFontScale);
				float alignVertically = (kHeight - (choicesPos.y - kPosY)) / 2.f;
				choicesPos.y += alignVertically;
				for (const Text& text : choices) {
					Window::Draw(text, choicesPos)
						.withScale(kFontScale)
						.withOrigin(text.Size()/2);
					choicesPos.x += xIncrease;
				}
			}
		}
	}
}

// Code adapted from https://github.com/cxong/sdl2-9-slice (MIT license)
void Render9Slice(GPU_Image* s,
	float x, float y, float w, float h, float top, float bottom, float left, float right,
	bool repeat) // whether to tile or stretch
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
					Window::Draw(s, BoxBounds(dst.x, dst.y, dst.w * 3, dst.h * 3)).withRect(src);
				}
			}
		}
	}
}
