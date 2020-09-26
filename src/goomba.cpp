#include "goomba.h"

#include "window.h"
#include "assets.h"
#include "tilemap.h"
#include "rand.h"

Goomba::Goomba(const vec& pos) : BoxEntity(pos - vec(0,8), {12, 12}), anim(AnimLib::GOOMBA)
{
	goingRight = Rand::OnceEach(2);
}

const float speed = 25;

void Goomba::Update(float dt)
{
	float goingRightDir = (goingRight ? 1 : -1);

	float new_pos_x = pos.x + dt * speed * goingRightDir;
	
	TileMap* tm = TileMap::instance();

	GPU_Rect r = anim.GetCurrentRect();
	veci tilePos = TileMap::toTiles(new_pos_x + r.w/2 * goingRightDir, pos.y + r.h);

	if (tm->isSolid(tilePos) || tm->getTile(tilePos).isOneWay())
	{
		pos.x = new_pos_x;
	}
	else
	{
		goingRight = !goingRight;
	}

	anim.Update(dt);
}


void Goomba::Draw() const
{
	pos.Debuggerino();
	GPU_Rect r = anim.GetCurrentRect();
	Window::Draw(Assets::marioTexture, pos-vec(r.w/2,r.h/2))
		.withRect(r);
	if (Debug::Draw) {
		bounds().Draw();
	}
}