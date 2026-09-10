#include "tower.h"

const float towerRadius = 30.f;
const float towerSpacing = 15.0f;
const float towerProbability = 0.0001f;

#include "entity.h"
#include "assets.h"
#include "window_draw.h"
#include "debug.h"
#include "camera.h"
#include "chunks.h"
#include "tweeny.h"
#include "collide.h"
#include <cmath>
#include "tower.h"
#include "rock.h"
#include "particles.h"
#include "imgui.h"

const float kTowerRadius = 30.f;

Tower::Tower(vec pos)
 : CircleEntity(pos, kTowerRadius)
{
	new Rock(pos);
}

void Tower::Update(float dt) {

}

static double hash(float x, float y) {
    std::uint32_t h = static_cast<std::uint32_t>(x) * 374761393u + static_cast<std::uint32_t>(y) * 668265263u;
    h = (h ^ (h >> 13)) * 1274126177u;
    h ^= h >> 16;
    return static_cast<double>(h) / 4294967296.0;
}

void Tower::Draw() {
	bool hitAnim = false; // (immunityTimer > 0 && ((int)((immunityTimer) * 12)) % 3);

	if (hitAnim) {
		Assets::tintShader.Activate();
		Assets::tintShader.SetUniform("flashColor", 1.f, 0.5f, 0.5f, 0.5f);
	}

	Window::Draw(Assets::towerTexture, pos)
		.withOrigin(Assets::towerTexture->w/2, Assets::towerTexture->h*4/5.f)
		.withScale(0.66f);

	Assets::tintShader.Deactivate();

	Bounds().DebugDraw();
}



static bool CanSpawn(float x, float y)
{
    for (Tower* tower : Tower::GetAll()) {
        if (tower->pos.DistanceSq(vec(x, y)) < 8*towerRadius*towerRadius) {
            return false;
        }
    }
    for (Rock* rock : Rock::GetAll()) {
        if (rock->Position().DistanceSq(vec(x, y)) < 8*towerRadius*towerRadius) {
            return false;
        }
    }

    return true;
}

void Tower::SpawnInChunk(const BoxBounds& bounds)
{
    for (float x = bounds.Left(); x < bounds.Right(); x += towerSpacing) {
        for (float y = bounds.Top(); y < bounds.Bottom(); y += towerSpacing) {
            if (hash(x, y) < towerProbability && CanSpawn(x, y)) {
                new Tower(vec(x, y));
            }
        }
    }
}

void Tower::DespawnFarFromChunk(veci currentChunk, int distance)
{
    std::vector<Tower*>& towers = Tower::GetAll();
    for (int i = towers.size() - 1; i >= 0; i--) {
        Tower* tower = towers[i];
        veci towerChunk = Chunks::GetChunk(tower->pos);
        if (std::abs(towerChunk.x - currentChunk.x) >= distance || std::abs(towerChunk.y - currentChunk.y) >= distance) {
            delete tower;
        }
    }
}


