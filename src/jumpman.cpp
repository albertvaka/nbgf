#include "jumpman.h"
#include "input.h"

const vec run_acc(1400, 0); //aceleracion corriendo
const vec air_acc(400, 0);  //aceleracion en el aire
const vec fri_acc(1000, 800); //aceleracion de la friccion
const vec gra_acc(0, 600); //aceleracion de la gravedad
const vec vel_max(225, 200); //velocidad maxima que adquiere un personaje
const vec vel_jmp(150, -150); //velocidad que adquiere un personaje al saltar

JumpMan::JumpMan(TileMap* _map)
	: acc(0, 0)
	, vel(0, 0)
	, siz(1, 1)
	, jumpTimeLeft(0.0f)
	, grounded(false)
	, crouched(false)
	, map(_map)
{
	ensureAnim("Idle");
}

void JumpMan::Update(float GameTime)
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
			vel.x += fri.x * GameTime;
			if (vel.x > 0) vel.x = 0;
		}
		else if (vel.x > 0)
		{
			vel.x -= fri.x * GameTime;
			if (vel.x < 0) vel.x = 0;
		}

	}
	if (fri.y != 0)
	{
		if (vel.y < 0)
		{
			vel.y += fri.y * GameTime;
			if (vel.y > 0) vel.y = 0;
		}
		else if (vel.x > 0)
		{
			vel.y -= fri.y * GameTime;
			if (vel.y < 0) vel.y = 0;
		}

	}

	if (jumpTimeLeft <= 0) acc += gra_acc; //La gravedad afecta mientras no salte
	else jumpTimeLeft -= GameTime; //Se le resta el tiempo mientras salta
	vec pos0 = pos; //pos0: posicion inicial

	//uniformly accelerated linear motion, posf: posicion final
	vec posf = pos0 + vel * GameTime + acc * GameTime * GameTime * 0.5f;
	vel = vel + acc * GameTime;

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
	if (direction.y < 0) //Vamos hacia abajo
	{
		grounded = false;
		//le restamos a la Y la mitad de su tamaño para obtener la Y inferior del sprite
		int yo = map->tilePosY(pos0.y - csiz.y),
			yn = map->tilePosY(posf.y - csiz.y),
			xl = map->tilePosX(pos0.x - cen.x + 2),
			xr = map->tilePosX(pos0.x + csiz.x - 2);
		for (int y = yo; y >= yn; y--)
		{
			for (int x = xl; x <= xr; x++)
			{
				if (map->isColl(x, y))
				{
					posf.y = map->Top(y) + csiz.y;
					vel.y = 0;
					jumpTimeLeft = 0;
					goto vert_exit;
				}
			}
		}

	}
	else if (direction.y > 0) //Vamos hacia arriba
	{
		//le sumamos a la Y la mitad de su tamaño para obtener la Y superior del sprite
		int yo = map->tilePosY(pos0.y + cen.y),
			yn = map->tilePosY(posf.y + cen.y),
			xl = map->tilePosX(pos0.x - cen.x + 2),
			xr = map->tilePosX(pos0.x + csiz.x - 2);
		for (int y = yo; y <= yn; y++)
		{
			for (int x = xl; x <= xr; x++)
			{
				if (map->isColl(x, y))
				{
					posf.y = map->Bottom(y) - cen.y;
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
		int xo = map->tilePosX(pos0.x - cen.x),
			xn = map->tilePosX(posf.x - cen.x),
			yb = map->tilePosY(pos0.y - csiz.y + 2),
			yt = map->tilePosY(pos0.y + cen.y - 2);
		for (int x = xo; x >= xn; x--)
		{
			for (int y = yb; y <= yt; y++)
			{
				if (map->isColl(x, y))
				{
					posf.x = map->Right(x) + cen.x;
					vel.x = 0;
					onWall = ONWALL_LEFT;
					goto horz_exit;
				}
			}
		}
	}
	else if (direction.x > 0) //Vamos hacia la derecha
	{
		int xo = map->tilePosX(pos0.x + csiz.x),
			xn = map->tilePosX(posf.x + csiz.x),
			yb = map->tilePosY(pos0.y - csiz.y + 2),
			yt = map->tilePosY(pos0.y + cen.y - 2);
		for (int x = xo; x <= xn; x++)
		{
			for (int y = yb; y <= yt; y++)
			{
				if (map->isColl(x, y))
				{
					posf.x = map->Left(x) - csiz.x;
					vel.x = 0;
					onWall = ONWALL_RIGHT;
					goto horz_exit;
				}
			}
		}
	}
	onWall = ONWALL_NO;

horz_exit:
	pos = posf; //asignamos la posicion final a pos

	//SpriteAnim::Update(GameTime);

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
