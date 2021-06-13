#include "City.h"
#include "Canister.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "rand.h"
#include "Tree.h"
#include "Chain.h"
#include "vec.h"
#include "window_conf.h"


City::City(){
    //mEntities.push_back(new Tree(vec(100, 100)));
    /*
    mTrees.push_back(new Tree(vec(200,200)));
    mTrees.push_back(new Tree(vec(500,400)));
    mCanisters.push_back(new Canister(vec(600,200)));
    mCanisters.push_back(new Canister(vec(400,600)));
    
    mLights.push_back(new Light(vec(500,700)));
    mBushs.push_back(new Bush(vec(200,700)));
    mCharcos.push_back(new Charco(vec(100,400)));
    mTiles.push_back(new Tile(vec(800,700)));
    */
    const int bushCount = Rand::roll(0, 70);
    for(int i = 0; i < bushCount; i++) {
        const float lSeparation = 1.4;
        const int groups = Rand::roll(2, 10);
        if(i % groups == 0 || i % groups == 1 || i % groups == 2)
            continue;
        collisionObjects.push_back(new Bush(vec(500+i*85*lSeparation,i*50*lSeparation)));
    }
    const int canisterCount = Rand::roll(0, 7);
    for(int i = 0; i < canisterCount; i++) {
        const float lSeparation = 9.5;
        collisionObjects.push_back(new Canister(vec(200+i*85*lSeparation,i*50*lSeparation)));
    }
    for(int i = 0; i < 15; i++) {
        const float lSeparation = 6;
        collisionObjects.push_back(new Light(vec(200+i*85*lSeparation,i*50*lSeparation)));
    }
    for(int i = 0; i < 50; i++) {
        noCollisionObjects.push_back(new Tile(vec(300+i*85,i*50)));
    }
    const int treeCount = Rand::roll(0, 10);
    for(int i = 0; i < treeCount; i++) {
        collisionObjects.push_back(new Tree(Rand::VecInRange(0,0,Window::GAME_WIDTH, Window::GAME_HEIGHT)));
    }
    const int charcoCount = Rand::roll(0, 3);
    for(int i = 0; i < charcoCount; i++) {
        noCollisionObjects.push_back(new Charco(Rand::VecInRange(0,0,Window::GAME_WIDTH, Window::GAME_HEIGHT)));
    }
}
City::~City() {

    for (EnvironmentObject* object : collisionObjects) {
        delete object;
    }

    for (EnvironmentObject* object : noCollisionObjects) {
        delete object;
    }
}

void City::Update(float dt, Chain* chain)
{
    for (EnvironmentObject* object : collisionObjects) {
        chain->CheckCollisionWithEnvironment(object);
    }
}

void City::Draw() {

}

std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> City::PartialDraws() {
    std::pair<std::vector<Window::PartialDraw>, std::vector<Window::PartialDraw>> p;
    std::vector<Window::PartialDraw> shadows;
    std::vector<Window::PartialDraw> sprites;


    for (EnvironmentObject* object : collisionObjects) {
        auto tpds = object->PartialDraws();
        shadows.push_back(tpds.first);
        sprites.push_back(tpds.second);
        object->Bounds().DebugDraw(255, 0, 0);
    }

    for(EnvironmentObject* object : noCollisionObjects) {
    shadows.push_back(object->PartialDraw());
    object->Bounds().DebugDraw(255,0,0);
    }


    p.first = shadows;
    p.second = sprites;
    return p;
}