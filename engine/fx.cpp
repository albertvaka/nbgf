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
		if (FreezeImage::worldStoppedTime <= 0) {
			FreezeImage::worldStoppedTime = -1.f;
		}
		return;
	}

	if (FreezeImage::nextWorldStopIn >= 0) {
		FreezeImage::nextWorldStopIn -= 1;
		if (FreezeImage::nextWorldStopIn < 0) {
			FreezeImage::worldStoppedTime = FreezeImage::nextWorldStopDuration;
			FreezeImage::nextWorldStopIn = -1;
		}
	}

	if (Screenshake::screenshakeTime > 0) {
		Screenshake::screenshakeTime -= dt;
		if (Screenshake::screenshakeTime <= 0) {
			Camera::screenshake_offset = vec::Zero;
		}
		else {
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
		}
		Camera::SetCenter(Camera::Center()); // makes it pick up the offset
	}

}

void Screenshake::DrawImgui()
{
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
		Screenshake::Start(shakeTime, veci(shakeAmplitudeX, shakeAmplitudeY), vec(shakeSpeedX, shakeSpeedY));
	};
	ImGui::End();
#endif
}

void InitRenderToTextureTarget()
{
	if (FullscreenShader::renderToTextureTarget) {
		GPU_FreeImage(FullscreenShader::renderToTextureTarget);
		FullscreenShader::renderToTextureTarget = nullptr;
	}
	FullscreenShader::renderToTextureTarget = Window::CreateTexture(Window::screenTarget->base_w, Window::screenTarget->base_h);
	GPU_Target* target = GPU_GetTarget(FullscreenShader::renderToTextureTarget);
	GPU_SetVirtualResolution(target, Window::GAME_WIDTH, Window::GAME_HEIGHT);
	FullscreenShader::renderToTextureSize = vec(Window::screenTarget->base_w, Window::screenTarget->base_h);
	// Start with both buffers fully black
	Window::Clear(0, 0, 0);
	GPU_Flip(Window::screenTarget);
	Window::Clear(0, 0, 0);
}

void FullscreenShader::Activate(bool clear)
{
	if (!shaderActivation) {
		return;
	}
	if (enabled) {
		return;
	}
	enabled = true;
	if (renderToTextureSize.x != Window::screenTarget->base_w || renderToTextureSize.y != Window::screenTarget->base_h) 
	{
		InitRenderToTextureTarget();
	}
	Window::BeginRenderToTexture(renderToTextureTarget, true);
	if (clear) {
		GPU_ClearRGBA(Window::currentDrawTarget, 0, 0, 0, 0);
	}
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

	GPU_UnsetVirtualResolution(Window::screenTarget);
	Camera::InScreenCoords::Begin();
	Window::Draw(FullscreenShader::renderToTextureTarget, Camera::InScreenCoords::TopLeft());
	Camera::InScreenCoords::End();
	GPU_SetVirtualResolution(Window::screenTarget, Window::GAME_WIDTH, Window::GAME_HEIGHT);

	Shader::Deactivate();
}

void Init()
{
	InitRenderToTextureTarget();

    ScreenTransition::blankTexture = Window::CreateTexture(32,32);
	Window::BeginRenderToTexture(ScreenTransition::blankTexture, false);
    Window::Clear(255,255,0);
    Window::EndRenderToTexture();

}

void BeforeEnterScene()
{

	//Stop everything

	FullscreenShader::shaderActivation = nullptr;

	ScreenTransition::transitionJustFinished = false;

	Screenshake::screenshakeTime = -1;
	Camera::screenshake_offset = vec::Zero;
	Camera::SetCenter(Camera::Center());

	FreezeImage::worldStoppedTime = -1;

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