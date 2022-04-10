#pragma once 

#include "vec.h"
#include "SDL_gpu.h"
#include <functional>

struct Shader;

namespace Fx {

	struct Screenshake {

		enum class Preset {
			Earthquake,
			LittleStomp,
			Stomp,
			ElectricShock,
		};

		static inline void StartPreset(Preset preset) {
			switch (preset) {
				break;
			case Preset::Earthquake:
				Start(0.2f, vec(2.5f, 2.5f), vec(35.f, 45.f));
				break;
			case Preset::LittleStomp:
				Start(0.3f, vec(0, 3), vec(0.f, 47.f));
				break;
			case Preset::Stomp:
				Start(0.3f, vec(0, 5), vec(0.f, 47.f));
				break;
			case Preset::ElectricShock:
				Start(0.157f, vec(8, 2), vec(86.7f, 14.1f));
				break;
			}
		}

		static inline void Start(float time, vec amplitude, vec speed, float dampening = -1.f) {
			if (time >= screenshakeTime) {
				shaking = true;
				screenshakeTime = time;
				screenshakeAmplitude = amplitude;
				screenshakeSpeed = speed;
				screenshakeDampening = dampening;
			}
		}

		static inline void Stop() {
			screenshakeTime = -1.f;
		}

		static void DrawImgui();

		static inline bool shaking = false;
		static inline float screenshakeTime;
		static inline float screenshakeDampening;
		static inline vec screenshakeAmplitude = veci(0, 0);
		static inline vec screenshakeSpeed = veci(0, 0);
	};


	struct ScreenTransition {

		static void Start(Shader& shader, float duration = 1.f);

		static inline bool IsActive() {
			return (transitionTime > 0);
		}

		static inline bool IsJustFinished() {
			return transitionJustFinished;
		}

		static inline Shader* Current() {
			return transitionShader;
		}

		static inline Shader* transitionShader = nullptr;
		static inline int transitionShaderProgressUniform = -1;
		static inline int transitionShaderAspectRatioUniform = -1;
		static inline float transitionTime = 0;
		static inline float transitionDuration = 0;
		static inline bool transitionJustFinished = false;
		static inline GPU_Image* blankTexture = nullptr;
	};

	struct FreezeImage {

		static void Freeze(float durationSeconds, bool continueScreenShakeWhileFrozen = false) {
			worldStoppedTime = durationSeconds;
			continueScreenShake = continueScreenShakeWhileFrozen;
		}

		static std::function<void(float dt)> GetAlternativeUpdateFnWhileFrozen() {
			return worldStoppedUpdate;
		}

		static void SetAlternativeUpdateFnWhileFrozen(std::function<void(float dt)> update = nullptr) {
			worldStoppedUpdate = update;
		}

		static void SetUnfreezeCallback(std::function<void()> update = nullptr) {
			unfreezeCallback = update;
		}

		static bool IsFrozen() { return (worldStoppedTime > 0.f); }

		static inline std::function<void(float dt)> worldStoppedUpdate = nullptr;
		static inline std::function<void()> unfreezeCallback = nullptr;
		static inline float worldStoppedTime = -1.f;
		static inline bool continueScreenShake = false;
	};

	struct FullscreenShader {

		static void SetShader(std::function<void()> shaderActivationLambda) { shaderActivation = shaderActivationLambda; }

		static void Activate(bool clearLastFrame=true);
		static void Deactivate();

		static inline bool enabled = false;
		static inline std::function<void()> shaderActivation = nullptr;
		static inline GPU_Image* renderToTextureTarget;
		static inline vec renderToTextureSize = vec(-1,-1);
	};


	void BeforeEnterScene();
	void AfterDraw();
	void Update(float dt);

};