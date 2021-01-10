#pragma once 

#include "vec.h"

struct Shader;

struct FxManager {

	static inline bool IsTransitionActive() {
		return (transitionTime > 0);
	}

	static inline Shader* GetCurrentTransition() {
		return transitionShader;
	}
	static void StartTransition(Shader& shader, float duration = 1.f);

	static inline bool IsTransitionDone() {
		return transitionFinished;
	}

	static inline void ResetTransitionDone() {
		transitionFinished = false;
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
		ElectricShock,
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
		case ScreenShakePreset::ElectricShock:
			StartScreenshake(0.157f, veci(8, 2), vec(86.7f, 14.1f));
			break;
		}
	}

	static void Update(float dt);

	static void BeginDraw();

	static void EndDraw();

	static inline vec GetScreenshake() {
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

	static inline Shader* transitionShader = nullptr;
	static inline int transitionShaderProgressUniform = -1;
	static inline int transitionShaderAspectRatioUniform = -1;
	static inline float transitionTime = 0;
	static inline float transitionDuration = 0;
	static inline bool transitionFinished = false;

	static inline float screenshakeTime;
	static inline veci screenshakeAmplitude = veci(0, 0);
	static inline vec screenshakeSpeed = veci(0, 0);

	static inline float worldStoppedTime = -1.f;
	static inline int nextWorldStopIn = -1.f;
	static inline float nextWorldStopDuration;
};