#include "Bush.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "window_conf.h"
#include "EnvironmentObject.h"


Bush::Bush(vec aPos): EnvironmentObject(aPos) {
}
Bush::~Bush() {
}

void Bush::Draw() {
    Window::Draw(Assets::bushTexture, pos)
		.withOrigin(Assets::bushTexture->w/2, 0);
    
    if(Debug::Draw) {
		Bounds().DebugDraw(255,0,0);
	}
}

std::pair<Window::PartialDraw, Window::PartialDraw> Bush::PartialDraws() {
    Window::PartialDraw shadow = Window::PartialDraw(Assets::bushShadowTexture, pos)
			.withOrigin(Assets::bushShadowTexture->w-65, Assets::bushShadowTexture->h-20);
    Window::PartialDraw sprite = Window::PartialDraw(Assets::bushTexture, pos)
			.withOrigin(Assets::bushTexture->w/2, Assets::bushTexture->h);
	std::pair<Window::PartialDraw, Window::PartialDraw> p(shadow, sprite);
    return p;
}