#include "Canister.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "window_conf.h"


Canister::Canister(vec aPos): BoxEntity(aPos, vec(100, 100)) {
}
Canister::~Canister() {
}

void Canister::Draw() {
    Window::Draw(Assets::canisterTexture, pos)
		.withOrigin(Assets::canisterTexture->w/2, 0);
    
    if(Debug::Draw) {
		Bounds().DebugDraw(255,0,0);
	}
}

std::pair<Window::PartialDraw, Window::PartialDraw> Canister::PartialDraws() {
    Window::PartialDraw shadow = Window::PartialDraw(Assets::canisterShadowTexture, pos)
			.withOrigin(Assets::canisterShadowTexture->w-25, Assets::canisterShadowTexture->h);
    Window::PartialDraw sprite = Window::PartialDraw(Assets::canisterTexture, pos)
			.withOrigin(Assets::canisterTexture->w/2, Assets::canisterTexture->h);
	std::pair<Window::PartialDraw, Window::PartialDraw> p(shadow, sprite);
    return p;
}