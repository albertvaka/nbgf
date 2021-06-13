#include "City.h"
#include "Canister.h"
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
    mCanisters.push_back(new Canister(vec(600,200)));
    mCanisters.push_back(new Canister(vec(400,600)));
    
    mLights.push_back(new Light(vec(500,700)));
    mBushs.push_back(new Bush(vec(200,700)));
    mCharcos.push_back(new Charco(vec(100,400)));
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
    for(Canister* t : mCanisters) {
        delete t;
    }
    for(Light* t : mLights) {
        delete t;
    }
    for(Bush* t : mBushs) {
        delete t;
    }
    for(Charco* t : mCharcos) {
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
    for(Canister* t : mCanisters) {
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
    for(Canister* t : mCanisters) {
        auto tpds = t->PartialDraws();
        shadows.push_back(tpds.first);
        sprites.push_back(tpds.second);
        t->Bounds().DebugDraw(255,0,0);
    }
    for(Light* t : mLights) {
        auto tpds = t->PartialDraws();
        shadows.push_back(tpds.first);
        sprites.push_back(tpds.second);
        t->Bounds().DebugDraw(255,0,0);
    }
    for(Bush* t : mBushs) {
        auto tpds = t->PartialDraws();
        shadows.push_back(tpds.first);
        sprites.push_back(tpds.second);
        t->Bounds().DebugDraw(255,0,0);
    }
    for(Charco* t : mCharcos) {
        sprites.push_back(t->PartialDraw());
        t->Bounds().DebugDraw(255,0,0);
    }
    p.first = shadows;
    p.second = sprites;
    return p;
}