#include "jumpman.h"
#include "input.h"

// accel
const float run_acc = 1400;
const float run_acc_onair = 400;
const float gravity_acc = 600;

// friction X
const float fri_acc_floor = 1000;
const float fri_acc_floor_crouched = 450;
const float fri_acc_air = 1000 / 8.f;

// friction Y
const float fri_acc_wall_up = 1000;
const float fri_acc_wall_down = 450;

// jump
const float vel_jump = -150;
const float vel_walljump = 130;

const vec vel_max(225, 200);

JumpMan::JumpMan(TileMap* _map)
	: acc(0, 0)
	, vel(0, 0)
	, jumpTimeLeft(0.0f)
	, crouched(false)
	, map(_map)
{
	animation.Ensure(MARIO_IDLE);
	sf::Rect rect = animation.CurrentFrame();
	siz = vec(rect.width, rect.height);
	cen = siz / 2;
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
	float marginGrounded = 3.f; //in pixels
	bool grounded = map->isCollInWorldCoordinates(pos.x - cen.x + 1, pos.y + marginGrounded) || map->isCollInWorldCoordinates(pos.x + cen.x - 1, pos.y + marginGrounded);

	acc = vec(0,0);
	crouched = ((crouched||grounded) && Keyboard::IsKeyPressed(GameKeys::DOWN)) || (crouched&&!grounded);
	if (Keyboard::IsKeyPressed(GameKeys::LEFT)) {
		lookingLeft = true;
		if (grounded) {
			if (!crouched) acc.x -= run_acc;
		}
		else acc.x -= run_acc_onair;
	}
	if (Keyboard::IsKeyPressed(GameKeys::RIGHT)) {
		lookingLeft = false;
		if (grounded) {
			if (!crouched) acc.x += run_acc;
		}
		else acc.x += run_acc_onair;
	}

	//Si en el frame anterior estaba tocando el suelo, inicializando
	//jumpTimeLeft a mas de 0 permite al jugador saltar durante ese rato
	if (Keyboard::IsKeyJustPressed(GameKeys::UP)) {
		std::cout << grounded << " " << pos << std::endl;
	}
	if (Keyboard::IsKeyJustPressed(GameKeys::UP) && (grounded || (onWall && !crouched)))
	{
		jumpTimeLeft = 0.35f;
		if (onWall && !grounded && !crouched) {
			vel.x = vel_walljump * -1.0f * float(onWall);
			lookingLeft = !lookingLeft;
		}
	}

	if (Keyboard::IsKeyPressed(GameKeys::UP) && jumpTimeLeft > 0)
	{
		vel.y = vel_jump;
	}
	else
	{
		jumpTimeLeft = 0; //A la que deja de pulsarse el boton de saltar cae de inmediato
	}

	if (jumpTimeLeft > 0)
	{
		jumpTimeLeft -= dt; //Se le resta el tiempo mientras salta
	}
	else
	{
		acc.y += gravity_acc; //La gravedad afecta mientras no salte
	}

	//FRICTION
	vec fri = vec(0,0);
	if (grounded)
	{
		if (crouched) 
		{
			fri.x = fri_acc_floor_crouched;
		}
		else 
		{
			fri.x = fri_acc_floor;
		}
	}
	else
	{
		fri.x = fri_acc_air;
	}

	if (!crouched && onWall)
	{
		if (vel.y < 0) { //Vamos hacia arriba
			fri.y = fri_acc_wall_up;
		} else {
			fri.y = fri_acc_wall_down;
		}
	}

	// Apply friction
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

	vel = vel + acc * dt;

	//Hacemos clamp de las velocidades
	if (vel.x > vel_max.x) vel.x = vel_max.x;
	if (vel.x < -vel_max.x) vel.x = -vel_max.x;
	if (vel.y > vel_max.y) vel.y = vel_max.y;
	if (vel.y < -vel_max.y) vel.y = -vel_max.y;

	//uniformly accelerated linear motion
	vec pos0 = pos;
	vec posf = pos0 + vel * dt; //posicion final
	std::cout << abs(pos0.x - posf.x) / dt << std::endl;

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
	vec csiz = siz - cen;
	vec direction = posf - pos0;
	const int N = 1;
	if (direction.y < 0) //Vamos hacia arriba
	{
		int yo = map->tilePosY(pos0.y - siz.y); // usamos la y superior del sprite
		int yn = map->tilePosY(posf.y - siz.y);
		int xl = map->tilePosX(pos0.x - cen.x + N);
		int xr = map->tilePosX(pos0.x + csiz.x - N);
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
		int xl = map->tilePosX(pos0.x - cen.x + N);
		int xr = map->tilePosX(pos0.x + csiz.x - N);
		for (int y = yo; y <= yn; y++)
		{
			for (int x = xl; x <= xr; x++)
			{
				if (map->isColl(x, y))
				{
					posf.y = map->Bottom(y);
					vel.y = 0;
					goto vert_exit;
				}
			}
		}
	}

vert_exit:
	if (direction.x < 0) //Vamos hacia la izquierda
	{
		int xo = map->tilePosX(pos0.x - cen.x);
		int xn = map->tilePosX(posf.x - cen.x);
		int yTop = map->tilePosY(pos0.y - siz.y + N);
		int yBottom = map->tilePosY(pos0.y - N);
		for (int x = xo; x >= xn; x--)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				if (map->isColl(x, y))
				{
					posf.x = map->Right(x) + cen.x;
					vel.x = -3.f; //stay against wall
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
		int yTop = map->tilePosY(pos0.y - siz.y + N);
		int yBottom = map->tilePosY(pos0.y - N);
		for (int x = xo; x <= xn; x++)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				if (map->isColl(x, y))
				{
					posf.x = map->Left(x) - csiz.x;
					vel.x = 3.f; //stay against wall
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
