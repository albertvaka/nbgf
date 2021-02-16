#pragma once

#include "entity.h"
#include "selfregister.h"
#include "musicplayer.h"
#include "assets.h"
#include "person.h"
#include "window.h"
#include "text.h"

#include "SDL_mixer.h"

const float fade_in_duration = 2;
const float fade_out_duration = 1.5;

struct FreezeSkill : SelfRegister<FreezeSkill>
{
	Text actionText;
	float countdown = 2;
	float actionLength = 1.5;
	bool freezeNow = false;
	bool alive = true;

	float fade_in = fade_in_duration;
	float fade_out = fade_out_duration;

	int freeze_channel = -1;

	FreezeSkill() : actionText(Assets::font_30, Assets::font_30_outline) 
	{
		actionText.SetFillColor(0, 0, 0);
		actionText.SetOutlineColor(255, 0, 0);
	}

	~FreezeSkill() {
		if (freeze_channel != -1) {
			Assets::freeze_sound.Stop(freeze_channel);
		}
		MusicPlayer::Resume();
		MusicPlayer::SetVolume(100);
	}

	void Update(float dt){
		if (countdown > 0) {
			if (freeze_channel == -1) {
				freeze_channel = Assets::freeze_sound.Play();
				MusicPlayer::Pause();
			}

			if (fade_out >= 0) {
				fade_out -= dt;
				Assets::freeze_sound.SetVolume((fade_out_duration - fade_out) * 100);
			}

			countdown -= dt;
			actionText.SetString("Freeze in " + Mates::to_string_with_precision(std::max(0.f, countdown), 2));
		} else if (actionLength > 0) {
			actionLength -= dt;
			actionText.SetString("Now freeze! " + Mates::to_string_with_precision(std::max(0.f,actionLength), 2));
			freezeNow = true;
		} else {
			freezeNow = false;
			if (freeze_channel != -1) {
				Assets::freeze_sound.Stop(freeze_channel);
				MusicPlayer::Resume();
				freeze_channel = -1;
			}
			if (fade_in >= 0) {
				fade_in -= dt;
				MusicPlayer::SetVolume((fade_in_duration - fade_in) * 100);
			} else {
				alive = false;
			}
		}
	}

	void Draw(){
		if (countdown > 0 || actionLength > 0) {
			Window::Draw(actionText, vec(Camera::Center().x, 60))
				.withOrigin(actionText.Size() / 2)
				.withScale(1.5f);
		}
	}
};
