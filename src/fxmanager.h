#pragma once 

#include "vec.h"

struct FxManager {

	static inline bool IsIntroTransitionActive() {
		return (introTime > 0);

	}

	static inline void StartIntroTransition(float time) {
		introTime = time;
		introDuration = time;
		introfinished = false;
	}

	static inline bool IsOuttroTransitionActive() {
		return (outtroTime > 0);

	}

	static inline void StartOuttroTransition(float time) {
		outtroTime = time;
		outtroDuration = time;
		outtrofinished = false;
	}

	static inline bool IsOuttroTransitionDone() {
		return outtrofinished;
	}

	static inline void ResetOuttroTransitionDone() {
		outtrofinished = false;
	}

	static inline bool IsIntroTransitionDone() {
		return introfinished;
	}

	static inline void ResetIntroTransitionDone() {
		introfinished = false;
	}

	static inline void StartScreenshake(float time, veci amplitude, vec speed) {
		if (time >= screenshakeTime) {
			screenshakeTime = time;
			screenshakeAmplitude = amplitude;
			screenshakeSpeed = speed;
		}
	}
	enum class ScreenShakePreset {
		Earthquake,
		LittleStomp,
		Stomp,
		Electroshok,
	};
	static inline void StartScreenshakePreset(ScreenShakePreset preset) {
		switch (preset) {
		case ScreenShakePreset::Earthquake: // Call repeatedly for a sustained effect
			StartScreenshake(0.1f, veci(2,2), vec(35.f,45.f));
			break;
		case ScreenShakePreset::LittleStomp:
			StartScreenshake(0.17f, veci(0, 2), vec(0.f, 47.f));
			break;
		case ScreenShakePreset::Stomp:
			StartScreenshake(0.17f, veci(0, 3), vec(0.f, 47.f));
			break;
		case ScreenShakePreset::Electroshok:
			StartScreenshake(0.157f, veci(8, 2), vec(86.7f, 14.1f));
			break;
		}
	}

	static void Update(float dt);

	static void BeginDraw();

	static void EndDraw();

	static inline const vec& GetScreenshake() {
		return screenshake;
	}

	static void DrawImgui();

	static void StopTheWorld(int in_frames, float during_time) {
		if (nextWorldStopIn < 0) {
			nextWorldStopIn = in_frames;
			nextWorldStopDuration = during_time;
		}
	}

	static bool IsTheWorldStopped() { return (worldStoppedTime > 0.f); }

private:
	static inline vec screenshake = vec();

	static inline float introTime = 0;
	static inline float introDuration = 0;
	static inline bool introfinished = false;

	static inline float outtroTime = 0;
	static inline float outtroDuration = 0;
	static inline bool outtrofinished = false;

	static inline float screenshakeTime;
	static inline veci screenshakeAmplitude = veci(0, 0);
	static inline vec screenshakeSpeed = veci(0, 0);

	static inline float worldStoppedTime = -1.f;
	static inline int nextWorldStopIn = -1.f;
	static inline float nextWorldStopDuration;
};