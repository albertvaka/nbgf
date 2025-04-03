#include "particles.h"
#include "assets.h"

namespace Particles {

void InitWaterTrail(PartSys& ps)
{
	ps.SetTexture(Assets::backgroundTexture);
	ps.AddSprite({ 0,0,15,15});

	ps.min_interval = 0.02f;
	ps.max_interval = 0.05f;

	ps.min_ttl = 1.f;
	ps.max_ttl = 3.f;

	ps.alpha = 0.7f;

	ps.min_vel.x = -50.f;
	ps.max_vel.x = 50.f;

	ps.min_vel.y = -50.f;
	ps.max_vel.y = 50.f;

	ps.min_scale = 1.0f;
	ps.max_scale = 1.5f;

	ps.scale_vel = -0.8f;
}

void Init()
{
	if (waterTrail.texture == nullptr)
	{
		InitWaterTrail(waterTrail);
	}
}

void UpdateAll(float dt)
{
	waterTrail.UpdateParticles(dt);
}

void ClearAll()
{
	waterTrail.Clear();
}

}
