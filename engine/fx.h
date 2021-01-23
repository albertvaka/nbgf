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
			case Preset::Earthquake: // Make it long by calling this repeatedly
				Start(0.1f, veci(2, 2), vec(35.f, 45.f));
				break;
			case Preset::LittleStomp:
				Start(0.17f, veci(0, 2), vec(0.f, 47.f));
				break;
			case Preset::Stomp:
				Start(0.17f, veci(0, 3), vec(0.f, 47.f));
				break;
			case Preset::ElectricShock:
				Start(0.157f, veci(8, 2), vec(86.7f, 14.1f));
				break;
			}
		}

		static inline void Start(float time, veci amplitude, vec speed) {
			if (time >= screenshakeTime) {
				screenshakeTime = time;
				screenshakeAmplitude = amplitude;
				screenshakeSpeed = speed;
			}
		}

		static void DrawImgui();

		static inline float screenshakeTime;
		static inline veci screenshakeAmplitude = veci(0, 0);
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
		static inline GPU_Image* blankTexture;
	};

	struct FreezeImage {

		static void Freeze(int in_frames, float during_time) {
			if (nextWorldStopIn < 0) {
				nextWorldStopIn = in_frames;
				nextWorldStopDuration = during_time;
			}
		}

		static bool IsFrozen() { return (worldStoppedTime > 0.f); }

		static inline float worldStoppedTime = -1.f;
		static inline int nextWorldStopIn = -1.f;
		static inline float nextWorldStopDuration;
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


	void Init();
	void BeforeEnterScene();
	void AfterDraw();
	void Update(float dt);

};