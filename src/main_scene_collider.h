#pragma once

#include "collider.h"
#include "player.h"
#include "cadaver.h"
#include "mesa.h"
#include "cinta.h"
#include "spawner.h"
#include "popupText.h"
#include "lever.h"
#include "door.h"
#include "cleaner.h"


void collision_player_extremity(Player* player, Extremity* extremity) {
	if (!player->isCarrying && !extremity->isCarried && !extremity->isLet) {
		player->extremity = extremity;
	}
}

void collision_player_cadaver(Player* player, Cadaver* cadaver) {
	if (!player->isCarrying && !cadaver->isCarried && !cadaver->isLet) {
		player->cadaver = cadaver;
	}
}

void collision_player_mesa(Player* player, Mesa* mesa) {
	player->mesa = mesa;
}

void collision_player_collector(Player* player, Collector* coll) {
	player->collector = coll;
}

void collision_player_lever(Player* player, Lever* lever) {
	player->lever = lever;
	lever->player = player;
}

void collision_entity_cinta(Cintable *ent, Cinta* cinta) {

	if (ent->prevCintaDirection == cinta->dir || ent->prevCintaDirection == EntityDirection::NONE) {
		ent->currCintaDirection = cinta->dir;
	}

}

void collision_cadaver_spawner(Cadaver* ent, Detector* detector) {
	if (detector->spawner)
	{
		detector->spawner->empty = false;
	}
}

void collision_entity_despawner(Entity* e, Despawner* _) {
	e->alive = false;
}

void collision_cadaver_despawner(Cadaver* e, Despawner* _)
{
	if (e->alive)
	{
		if(e->IsOk())
		{
			new TextMolest(vec(Window::GAME_WIDTH/2, Window::GAME_HEIGHT/2), TextMolest::GOOD);
		}
		else
		{
			new TextMolest(vec(Window::GAME_WIDTH / 2, Window::GAME_HEIGHT / 2), TextMolest::BAD);
		}
	}
	e->alive = false;

}

void collision_clean_taques(Taca* t, Cleaner* c)
{
	c->Absorbed();
	t->AbsorbByRoomba(c);
}

void collision_stop_cleaner(Player* _, Cleaner* c)
{
	c->speed.x = 0;
	c->speed.y = 0;

	c->pos = c->oldPos;
	c->stuck = true;

}

void coll_ent_doorsensor(Entity* _, DoorSensor* ds)
{
	ds->activated = true;
}


void coll_bullet_cadaver(Bullet* b, Cadaver* c) {
	if (c->isLet || c->isCarried) {
		return;
	}
	c->alive = false;
	b->explode = true;
	b->pos = c->pos;
	for (int i = 0; i < 100; i++) {
		c->SpawnBlood();
	}
}

void main_scene_collisions()
{
	for (Player* player : SelfRegister<Player>::GetAll())
	{
		if (!player->isCarrying) {
			player->cadaver = NULL;
		}
		if (!player->isCarrying) {
			player->extremity = NULL;
		}
		player->collector = NULL;
		player->mesa = NULL;
		player->lever = NULL;
	}
	collide(SelfRegister<Taca>::GetAll(), SelfRegister<Cleaner>::GetAll(), collision_clean_taques);
	collide(SelfRegister<Player>::GetAll(), SelfRegister<Cleaner>::GetAll(), collision_stop_cleaner);
	collide(SelfRegister<Player>::GetAll(), SelfRegister<Extremity>::GetAll(), collision_player_extremity);
	collide(SelfRegister<Player>::GetAll(), SelfRegister<Cadaver>::GetAll(), collision_player_cadaver);
	collide(SelfRegister<Player>::GetAll(), SelfRegister<Mesa>::GetAll(), collision_player_mesa);
	collide(SelfRegister<Player>::GetAll(), SelfRegister<Lever>::GetAll(), collision_player_lever);
	collide(SelfRegister<Player>::GetAll(), SelfRegister<Collector>::GetAll(), collision_player_collector);
	//collide(SelfRegister<Player>::GetAll(), SelfRegister<Cinta>::GetAll(), collision_entity_cinta);
	collide(SelfRegister<Cintable>::GetAll(), SelfRegister<Cinta>::GetAll(), collision_entity_cinta);
	collide(SelfRegister<Cadaver>::GetAll(), SelfRegister<Detector>::GetAll(), collision_cadaver_spawner);
	collide(SelfRegister<Cadaver>::GetAll(), SelfRegister<Despawner>::GetAll(), collision_cadaver_despawner);
	collide(SelfRegister<Extremity>::GetAll(), SelfRegister<Despawner>::GetAll(), collision_entity_despawner);
	collide(SelfRegister<Cleaner>::GetAll(), SelfRegister<Despawner>::GetAll(), collision_entity_despawner);
	collide(SelfRegister<Player>::GetAll(), SelfRegister<DoorSensor>::GetAll(), coll_ent_doorsensor);
	collide(SelfRegister<Cleaner>::GetAll(), SelfRegister<DoorSensor>::GetAll(), coll_ent_doorsensor);
	collide(SelfRegister<Bullet>::GetAll(), SelfRegister<Cadaver>::GetAll(), coll_bullet_cadaver);
}
