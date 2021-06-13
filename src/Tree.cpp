#include "Tree.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "window_conf.h"


Tree::Tree(vec aPos): BoxEntity(aPos, vec(100, 100)) {
}
Tree::~Tree() {
}

void Tree::Draw() {
    Window::Draw(Assets::treeTexture, pos)
		.withOrigin(Assets::treeTexture->w/2, 0);
    
    if(Debug::Draw) {
		Bounds().DebugDraw(255,0,0);
	}
}