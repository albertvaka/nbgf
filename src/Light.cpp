#include "Light.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "window_conf.h"
#include "EnvironmentObject.h"


Light::Light(vec aPos): EnvironmentObject(aPos) {
}
Light::~Light() {
}

void Light::Draw() {
    Window::Draw(Assets::lightTexture, pos)
		.withOrigin(Assets::lightTexture->w/2, 0);
    
    if(Debug::Draw) {
		Bounds().DebugDraw(255,0,0);
	}
}

std::pair<Window::PartialDraw, Window::PartialDraw> Light::PartialDraws() {
    Window::PartialDraw shadow = Window::PartialDraw(Assets::lightShadowTexture, pos)
			.withOrigin(Assets::lightShadowTexture->w, Assets::lightShadowTexture->h);
    Window::PartialDraw sprite = Window::PartialDraw(Assets::lightTexture, pos)
			.withOrigin(Assets::lightTexture->w/2, Assets::lightTexture->h);
	std::pair<Window::PartialDraw, Window::PartialDraw> p(shadow, sprite);
    return p;
}