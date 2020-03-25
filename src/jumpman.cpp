#include "jumpman.h"
//Based on the work of: Jordi Santiago

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
const float jump_time = 0.35f;

const vec vel_max(225, 200);

JumpMan::JumpMan(TileMap* _map)
	: vel(0, 0)
	, jumpTimeLeft(0.0f)
	, crouched(false)
	, map(_map)
{
	animation.Ensure(MARIO_IDLE);
	sf::Rect rect = animation.CurrentFrame();
	siz = vec(rect.width, rect.height);
	cen = siz / 2;
	InitPolvito();
}

void JumpMan::Draw(sf::Sprite& spr, sf::RenderTarget& window) {
	polvito.Draw(window);
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
	float marginGrounded = 1.f; //in pixels
	bool grounded = map->isCollInWorldCoordinates(pos.x - cen.x + 1, pos.y + marginGrounded) || map->isCollInWorldCoordinates(pos.x + cen.x - 1, pos.y + marginGrounded);

	crouched = ((crouched || grounded) && Keyboard::IsKeyPressed(GameKeys::DOWN)) || (crouched && !grounded);

	//Si en el frame anterior estaba tocando el suelo, inicializando
	//jumpTimeLeft a mas de 0 permite al jugador saltar durante ese rato
	if (Keyboard::IsKeyJustPressed(GameKeys::UP, 0.15f) && (grounded || (onWall && !crouched)))
	{
		Keyboard::ConsumeJustPressed(GameKeys::UP);
		//if (!Keyboard::IsKeyJustPressed(GameKeys::UP)) std::cout << "cheats" << std::endl;
		jumpTimeLeft = jump_time;
		if (onWall && !grounded && !crouched) {
			vel.x = vel_walljump * -1.0f * float(onWall);
			lookingLeft = !lookingLeft;
			DoPolvitoWallJump();
		}
		else {
			DoPolvitoJump();
		}
	}

	vec acc = vec(0, 0);
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
	vec fri = vec(0, 0);
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
		}
		else {
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
	//std::cout << abs(pos0.x - posf.x) / dt << std::endl;

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
		//No collision up
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
					DoPolvitoLand();
					vel.y = 0;
					goto vert_exit;
				}
			}
		}
		//No collision down
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
					lookingLeft = true;
					goto horz_exit;
				}
			}
		}
		//No collision left
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
					lookingLeft = false;
					goto horz_exit;
				}
			}
		}
		//No collision right
	}
	onWall = ONWALL_NO;

horz_exit:
	pos = posf; //asignamos la posicion final a pos

	animation.Update((int)(dt * 1000));

	bool isWalking = false;
	bool isTurning = false;
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
				isWalking = true;
				if (vel.x > 0) {
					animation.Ensure(MARIO_TURN);
					isTurning = true;
				}
				else {
					animation.Ensure(MARIO_WALK);
				}
			}
			else if (Keyboard::IsKeyPressed(GameKeys::RIGHT) && !Keyboard::IsKeyPressed(GameKeys::LEFT))
			{
				isWalking = true;
				if (vel.x < 0) {
					animation.Ensure(MARIO_TURN);
					isTurning = true;
				}
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
	if (isWalking) {
		DoPolvitoRun(dt, acc, isTurning);
	}
	polvito.UpdateParticles(dt);
}







// PARTICULITAS

void JumpMan::InitPolvito() {
	polvito.min_interval = 0.02f;
	polvito.max_interval = 0.05f;

	polvito.min_ttl = 0.1f;
	polvito.max_ttl = 0.4f;

	polvito.min_vel.x = 9; //it's swapped when changing direction
	polvito.max_vel.x = 18;

	polvito.min_vel.y = -50.f;
	polvito.max_vel.y = -20.f;

	polvito.acc.y = 120.f;

	polvito.min_scale = 1.0f;
	polvito.max_scale = 1.8f;
	polvito.scale_vel = -3.5f;

	polvito.min_rotation = 0.f;
	polvito.max_rotation = 360.f;
	polvito.rotation_vel = 160.f;

	polvito.alpha_vel = -1.8f;
}

inline void JumpMan::DoPolvitoJump() {
	// Pluf cap als dos costats
	polvito.pos = pos + vec(-1.5f, -1.5f);
	if (polvito.min_vel.x > 0) {
		polvito.FlipX();
	}
	polvito.AddParticle(2);
	polvito.pos = pos + vec(1.5f, -1.5f);
	polvito.FlipX();
	polvito.AddParticle(2);
}

inline void JumpMan::DoPolvitoWallJump() {
	// Pluf cap als dos costats
	if (vel.x > 0) {
		polvito.pos = pos + vec(-7.f, -16.f);
	}
	else {
		polvito.pos = pos + vec(7.f, -16.f);
	}

	if (vel.x < 0 && polvito.min_vel.x > 0 || vel.x > 0 && polvito.min_vel.x < 0) {
		polvito.FlipX();
	}
	polvito.AddParticle(5);
}

inline void JumpMan::DoPolvitoLand() {

	if (vel.y > 50) {
		// Pluf cap als dos costats
		polvito.pos = pos + vec(-8.f, -0.5f);
		if (polvito.min_vel.x > 0) {
			polvito.FlipX();
		}
		polvito.AddParticle(3);
		polvito.pos = pos + vec(8.f, -0.5f);
		polvito.FlipX();
		polvito.AddParticle(3);
	}
}

inline void JumpMan::DoPolvitoRun(float dt, vec acc, bool isTurning) {
	if (acc.x < 0) {
		polvito.pos = pos + vec(4.f, -0.5f);
		if (polvito.min_vel.x < 0) {
			polvito.FlipX();
		}
	}
	else {
		polvito.pos = pos + vec(-4.f, -0.5f);
		if (polvito.min_vel.x > 0) {
			polvito.FlipX();
		}
	}
	if (isTurning) {
		polvito.AddParticle(2);
	}
	polvito.Spawn(dt);
}
