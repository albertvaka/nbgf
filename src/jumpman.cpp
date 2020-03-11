#include "jumpman.h"
#include "input.h"

JumpMan::JumpMan(TileMap* map) : Character(map)
{
	ensureAnim("Idle");
	jumpTimeLeft = 0.0f;
	crouched = false;
}

void JumpMan::Update(float GameTime)
{
	acc = vec(0,0);
	crouched = ((crouched||grounded) && Keyboard::IsKeyPressed(GameKeys::DOWN)) || (crouched&&!grounded);
	if (Keyboard::IsKeyPressed(GameKeys::LEFT)) {
		mirror = true;
		if (grounded) {
			if (!crouched) acc -= run_acc;
		}
		else acc -= air_acc;
	}
	if (Keyboard::IsKeyPressed(GameKeys::RIGHT)) {
		mirror = false;
		if (grounded) {
			if (!crouched) acc += run_acc;
		}
		else acc += air_acc;
	}

	//FRICTION
	fri = vec(0,0);
	if (grounded)
	{
		if (crouched) fri.x = fri_acc.x/2;
		else          fri.x = fri_acc.x;
	}
	else
	{
		fri.x = fri_acc.x/8;
	}

	if (!crouched && onWall)
	{
		fri.y = fri_acc.y;
	}

	//Si en el frame anterior estaba tocando el suelo, inicializando
	//jumpTimeLeft a mas de 0 permite al jugador saltar durante ese rato
	if (Keyboard::IsKeyPressed(GameKeys::UP) && (grounded||(onWall&&!crouched)))
	{
		jumpTimeLeft = 0.35f;
		if (onWall && !grounded && !crouched) {
			vel.x = vel_jmp.x * -1.0f * float(onWall);
		}
	}

	if (Keyboard::IsKeyPressed(GameKeys::UP) && jumpTimeLeft > 0)
		vel.y = vel_jmp.y;
	else //A la que deja de pulsarse el boton de saltar cae de inmediato
		jumpTimeLeft = 0;

	Character::Update(GameTime);

	if (crouched)
	{
		ensureAnim("Crouch");
	}
	else
	{
		if (grounded)
		{
			if (Keyboard::IsKeyPressed(GameKeys::LEFT) && !Keyboard::IsKeyPressed(GameKeys::RIGHT))
			{
				if (vel.x > 0) ensureAnim("Turn");
				else ensureAnim("Walk");
			}
			else if (Keyboard::IsKeyPressed(GameKeys::RIGHT) && !Keyboard::IsKeyPressed(GameKeys::LEFT))
			{
				if (vel.x < 0) ensureAnim("Turn");
				else ensureAnim("Walk");
			}
			else
			{
				ensureAnim("Idle");
			}
		}
		else
		{
			if (onWall) ensureAnim("OnWall");
			else ensureAnim("Jump");
		}
	}
}
