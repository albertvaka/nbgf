#include "jumpman.h"
//Based on the work of: Jordi Santiago

#include "input.h"
#include "bullet.h"
#include "assets.h"

// accel
const float run_acc = 1400;
const float run_acc_onair = 400;
const float gravity_acc = 600;

// friction X
const float fri_acc_floor = 1000;
const float fri_acc_floor_crouched = 450;
const float fri_acc_air = 145;

// friction Y
const float fri_acc_wall_up = 1200;
const float fri_acc_wall_down = 450;

// jump
const float vel_jump = -150;
const float vel_walljump = 90;
const float jump_time = 0.35f;

const vec vel_max(220, 200);

// bfg
const float bulletVel = 400.f;
const float bfgCooldown = 0.6f;
const float bfgPushBack = 150.f;

// damage
const vec vel_hit(180.f, -150.f);
const float invencibleTimeAfterHit = 0.5f;

extern sf::Clock mainClock;

JumpMan::JumpMan()
{
	polvito.AddSprite(Assets::hospitalTexture, sf::IntRect(69, 50, 2, 2));

	animation.Ensure(MARIO_IDLE);
	sf::Rect rect = animation.CurrentFrame();
	siz = vec(rect.width, rect.height);
	cen = siz / 2;
	InitPolvito();
}

void JumpMan::Draw(sf::RenderTarget& window) {

	polvito.Draw(window);
	
	sf::Sprite& spr = Assets::marioSprite;
	sf::Shader* shader = nullptr;
	if (isHit() > 0.f) {
		shader = &Assets::tintShader;
		shader->setUniform("flashColor", sf::Glsl::Vec4(1, 0, 0, 0.7));
	}

	spr.setTextureRect(animation.CurrentFrame());
	spr.setOrigin(cen.x, siz.y);
	spr.setPosition(pos.x, pos.y);
	if (lookingLeft) {
		spr.setScale(-1.f, 1.f);
	}
	else {
		spr.setScale(1.f, 1.f);
	}
	window.draw(spr,shader);

	//BFG
	if (bfgCooldownTimer > (bfgCooldown-bfgCooldown/4.f)) {
		bool blink = ((mainClock.getElapsedTime().asMilliseconds() / 10) % 2);
		spr.setTextureRect(sf::IntRect(blink ? 32 * 3: 32 * 2, 3 * 16, 2 * 16, 16));
	}
	else {
		bool blink = ((mainClock.getElapsedTime().asMilliseconds() / 160) % 10) > 8;
		spr.setTextureRect(sf::IntRect(blink ? 32 : 0, 3 * 16, 2 * 16, 16));
	}
	spr.setOrigin(10, 8);
	float scale =  (0.333f + (Mates::MaxOf(bfgCooldown / 1.5f, bfgCooldownTimer) / bfgCooldown));
	spr.setPosition(bfgPos);
	if (bfgAngle < 270 || bfgAngle  > 450) {
		spr.setScale(scale, -scale);
	} else {
		spr.setScale(scale, scale);
	}
	spr.setRotation(bfgAngle);
	window.draw(spr,shader);

	//Restore everything
	spr.setScale(1.f, 1.f);
	spr.setOrigin(0.f, 0.f);
	spr.setRotation(0.f);
}

void JumpMan::Update(float dt)
{
	TileMap* map = TileMap::instance();

	float marginGrounded = 1.f; //in pixels
	grounded = map->isSolidInWorldCoordinates(pos.x - cen.x + 1.f, pos.y + marginGrounded) || map->isSolidInWorldCoordinates(pos.x + cen.x - 1.f, pos.y + marginGrounded);

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
			bool ceiling = map->isSolidInWorldCoordinates(pos.x - cen.x + 1.f, pos.y - siz.y - 1.f) || map->isSolidInWorldCoordinates(pos.x + cen.x - 1.f, pos.y - siz.y - 1.f);
			if (!ceiling) {
				DoPolvitoJump();
				grounded = false;
			}
		}
	}

	vec acc = vec(0, 0);
	if (Keyboard::IsKeyPressed(GameKeys::LEFT)) {
		lookingLeft = true;
		if (grounded) {
			if (!crouched) acc.x -= run_acc;
		}
		else {
			acc.x -= run_acc_onair;
		}
	}
	if (Keyboard::IsKeyPressed(GameKeys::RIGHT)) {
		lookingLeft = false;
		if (grounded) {
			if (!crouched) acc.x += run_acc;
		}
		else {
			acc.x += run_acc_onair;
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
	vec posf = pos + vel * dt; //posicion final
	//std::cout << abs(pos.x - posf.x) / dt << std::endl;

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
	vec direction = posf - pos;
	const int N = 1;


	if (direction.x < 0) //Vamos hacia la izquierda
	{
		int xo = map->toTiles(pos.x - cen.x);
		int xn = map->toTiles(posf.x - cen.x);
		int yTop = map->toTiles(pos.y - siz.y + N);
		int yBottom = map->toTiles(pos.y - N);
		for (int x = xo; x >= xn; x--)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				if (map->isSolid(x, y))
				{
					posf.x = map->Right(x) + cen.x;
					vel.x = -10.f; //stay against wall
					if (!isHit()) {
						onWall = ONWALL_LEFT;
					}
					lookingLeft = true;
					goto horz_exit;
				}
			}
		}
		//No collision left
	}
	else if (direction.x > 0) //Vamos hacia la derecha
	{
		int xo = map->toTiles(pos.x + csiz.x);
		int xn = map->toTiles(posf.x + csiz.x);
		int yTop = map->toTiles(pos.y - siz.y + N);
		int yBottom = map->toTiles(pos.y - N);
		for (int x = xo; x <= xn; x++)
		{
			for (int y = yTop; y <= yBottom; y++)
			{
				if (map->isSolid(x, y))
				{
					posf.x = map->Left(x) - csiz.x;
					vel.x = 10.f; //stay against wall
					if (!isHit()) {
						onWall = ONWALL_RIGHT;
					}
					lookingLeft = false;
					goto horz_exit;
				}
			}
		}
		//No collision right
	}
	onWall = ONWALL_NO;

horz_exit:
	pos.x = posf.x;

	if (direction.y < 0) //Vamos hacia arriba
	{
		int yo = map->toTiles(pos.y - siz.y); // usamos la y superior del sprite
		int yn = map->toTiles(posf.y - siz.y);
		int xl = map->toTiles(pos.x - cen.x + N);
		int xr = map->toTiles(pos.x + csiz.x - N);
		for (int y = yo; y >= yn; y--)
		{
			for (int x = xl; x <= xr; x++)
			{
				if (map->isSolid(x, y))
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
		int yo = map->toTiles(pos.y); // usamos la y inferior del sprite
		int yn = map->toTiles(posf.y);
		int xl = map->toTiles(pos.x - cen.x + N);
		int xr = map->toTiles(pos.x + csiz.x - N);
		for (int y = yo; y <= yn; y++)
		{
			for (int x = xl; x <= xr; x++)
			{
				if (map->isSolid(x, y))
				{
					posf.y = map->Bottom(y);
					if (vel.y > 50) DoPolvitoLand();
					vel.y = 0;
					onWall = ONWALL_NO;
					grounded = true;
					goto vert_exit;
				}
			}
		}
		//No collision down
	}
vert_exit:
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
		DoPolvitoRun(dt, (acc.x < 0), isTurning);
	}
	polvito.UpdateParticles(dt);

	if (crouched) {
		bfgPos = vec(pos.x, pos.y - 10);
	}
	else {
		bfgPos = vec(pos.x, pos.y - 16);
	}
	bfgAngle = bfgPos.Angle(Mouse::GetPositionInWorld());
	bfgAngle = (int(bfgAngle + 360 + (45.f / 2)) / 45) * 45.f;
	if (bfgCooldownTimer > 0.f) {
		bfgCooldownTimer -= dt;
		if (bfgCooldownTimer < 0.f) {
			bfgCooldownTimer = 0.f;
		}
	} 
	else if (Mouse::IsPressed() && !Keyboard::IsKeyPressed(DEBUG_EDIT_MODE)) {
		float angleInRads = Mates::DegsToRads(bfgAngle);
		bfgCooldownTimer = bfgCooldown;
		vec tipOfTheGun = bfgPos + vec(17, 0).RotatedAroundOrigin(angleInRads);
		new Bullet(tipOfTheGun, vec(bulletVel, 0).RotatedAroundOrigin(angleInRads), 1.5f);
		vel -= vec(bfgPushBack, 0).RotatedAroundOrigin(angleInRads);
		jumpTimeLeft = 0; // Overrides jump impulse 
		if (onWall) {
			vel.x = 0; // Will let wall go if we shoot and we aren't explicitly moving towards the wall
		}
		if (grounded) {
			if (abs(vel.x) < 0.1) {
				DoPolvitoLand();
			} else {
				DoPolvitoRun(dt, vel.x < 0, true);
				DoPolvitoRun(dt, vel.x < 0, true);
			}
		}
	}

	if (invencibleTimer > 0.f) {
		invencibleTimer -= dt;
	}
}

void JumpMan::takeDamage(vec src) {
	invencibleTimer = invencibleTimeAfterHit;
	if (pos.x > src.x) {
		vel.x = vel_hit.x;
	}
	else {
		vel.x = -vel_hit.x;
	}
	if (grounded) {
		vel.y = vel_hit.y;
	}
	jumpTimeLeft = 0;
	onWall = ONWALL_NO;
	crouched = false;
}





// PARTICULITAS

void JumpMan::InitPolvito() {
	polvito.min_interval = 0.01f;
	polvito.max_interval = 0.05f;

	polvito.min_ttl = 0.1f;
	polvito.max_ttl = 0.3f;

	polvito.min_vel.x = 3;
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
	polvito.AddParticles(2);
	polvito.pos = pos + vec(1.5f, -1.5f);
	polvito.FlipX();
	polvito.AddParticles(2);
}

inline void JumpMan::DoPolvitoWallJump() {
	if (vel.x > 0) {
		polvito.pos = pos + vec(-7.f, -16.f);
	}
	else {
		polvito.pos = pos + vec(7.f, -16.f);
	}

	if ((vel.x < 0 && polvito.min_vel.x > 0) || (vel.x > 0 && polvito.min_vel.x < 0)) {
		polvito.FlipX();
	}
	polvito.AddParticles(5);
}

inline void JumpMan::DoPolvitoLand() {
	// Pluf cap als dos costats
	polvito.pos = pos + vec(-8.f, -0.3f);
	if (polvito.min_vel.x > 0) {
		polvito.FlipX();
	}
	polvito.AddParticles(3);
	polvito.pos = pos + vec(8.f, -0.3f);
	polvito.FlipX();
	polvito.AddParticles(3);
}

inline void JumpMan::DoPolvitoRun(float dt, bool toTheLeft, bool doTheExtraPolvitoLikeYouKnowItsDone) {
	if (toTheLeft) {
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
	if (doTheExtraPolvitoLikeYouKnowItsDone) {
		polvito.AddParticles(2);
	}
	polvito.Spawn(dt);
}
