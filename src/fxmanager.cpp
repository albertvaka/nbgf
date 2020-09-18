#include "fxmanager.h"

#include "camera.h"
#include "SDL_gpu.h"
#include "shader.h"
#include "assets.h"
#include "window.h"
#ifdef _IMGUI
#include "imgui.h"
#endif

static GPU_Image* renderToTextureTarget;

void FxManager::Update(float dt) {
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
			const int discreteSteps = 4; // There will be N fixed position up and N down the screen can be at.
			if (screenshakeAmplitude.y > 0) {
				screenshake.y = (int(cos(screenshakeTime * screenshakeSpeed.y) * discreteSteps) * screenshakeAmplitude.y) / discreteSteps;
			}
			else {
				screenshake.y = 0;
			}
			if (screenshakeAmplitude.x > 0) {
				screenshake.x = int(cos(screenshakeTime * screenshakeSpeed.x) * discreteSteps * screenshakeAmplitude.x) / discreteSteps;
			}
			else {
				screenshake.x = 0;
			}
		}
	}

}

void FxManager::DrawImgui() {
#ifdef _IMGUI
	ImGui::Begin("Screenshake");
	static float shakeTime = 1.f;
	static int shakeAmplitudeX = 0;
	static int shakeAmplitudeY = 5;
	static float shakeSpeedX = 10;
	static float shakeSpeedY = 10;
	ImGui::SliderFloat("shakeTime", &shakeTime, 0.1f, 2.f);
	ImGui::SliderInt("shakeAmplitudeX", &shakeAmplitudeX, 0, 10);
	ImGui::SliderInt("shakeAmplitudeY", &shakeAmplitudeY, 0, 10);
	ImGui::SliderFloat("shakeSpeedX", &shakeSpeedX, 0, 100);
	ImGui::SliderFloat("shakeSpeedY", &shakeSpeedY, 0, 100);
	if (ImGui::Button("Shake")) {
		FxManager::StartScreenshake(shakeTime, veci(shakeAmplitudeX, shakeAmplitudeY), vec(shakeSpeedX, shakeSpeedY));
	};
	ImGui::End();
#endif
}

void FxManager::BeginDraw() {
	//if (!renderToTextureTarget) {
	//	renderToTextureTarget = Window::CreateTexture(Window::GAME_WIDTH, Window::GAME_HEIGHT);
	//}
	//Window::BeginRenderToTexture(renderToTextureTarget(), true);
}

void FxManager::EndDraw() {
	
	// Apply fullscreen shaders
	//Window::EndRenderToTexture();
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
