#include "jumpman.h"
#include "input.h"

const vec run_acc(1400, 0); //aceleracion corriendo
const vec air_acc(400, 0);  //aceleracion en el aire
const vec fri_acc(1000, 450); //aceleracion de la friccion
const vec gra_acc(0, 600); //aceleracion de la gravedad
const vec vel_max(225, 200); //velocidad maxima que adquiere un personaje
const vec vel_jmp(150, -150); //velocidad que adquiere un personaje al saltar


JumpMan::JumpMan(TileMap* _map)
	: acc(0, 0)
	, vel(0, 0)
	, siz(16, 32)
	, cen(siz/2)
	, jumpTimeLeft(0.0f)
	, grounded(false)
	, crouched(false)
	, map(_map)
{
	animation.Ensure(MARIO_IDLE);
}

void JumpMan::Draw(sf::Sprite& spr, sf::RenderTarget& window) {
	spr.setTextureRect(animation.CurrentFrame());
	if (lookingLeft) {
		spr.setScale(-1.f, 1.f);
		spr.setPosition(pos.x + cen.x, pos.y - siz.y);
	}
	else {
		spr.setScale(1.f, 1.f);
		spr.setPosition(pos.x - cen.x, pos.y - siz.y);
	}
	window.draw(spr);
	spr.setScale(1.f, 1.f);
}

void JumpMan::Update(float dt)
{
	acc = vec(0,0);
	crouched = ((crouched||grounded) && Keyboard::IsKeyPressed(GameKeys::DOWN)) || (crouched&&!grounded);
	if (Keyboard::IsKeyPressed(GameKeys::LEFT)) {
		lookingLeft = true;
		if (grounded) {
			if (!crouched) acc -= run_acc;
		}
		else acc -= air_acc;
	}
	if (Keyboard::IsKeyPressed(GameKeys::RIGHT)) {
		lookingLeft = false;
		if (grounded) {
			if (!crouched) acc += run_acc;
		}
		else acc += air_acc;
	}

	//FRICTION
	vec fri = vec(0,0);
	if (grounded)
	{
		if (crouched) 
		{
			fri.x = fri_acc.x / 2;
		}
		else 
		{
			fri.x = fri_acc.x;
		}
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
	if (Keyboard::IsKeyJustPressed(GameKeys::UP) && (grounded||(onWall&&!crouched)))
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

	//obtenemos las constantes de World

	if (fri.x != 0) //friccion
	{
		if (vel.x < 0)
		{
			vel.x += fri.x * dt;
			if (vel.x > 0) vel.x = 0;
		}
		else if (vel.x > 0)
		{
			vel.x -= fri.x * dt;
			if (vel.x < 0) vel.x = 0;
		}

	}
	if (fri.y != 0)
	{
		if (vel.y < 0)
		{
			vel.y += fri.y * dt;
			if (vel.y > 0) vel.y = 0;
		}
		else if (vel.y > 0)
		{
			vel.y -= fri.y * dt;
			if (vel.y < 0) vel.y = 0;
		}

	}

	if (jumpTimeLeft <= 0) acc += gra_acc; //La gravedad afecta mientras no salte
	else jumpTimeLeft -= dt; //Se le resta el tiempo mientras salta
	vec pos0 = pos; //pos0: posicion inicial

	//uniformly accelerated linear motion, posf: posicion final
	vec posf = pos0 + vel * dt + acc * dt * dt * 0.5f;
	vel = vel + acc * dt;

	//Hacemos clamp de las velocidades
	if (vel.x > vel_max.x) vel.x = vel_max.x;
	if (vel.x < -vel_max.x) vel.x = -vel_max.x;
	if (vel.y > vel_max.y) vel.y = vel_max.y;
	if (vel.y < -vel_max.y) vel.y = -vel_max.y;


	vec csiz = siz - cen;
	//Obtenemos el vector direccion para saber hacia donde nos dirigimos

	// El funcionamiento de las colisiones es el siguiente:
	// Aun que tengamos una posicion continua del personaje tenemos posiciones discretas
	// donde mirar si hay tiles que colisionan o no. El algoritmo va por pasos, primero
	// abanzaremos en la coordenada Y, y luego en la X.
	//
	// Cuando abancemos en la Y, tendremos una Y_origen y una Y_destino, y
	// ademas el personajes es posible que este ocupando un rango de varias X
	// si es lo suficiente ancho o simplemente esta entre dos tiles, por tanto
	// tambien tenemos unas X_izquierda y X_derecha.
	// Ahora lo unico que hay que hacer es, para cada y que hay entre Y_origen
	// e Y_destino, y para cada x entre X_izquierda y X_derecha miramos si en
	// la posicion x,y del tilemap hay un tile colisionable. Si lo hay es que
	// nuestro personaje se va a chochar. Lo aclamos a la posicion de la colision
	// y le quitamos su velocidad Y. En caso de que estuviesemos cayendo
	// significa que estaremos grounded, en caso de subir significia que estabamos
	// saltando y que ya no podremos saltar mas.
	vec direction = posf - pos0;
	if (direction.y < 0) //Vamos hacia arriba
	{
		grounded = false;
		int yo = map->tilePosY(pos0.y - siz.y); // usamos la y superior del sprite
		int yn = map->tilePosY(posf.y - siz.y);
		int xl = map->tilePosX(pos0.x - cen.x + 2);
		int xr = map->tilePosX(pos0.x + csiz.x - 2);
		for (int y = yo; y >= yn; y--)
		{
			for (int x = xl; x <= xr; x++)
			{
				if (map->isColl(x, y))
				{
					posf.y = map->Top(y) + siz.y;
					vel.y = 0;
					jumpTimeLeft = 0;
					goto vert_exit;
				}
			}
		}

	}
	else if (direction.y > 0) //Vamos hacia abajo
	{
		int yo = map->tilePosY(pos0.y); // usamos la y inferior del sprite
		int yn = map->tilePosY(posf.y);
		int xl = map->tilePosX(pos0.x - cen.x + 2);
		int xr = map->tilePosX(pos0.x + csiz.x - 2);
		for (int y = yo; y <= yn; y++)
		{
			for (int x = xl; x <= xr; x++)
			{
				if (map->isColl(x, y))
				{
					posf.y = map->Bottom(y);
					vel.y = 0;
					grounded = true;
					goto vert_exit;
				}
			}
		}
		grounded = false;

	}

vert_exit:
	if (direction.x < 0) //Vamos hacia la izquierda
	{
		int xo = map->tilePosX(pos0.x - cen.x);
		int xn = map->tilePosX(posf.x - cen.x);
		int yTop = map->tilePosY(pos0.y - siz.y + 2);
		int yBottom = map->tilePosY(pos0.y - 2);
		for (int x = xo; x >= xn; x--)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				if (map->isColl(x, y))
				{
					posf.x = map->Right(x) + cen.x;
					vel.x = -3.f;
					onWall = ONWALL_LEFT;
					goto horz_exit;
				}
			}
		}
	}
	else if (direction.x > 0) //Vamos hacia la derecha
	{
		int xo = map->tilePosX(pos0.x + csiz.x);
		int xn = map->tilePosX(posf.x + csiz.x);
		int yTop = map->tilePosY(pos0.y - siz.y + 2);
		int yBottom = map->tilePosY(pos0.y - 2);
		for (int x = xo; x <= xn; x++)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				if (map->isColl(x, y))
				{
					posf.x = map->Left(x) - csiz.x;
					vel.x = 3.f;
					onWall = ONWALL_RIGHT;
					goto horz_exit;
				}
			}
		}
	}
	onWall = ONWALL_NO;

horz_exit:
	pos = posf; //asignamos la posicion final a pos

	animation.Update((int)(dt*1000));

	if (crouched)
	{
		siz.y = 22;
		animation.Ensure(MARIO_CROUCH);
	}
	else
	{
		siz.y = 32;
		if (grounded)
		{
			if (Keyboard::IsKeyPressed(GameKeys::LEFT) && !Keyboard::IsKeyPressed(GameKeys::RIGHT))
			{
				if (vel.x > 0) animation.Ensure(MARIO_TURN);
				else animation.Ensure(MARIO_WALK);
			}
			else if (Keyboard::IsKeyPressed(GameKeys::RIGHT) && !Keyboard::IsKeyPressed(GameKeys::LEFT))
			{
				if (vel.x < 0) animation.Ensure(MARIO_TURN);
				else animation.Ensure(MARIO_WALK);
			}
			else
			{
				animation.Ensure(MARIO_IDLE);
			}
		}
		else
		{
			if (onWall) animation.Ensure(MARIO_ONWALL);
			else animation.Ensure(MARIO_JUMP);
		}
	}
}
