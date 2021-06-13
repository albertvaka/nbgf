#include "Charco.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "window_conf.h"
#include "EnvironmentObject.h"


Charco::Charco(vec aPos): EnvironmentObject(aPos) {
}
Charco::~Charco() {
}

void Charco::Draw() {
    Window::Draw(Assets::charcoTexture, pos)
		.withOrigin(Assets::charcoTexture->w/2, 0);
    
    if(Debug::Draw) {
		Bounds().DebugDraw(255,0,0);
	}
}

Window::PartialDraw Charco::PartialDraw() {
	return Window::PartialDraw(Assets::tileTexture, pos)
		.withOrigin(Assets::tileTexture->w / 2, Assets::tileTexture->h);
}