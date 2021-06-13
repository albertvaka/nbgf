#include "Tile.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "window_conf.h"
#include "EnvironmentObject.h"


Tile::Tile(vec aPos): EnvironmentObject(aPos) {
}
Tile::~Tile() {
}

void Tile::Draw() {
    Window::Draw(Assets::tileTexture, pos)
		.withOrigin(Assets::tileTexture->w/2, 0);
    
    if(Debug::Draw) {
		Bounds().DebugDraw(255,0,0);
	}
}

Window::PartialDraw Tile::PartialDraw() {
    return Window::PartialDraw(Assets::tileTexture, pos)
			.withOrigin(Assets::tileTexture->w/2, Assets::tileTexture->h);
}