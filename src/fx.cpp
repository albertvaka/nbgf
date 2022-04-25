#include "fx.h"

#include "camera.h"
#include "SDL_gpu.h"
#include "shader.h"
#include "window.h"
#ifdef _IMGUI
#include "imgui.h"
#endif

namespace Fx {

void ScreenTransition::Start(Shader& shader, float duration)
{
	transitionTime = duration;
	transitionDuration = duration;
	transitionJustFinished = false;
	transitionShader = &shader;
	transitionShaderProgressUniform = shader.GetUniformLocation("progress", false);
	transitionShaderAspectRatioUniform = shader.GetUniformLocation("aspectRatio", false);
}

void Update(float dt)
{

	ScreenTransition::transitionJustFinished = false;
	if (ScreenTransition::transitionTime > 0) {
		ScreenTransition::transitionTime -= dt;
		ScreenTransition::transitionJustFinished = (ScreenTransition::transitionTime <= 0);
		return;
	}

	if (FreezeImage::worldStoppedTime > 0) {
		FreezeImage::worldStoppedTime -= dt;
		if (FreezeImage::worldStoppedUpdate) {
			FreezeImage::worldStoppedUpdate(dt);
		}
		if (FreezeImage::worldStoppedTime <= 0) {
			FreezeImage::worldStoppedTime = -1.f;
			if (FreezeImage::unfreezeCallback) {
				FreezeImage::unfreezeCallback();
			}
		}
		if (!FreezeImage::continueScreenShake) {
			return;
		}
	}

	if (Screenshake::shaking) {
		if (Screenshake::screenshakeTime <= 0) {
			Camera::screenshake_offset = vec::Zero;
			Screenshake::shaking = false;
		}
		else {
			Screenshake::screenshakeTime -= dt;
			const int discreteSteps = 4; // There will be N fixed position up and N down the screen can be at.
			if (Screenshake::screenshakeAmplitude.y > 0) {
				Camera::screenshake_offset.y = (int(cos(Screenshake::screenshakeTime * Screenshake::screenshakeSpeed.y) * discreteSteps) * Screenshake::screenshakeAmplitude.y) / discreteSteps;
			}
			else {
				Camera::screenshake_offset.y = 0;
			}
			if (Screenshake::screenshakeAmplitude.x > 0) {
				Camera::screenshake_offset.x = int(cos(Screenshake::screenshakeTime * Screenshake::screenshakeSpeed.x) * discreteSteps * Screenshake::screenshakeAmplitude.x) / discreteSteps;
			}
			else {
				Camera::screenshake_offset.x = 0;
			}
			if (Screenshake::screenshakeDampening > 0) {
				Screenshake::screenshakeAmplitude.x *= Screenshake::screenshakeDampening;
				Screenshake::screenshakeAmplitude.y *= Screenshake::screenshakeDampening;
			}
		}
		Camera::SetCenter(Camera::Center()); // makes sdl_gpu pick up the offset in case nothing else updates the camera this frame
	}
}

void Screenshake::DrawImgui()
{
#ifdef _IMGUI
	ImGui::Begin("Screenshake");
	static float shakeTime = 1.f;
	static float shakeAmplitudeX = 0;
	static float shakeAmplitudeY = 5;
	static float shakeSpeedX = 10;
	static float shakeSpeedY = 10;
	ImGui::SliderFloat("shakeTime", &shakeTime, 0.1f, 2.f);
	ImGui::SliderFloat("shakeAmplitudeX", &shakeAmplitudeX, 0.f, 10.f);
	ImGui::SliderFloat("shakeAmplitudeY", &shakeAmplitudeY, 0.f, 10.f);
	ImGui::SliderFloat("shakeSpeedX", &shakeSpeedX, 0, 100);
	ImGui::SliderFloat("shakeSpeedY", &shakeSpeedY, 0, 100);
	if (ImGui::Button("Shake")) {
		Screenshake::Start(shakeTime, veci(shakeAmplitudeX, shakeAmplitudeY), vec(shakeSpeedX, shakeSpeedY));
	};
	ImGui::End();
#endif
}

void InitRenderToTextureTarget()
{
	if (FullscreenShader::renderToTextureTarget) {
		Window::DestroyTexture(FullscreenShader::renderToTextureTarget);
		FullscreenShader::renderToTextureTarget = nullptr;
	}
	FullscreenShader::renderToTextureTarget = Window::CreateTexture(Window::GAME_WIDTH, Window::GAME_HEIGHT);
	FullscreenShader::renderToTextureScale = Window::GetViewportScale();
	// Start with both buffers fully black
	//Window::Clear(0, 0, 0);
	//GPU_Flip(Window::screenTarget);
	//Window::Clear(0, 0, 0);
}

bool FullscreenShader::Activate()
{
	if (!shaderActivation) {
		return false;
	}
	if (enabled) {
		return true;
	}
	enabled = true;
	if (renderToTextureScale != Window::GetViewportScale())
	{
		InitRenderToTextureTarget();
	}
	Window::BeginRenderToTexture(renderToTextureTarget, true);
	return true;
}


void FullscreenShader::Deactivate()
{
	if (!enabled) {
		return;
	}

	enabled = false;

	Window::EndRenderToTexture();

	if (shaderActivation) {
		shaderActivation();
	}

	Window::Draw(FullscreenShader::renderToTextureTarget, Camera::TopLeft());

	Shader::Deactivate();
}


void BeforeEnterScene()
{
	// First time init
	if (ScreenTransition::blankTexture == nullptr) {
		InitRenderToTextureTarget();

		ScreenTransition::blankTexture = Window::CreateTexture(32, 32);
		Window::BeginRenderToTexture(ScreenTransition::blankTexture, false);
		Window::Clear(255, 255, 0);
		Window::EndRenderToTexture();
	}

	//Stop everything

	FullscreenShader::shaderActivation = nullptr;

	ScreenTransition::transitionJustFinished = false;

	Screenshake::shaking = false;
	Screenshake::screenshakeTime = -1;
	Camera::screenshake_offset = vec::Zero;
	Camera::SetCenter(Camera::Center());

	FreezeImage::worldStoppedTime = -1;
	FreezeImage::worldStoppedUpdate = nullptr;
	FreezeImage::unfreezeCallback = nullptr;
}

void AfterDraw()
{

	FullscreenShader::Deactivate();

	if (ScreenTransition::transitionTime > 0.f || ScreenTransition::transitionJustFinished) {
		Camera::InScreenCoords::Begin();

		ScreenTransition::transitionShader->Activate();

		BoxBounds screenBounds = Camera::InScreenCoords::Bounds();
		ScreenTransition::transitionShader->SetUniform(ScreenTransition::transitionShaderProgressUniform, ScreenTransition::transitionTime / ScreenTransition::transitionDuration);
		ScreenTransition::transitionShader->SetUniform(ScreenTransition::transitionShaderAspectRatioUniform, screenBounds.width/screenBounds.height);

		Window::Draw(ScreenTransition::blankTexture, screenBounds);

		Shader::Deactivate();

		Camera::InScreenCoords::End();
	}

}

}