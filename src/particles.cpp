#include "particles.h"
#include "assets.h"

namespace Particles {

void InitWaterTrail(PartSys& ps)
{
	ps.SetTexture(Assets::particlesTexture);
	ps.AddSprite({ 0,0,32,32 });
	ps.AddSprite({ 64,0,32,32 });

	ps.min_interval = 0.01f;
	ps.max_interval = 0.03f;

	ps.min_ttl = 1.f;
	ps.max_ttl = 3.f;

	ps.alpha = 0.7f;

	ps.min_vel.x = -50.f;
	ps.max_vel.x = 50.f;

	ps.min_vel.y = -50.f;
	ps.max_vel.y = 50.f;

	ps.min_scale = 0.7f;
	ps.max_scale = 0.9f;

	ps.min_rotation = 0;
	ps.max_rotation = 360;

	ps.scale_vel = -0.6f;

	ps.color = { 178, 204, 228, 255 };
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
