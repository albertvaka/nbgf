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
static bool renderingToTexture = false;
static vec renderToTextureSize = vec(-1,-1);
extern float mainClock;

void FxManager::StartTransition(Shader& shader, float duration)
{
	transitionTime = duration;
	transitionDuration = duration;
	transitionFinished = false;
	transitionShader = &shader;
	transitionShaderProgressUniform = shader.GetUniformLocation("progress", false);
	transitionShaderAspectRatioUniform = shader.GetUniformLocation("aspectRatio", false);
}

void FxManager::Update(float dt) {

	if (worldStoppedTime > 0) {
		worldStoppedTime -= dt;
		if (worldStoppedTime <= 0) {
			worldStoppedTime = -1.f;
		}
	}

	if (nextWorldStopIn >= 0) {
		nextWorldStopIn -= 1;
		if (nextWorldStopIn < 0) {
			worldStoppedTime = nextWorldStopDuration;
			nextWorldStopIn = -1;
		}
	}

	if (transitionTime > 0) {
		transitionTime -= dt;
		transitionFinished = transitionTime <= 0;
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
	if (!beforeDrawFullscreen) {
		renderingToTexture = false;
		return;
	}
	renderingToTexture = true;
	if (renderToTextureSize.x != Window::screenTarget->base_w || renderToTextureSize.y != Window::screenTarget->base_h) 
	{
		if (renderToTextureTarget) {
			GPU_FreeImage(renderToTextureTarget);
			renderToTextureTarget = nullptr;
		}
		renderToTextureTarget = Window::CreateTexture(Window::screenTarget->base_w, Window::screenTarget->base_h);
		GPU_Target* target = GPU_GetTarget(renderToTextureTarget);
		GPU_SetVirtualResolution(target, Window::GAME_WIDTH, Window::GAME_HEIGHT);
		renderToTextureSize = vec(Window::screenTarget->base_w, Window::screenTarget->base_h);
		// Start with both buffers fully black
		Window::Clear(0, 0, 0);
		GPU_Flip(Window::screenTarget);
		Window::Clear(0, 0, 0);
	}
	Window::BeginRenderToTexture(renderToTextureTarget, true);
}

void FxManager::EndDraw() {
	
	if (renderingToTexture) {
		Window::EndRenderToTexture();

		if (beforeDrawFullscreen) {
			beforeDrawFullscreen();
		}

		GPU_UnsetVirtualResolution(Window::screenTarget);
		Camera::InScreenCoords::Begin();
		Window::Draw(renderToTextureTarget, Camera::InScreenCoords::TopLeft());
		Camera::InScreenCoords::End();
		GPU_SetVirtualResolution(Window::screenTarget, Window::GAME_WIDTH, Window::GAME_HEIGHT);

		Shader::Deactivate();
	}

	if (transitionTime > 0.f) {
		Camera::InScreenCoords::Begin();

		transitionShader->Activate();

		BoxBounds screenBounds = Camera::InScreenCoords::Bounds();
		transitionShader->SetUniform(transitionShaderProgressUniform, transitionTime / transitionDuration);
		transitionShader->SetUniform(transitionShaderAspectRatioUniform, screenBounds.width/screenBounds.height);

		Window::Draw(Assets::blankTexture, screenBounds);

		Shader::Deactivate();

		Camera::InScreenCoords::End();
	}



}
