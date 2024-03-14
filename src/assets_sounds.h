#pragma once

#include "sound.h"

typedef struct _Mix_Music Mix_Music;

namespace Assets
{
	inline Sound soundSlimyDead;
	inline Sound soundSlimyAttack;
	inline MultiSound<2> soundLeaves;
	inline MultiSound<4> soundStep;
	inline MultiSound<2> soundSword;
	inline MultiSound<2> soundMissile;
	inline MultiSound<2> enemyDies;
	inline MultiSound<3> batScreech;
	inline MultiSound<2> lavaBigSplash;
	inline MultiSound<2> lavaSplash;
	inline MultiSound<3> shoot;
	inline MultiSound<2> heal;
	inline Sound breakBlock;
	inline Mix_Music* musicHellcross;
};

