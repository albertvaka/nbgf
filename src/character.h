#pragma once

#include "vector.h"
#include "tilemap.h"

const vec run_acc(1400, 0); //aceleracion corriendo
const vec air_acc(400, 0);  //aceleracion en el aire
const vec fri_acc(1000, 800); //aceleracion de la friccion
const vec gra_acc(0, -600); //aceleracion de la gravedad
const vec vel_max(225, 300); //velocidad maxima que adquiere un personaje
const vec vel_jmp(225, 225); //velocidad que adquiere un personaje al saltar

class Character
{
public:
	Character(TileMap* map);
	void Update(float GameTime);
	void Reset();
	vec pos;

protected:
	vec acc;
	vec vel;
    vec fri;

	bool grounded;
	char onWall;
	enum {ONWALL_LEFT = -1, ONWALL_NO = 0, ONWALL_RIGHT = 1};
	float jumpTimeLeft;

	void ensureAnim(std::string name);
	std::string lastAnim;

private:
	vec siz;
	vec cen;
	TileMap* map;
};
