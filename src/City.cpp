#include "City.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "Tree.h"
#include "vec.h"
#include "window_conf.h"


City::City(): mTrees() {
    //mEntities.push_back(new Tree(vec(100, 100)));
    mTrees.push_back(new Tree(vec(200,200)));
    mTrees.push_back(new Tree(vec(500,400)));
}
City::~City() {
    /*
    for(auto* e : mEntities) {
        delete e;
    }
    */
    for(Tree* t : mTrees) {
        delete t;
    }
}

void City::Draw() {
    /*
    for(auto& e : mEntities) {
        e->Draw();
    }
    */
    for(Tree* t : mTrees) {
        t->Draw();
    }
}

std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> City::PartialDraws() {
    std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> p;
    std::vector<Window::PartialDraw> shadows;
    std::vector<Window::PartialDraw> sprites;
    /*
    for(auto& e : mEntities) {
        e->Draw();
    }
    */
    for(Tree* t : mTrees) {
        auto tpds = t->PartialDraws();
        shadows.push_back(tpds.first);
        sprites.push_back(tpds.second);
        t->Bounds().DebugDraw(255,0,0);
    }
    p.first = shadows;
    p.second = sprites;
    return p;
}