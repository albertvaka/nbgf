#pragma once 

#include "camera.h"
#include "SDL_gpu.h"
#include "shader.h"
#include "assets.h"
#include "window.h"
#include "singleinstance.h"

extern float mainClock;

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

	static inline void StartScreenshake(veci intensity, float time) {
		screenshakeTime = time;
		screenshakeIntensity = intensity;
	}

	static inline void Update(float dt) {
		if (introTime > 0) {
			introTime -= dt;
			introfinished = introTime <= 0;
		}

		if (outtroTime > 0) {
			outtroTime -= dt;
			outtrofinished = outtroTime <= 0;
		}

		if (screenshakeTime > 0) {
			screenshakeTime -= dt;
			if (screenshakeTime <= 0) {
				screenshake = vec::Zero;
			}
			else {
				if (screenshakeIntensity.y > 0.1) {
					int v = screenshakeIntensity.y;
					screenshake.y = (int(cos(mainClock * 15) * 3 * v) / v) - v / 2;
				}
				else {
					screenshake.y = 0;
				}
				if (screenshakeIntensity.x > 0.1) {
					int v = screenshakeIntensity.x;
					screenshake.x = (int(cos(mainClock * 10) * 3 * v) / v) - v/2;
				}
				else {
					screenshake.x = 0;
				}
			}
		}

	}

	static inline void BeginDraw() {
		//Window::BeginRenderToTexture(renderToTextureTarget(), true);
	}

	static inline void EndDraw() {
		Window::EndRenderToTexture();

		//Assets::waveShader.Activate();
		//Assets::waveShader.SetUniform("time", mainClock);
		//Window::Draw(renderToTextureTarget(), Camera::GetTopLeft());
		//Shader::Deactivate();

		if (introTime > 0.f) {
			Assets::fadeInDiamondsShader.Activate();
			Assets::fadeInDiamondsShader.SetUniform("fadeInProgress", introTime / introDuration);
			Window::Draw(Assets::blankTexture, Camera::GetBounds());
			Shader::Deactivate();
		}
		if (outtroTime > 0.f) {
			Assets::fadeOutDiamondsShader.Activate();
			Assets::fadeOutDiamondsShader.SetUniform("fadeOutProgress", outtroTime / outtroDuration);
			Window::Draw(Assets::blankTexture, Camera::GetBounds());
			Shader::Deactivate();
		}

	}

	static inline const vec& GetScreenshake() {
		return screenshake;
	}

private:
	static inline vec screenshake = vec();

	static inline float introTime = 0;
	static inline float introDuration = 0;
	static inline bool introfinished = false;

	static inline float outtroTime = 0;
	static inline float outtroDuration = 0;
	static inline bool outtrofinished = false;

	static inline float screenshakeTime;
	static inline veci screenshakeIntensity = veci(0,0);

	static inline GPU_Image* renderToTextureTarget() {
		static GPU_Image* target = Window::CreateTexture(Window::GAME_WIDTH, Window::GAME_HEIGHT);
		return target;
	}

};