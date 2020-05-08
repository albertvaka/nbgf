#include "scene_jumpman.h"
#include "input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "bullet.h"
#include "enemy_door.h"
#include "assets.h"
#include "simplexnoise.h"
#include "parallax.h"
#include "bat.h"
#include "lava.h"
#include "savestation.h"
#include "debug.h"

extern float mainClock;

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;
const float camSpeed = 2000;

#ifdef _DEBUG
static int currentPlacingTile = 1;
#endif

const bool random_mode = false;

JumpScene::JumpScene()
	: map(TiledMap::map_size.x, TiledMap::map_size.y)
	, fogPartSys(Assets::fogTexture)
	, bulletPartSys(Assets::marioTexture)
{
	bulletPartSys.AddSprite({ 5, 37, 6, 6 });

	float vel = 15;
	bulletPartSys.max_vel = vec(vel, vel);
	bulletPartSys.min_vel = vec(-vel, -vel);
	bulletPartSys.min_ttl = 0.5f;
	bulletPartSys.max_ttl = 1.f;
	bulletPartSys.min_interval = 0.03f;
	bulletPartSys.max_interval = 0.06f;
	bulletPartSys.min_scale = 0.5f;
	bulletPartSys.max_scale = 0.9f;
	bulletPartSys.scale_vel = -2.5f;
	bulletPartSys.min_rotation = 0.f;
	bulletPartSys.max_rotation = 360.f;
	bulletPartSys.rotation_vel = 180.f;
	bulletPartSys.alpha = 0.75f;

	fogPartSys.AddSprite({ 0, 0, 140, 61 });
	fogPartSys.AddSprite({140, 0, 140, 61 });
	fogPartSys.AddSprite({140*2, 0, 140, 61 });
	fogPartSys.AddSprite({140 * 3, 0, 140, 61 });
	fogPartSys.alpha = 0.f;
	fogPartSys.alpha_vel = 0.1f;
	fogPartSys.bounce_alpha = 0.5f;
	fogPartSys.min_scale = 2.f;
	fogPartSys.max_scale = 3.f;
	fogPartSys.min_vel.x = 17.f;
	fogPartSys.max_vel.x = 20.f;
	fogPartSys.min_ttl = fogPartSys.max_ttl = 10.f;
	fogPartSys.min_interval = 4.5f;
	fogPartSys.max_interval = 6.f;

	if (!random_mode) {
		for (const Bounds& p : TiledAreas::parallax_forest) {
			new Parallax(p);
		}
	}
}

void JumpScene::EnterScene() 
{
	player.Reset();

	if (random_mode) {
		player.pos = vec(160,160);
		RandomMap();
		return;
	}

	player.pos = TiledEntities::spawn;

	new GunUp(TiledEntities::spawn);

	map.LoadFromTiled();

	for (const vec& v : TiledEntities::bat) {
		new Bat(v,false, false);
	}

	for (const vec& v : TiledEntities::angrybat) {
		new Bat(v, true, false);
	}

	for (const vec& v : TiledEntities::batawake) {
		new Bat(v, false, true);
	}

	for (const vec& v : TiledEntities::save) {
		new SaveStation(v);
	}

	for (const vec& v : TiledEntities::gunup) {
		//GunUp* up = new GunUp(v);
	}
	//gunup_tancaporta = new GunUp(TiledEntities::gunup_tancaporta);

	for (const vec& v : TiledEntities::healthup) {
		new HealthUp(v);
	}

	for (const vec& v : TiledEntities::enemy_door) {
		EnemyDoor* d = new EnemyDoor(v);
		int door_screen = screenManager.FindScreenContaining(d->pos);
		if (door_screen < 0) {
			Debug::out << "Warning: Enemy door outside a screen";
		}
		for (const Bat* b : Bat::getAll()) {
			if (b->screen == door_screen) {
				d->AddEnemy(b);
			}
		}
	}

	for (const Bounds& a : TiledAreas::lava) {
		new Lava(a);
	}

	screenManager.UpdateCurrentScreen(player.pos);
	Camera::SetCenter(player.pos);
}

void JumpScene::ExitScene()
{
	bulletPartSys.Clear();
	Bullet::deleteAll();
	Bat::deleteAll();
	Lava::deleteAll();
	destroyedTiles.Clear();
	EnemyDoor::deleteAll();
}

void JumpScene::Update(float dt)
{

	for (const Lava* l : Lava::getAll()) {
		if (l->IsInside(player.pos - vec(0,7.f))) {
			player.dead = true;
			player.invencibleTimer = 1;
			player.pos.y += 6 * dt; //sink slowly in the lava
			player.bfgPos.y = -1000;
			player.onWall = JumpMan::ONWALL_NO;
		}
		if (l->IsInside(player.pos - vec(0, 14.f))) {
			ExitScene();
			EnterScene();
		}
	}

	if (Input::IsJustPressed(0,GameKeys::RESTART)) {
		ExitScene();
		EnterScene();
	}

	skillTree.Update(dt);
	if (skillTree.open) return;

	player.Update(dt);

	if (!random_mode) {
		screenManager.UpdateCurrentScreen(player.pos);
	}

	// TODO: keep the camera so you see a bit more in the direction you are going (like in https://youtu.be/AqturoCh5lM?t=3801)
	vec camPos = (player.pos* 17 + Mouse::GetPositionInWorld()*2) / 19.f;
	screenManager.ClampCameraToScreen(camPos);

	vec oldPos = Camera::GetCenter();
	vec displacement = camPos - oldPos;
	displacement.Truncate(camSpeed * dt);
	Camera::SetCenter(oldPos + displacement);

	// TODO: Better selfregister that does all the push_backs/erases at once at the end of the frame
	for (Bullet* e  : Bullet::getAll()) {
		e->Update(dt);
		if (e->explode) continue;

		for (Lava* l : Lava::getAll()) {
			if (l->IsInside(e->pos)) {
				AwakeNearbyBats(e->pos);
				l->Plof(e->pos.x);
				e->alive = false;
				continue;
			}
		}

		vec toTheOutside = e->vel.Perp().Normalized()* e->radius * 0.85f;
		//(e->pos + toTheOutside).Debuggerino(sf::Color::White);
		//(e->pos - toTheOutside).Debuggerino(sf::Color::White);
		veci t = map.toTiles(e->pos+ toTheOutside);
		Tile tile = map.getTile(t);
		if (!tile.isFullSolid()) {
			t = map.toTiles(e->pos - toTheOutside);
			tile = map.getTile(t);
		}
		if (tile.isFullSolid()) {
			if (tile.isBreakable()) {
				destroyedTiles.Destroy(t.x, t.y);
			}
			AwakeNearbyBats(e->pos);
			bulletPartSys.pos = e->pos;
			for (int i = 0; i < 5; i++) {
				auto& p = bulletPartSys.AddParticle();
				p.scale = 1.7f;
				p.vel *= 1.5f;
			}
			e->alive = false;
			continue;
		}

		bulletPartSys.pos = e->pos + vec::Rand(-4, -4, 4, 4);
		bulletPartSys.Spawn(dt);
	}
	Bullet::deleteNotAlive();

	for (Bat* e : Bat::getAll()) {
		e->Update(dt);
		for (Bullet* b : Bullet::getAll()) {
			if (b->explode) continue;
			if (Collide(e, b)) {
				b->pos = e->pos;
				b->explode = true;
				e->alive = false;
				AwakeNearbyBats(e->pos);
				break;
			}
		}
		if (!e->alive) continue;
		if (!player.isInvencible()) {
			if (Collide(player.bounds(), e->bounds())) {
				player.takeDamage(e->pos);
			}
		}
	}

	for (EnemyDoor* ed : EnemyDoor::getAll()) {
		ed->Update(dt); // Checks for enemies with alive = false, crashes if they have been deleted already
	}

	Bat::deleteNotAlive(); //Must happen after enemydoor update


#ifdef _DEBUG
	const SDL_Scancode killall = SDL_SCANCODE_F12;
	const SDL_Scancode screen_left = SDL_SCANCODE_F6;
	const SDL_Scancode screen_right = SDL_SCANCODE_F7;
	if (Keyboard::IsKeyJustPressed(screen_left)) {
		player.pos.x -= Window::GAME_WIDTH;
	}
	if (Keyboard::IsKeyJustPressed(screen_right)) {
		player.pos.x += Window::GAME_WIDTH;
	}
	if (Keyboard::IsKeyJustPressed(killall)) {
		for (Bat* e : Bat::getAll()) {
			if (e->screen == screenManager.currentScreen) {
				(new Bullet(e->pos, vec(), 1.5f))->explode = true;
				e->alive = false;
			}
		}
	}
	if (Debug::Draw) {
		if (Mouse::GetScrollWheelMovement() < 0.f) {
			currentPlacingTile -= 1;
			if (currentPlacingTile < 1) currentPlacingTile = magic_enum::enum_count<Tile::Value>() - 1;
		} else if (Mouse::GetScrollWheelMovement() > 0.f) {
			currentPlacingTile += 1;
			if (currentPlacingTile >= int(magic_enum::enum_count<Tile::Value>())) currentPlacingTile = 1;
		}
		if (Input::IsJustPressed(0,LEFT)) currentPlacingTile = Tile::LSLOPE_1;
		if (Input::IsJustPressed(0,RIGHT)) currentPlacingTile = Tile::RSLOPE_1;
		if (Input::IsJustPressed(0,UP)) currentPlacingTile = Tile::ONEWAY_1;
		if (Input::IsJustPressed(0,DOWN)) currentPlacingTile = Tile::SOLID_1;
		bool left = Mouse::IsPressed(Mouse::Button::Left);
		bool right = Mouse::IsPressed(Mouse::Button::Right);
		if (left || right) {
			Tile what_to_set = left ? Tile::Value(currentPlacingTile) : Tile::NONE;
			vec pos = Mouse::GetPositionInWorld();
			veci tile = map.toTiles(pos);
			if (what_to_set == Tile::NONE) {
				map.setTile(tile.x, tile.y, Tile::NONE);
			}
			else if (map.getTile(tile) != what_to_set && map.getTile(tile) != Tile::SOLID_TRANSPARENT) {
				destroyedTiles.toSpawn.emplace_back(tile.x, tile.y, what_to_set, -1.f);
			}
		}
	}
#endif

	bulletPartSys.UpdateParticles(dt);

	destroyedTiles.Update(dt);
	for (Lava* l : Lava::getAll()) {
		l->Update(dt);
	}

	contextActionButton = GameKeys::NONE;
	for (SaveStation* ss : SaveStation::getAll()) {
		ss->Update(dt);
		if (ss->enabled && Collide(ss->bounds(), player.bounds())) {
			contextActionButton = GameKeys::ACTIVATE;
			if (Input::IsJustPressed(0,GameKeys::ACTIVATE)) {
				ss->Activate();
			}
		}
	}

	for (const Bounds& a : TiledAreas::fog) {
		if (!Collide(Camera::GetBounds(),(Bounds(a)*2.f))) {
			continue;
		}
		for (int x = 50; x < a.width - 180; x += 50) {
			for (int y = 0; y < a.height; y += 50) {
				fogPartSys.pos = vec(a.left + x, a.top + y);
				fogPartSys.Spawn(dt);
			}
		}
		fogPartSys.UpdateParticles(dt);
	}

}

void JumpScene::Draw()
{
	Window::Clear(31, 36, 50);

	if (skillTree.open) {
		skillTree.DrawMenu();
		return;
	}

	if (Debug::Draw) {
		Simplex::DebugDraw(Tile::size, [this](int x, int y) {
			return Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize);
		});
	}

	for (const Parallax* p : Parallax::getAll()) {
		p->Draw();
	}

	map.Draw();
	destroyedTiles.Draw();

	for (const SaveStation* ss : SaveStation::getAll()) {
		ss->Draw();
	}

	for (const EnemyDoor* ed : EnemyDoor::getAll()) {
		ed->Draw();
	}

	for (const Bat* e : Bat::getAll()) {
		e->Draw();
		if (Debug::Draw && Camera::GetBounds().Contains(e->pos)) {
			e->drawBounds();
			e->DrawSenseArea();
		}
	}

	bulletPartSys.Draw();
	//bulletPartSys.DrawImGUI("BulletTrail");

	for (const Bullet* e : Bullet::getAll()) {
		e->Draw();
		if (Debug::Draw) {
			e->drawBounds();
		}
	}

	for (const GunUp* g : GunUp::getAll()) {
		g->Draw();
	}
	for (const HealthUp* g : HealthUp::getAll()) {
		g->Draw();
	}

	player.Draw();

	if (contextActionButton != GameKeys::NONE) {
		AnimationType anim = BUTTON_A_PRESS; // TODO: switch depending on the key
		Window::Draw(Assets::hospitalTexture, player.bounds().TopRight() + vec(2, -6))
			.withRect(Animation::AnimFrame(anim, mainClock * 1000));
	}

	for (const Lava* l : Lava::getAll()) {
		l->Draw();
	}

	if (Debug::Draw) {
		player.bounds().Draw();
		//player.pos.Debuggerino(sf::Color::White);
		//player.bounds().Center().Debuggerino(sf::Color::Magenta);
	}

#ifdef _DEBUG
	{
		ImGui::Begin("jumpman scene");
		//ImGui::SliderFloat("y", &player.pos.y, 0.f, 25 * 16.f);
		vec m = Mouse::GetPositionInWorld();
		veci t = map.toTiles(m);
		ImGui::Text("Mouse: %f,%f", m.x, m.y);
		if (ImGui::Button("Add point")) {
			skillTree.gunpoints += 1;
		};
		ImGui::Text("Mouse on tile: %d,%d", t.x, t.y);
		ImGui::SliderFloat("lava", &(Lava::getAll()[0]->targetY), (TiledMap::map_size.y - 1) * 16, (TiledMap::map_size.y - 1) * 16 - 1000);
		ImGui::End();
	}

	if (Debug::Draw) {
		vec pos = Camera::GetBounds().TopLeft() + vec(0, 16);
		Bounds(pos, vec(Tile::size, Tile::size)).Draw(0,0,0);
		Window::Draw(Assets::marioTexture, pos)
			.withRect(Tile::tileToTextureRect[currentPlacingTile]);
	}
#endif
	fogPartSys.Draw();
	fogPartSys.DrawImGUI();
	
	skillTree.DrawOverlay();

	//player.polvito.DrawImGUI("Polvito");
}



void JumpScene::RandomMap() {
	player.pos = vec(160, 160);

	randomSeed = Random::roll(0, 10000);
	map.Randomize(randomSeed);

	Debug::out << "seed=" << randomSeed << ", bats=" << Bat::getAll().size();

	for (int y = -1; y < map.sizes.y - 5; y++) { //don't spawn at the bottom rows
		for (int x = 20; x < map.sizes.x; x += 2) { // don't spawn at the leftmost part of the map where the player starts, don't spawn two bats together
			if (map.isSolid(x, y)) {
				float noise = Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize); // returns a number between -1 and 1
				if (y == -1) noise -= 0.66f;
				if (noise > 0.f) {
					bool angry = (Random::rollf() < chanceAngryBat);
					new Bat(TileMap::fromTiles(x,y+2), angry, false);
					map.setTile(x - 1, y + 1, Tile::NONE);
					map.setTile(x, y + 1, Tile::NONE);
					map.setTile(x + 1, y + 1, Tile::NONE);
					map.setTile(x - 1, y + 2, Tile::NONE);
					map.setTile(x, y + 2, Tile::NONE);
					map.setTile(x + 1, y + 2, Tile::NONE);
				}
			}
		}
	}

	veci pos = map.toTiles(player.pos);
	map.setTile(pos.x - 1, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x - 2, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x - 3, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x - 4, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x - 5, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x - 6, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x - 3, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x + 1, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x + 2, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x + 3, pos.y + 1, Tile::SOLID_1);
	map.setTile(pos.x - 1, pos.y, Tile::NONE);
	map.setTile(pos.x, pos.y, Tile::NONE);
	map.setTile(pos.x - 1, pos.y - 1, Tile::NONE);
	map.setTile(pos.x, pos.y - 1, Tile::NONE);
	map.setTile(pos.x - 1, pos.y - 2, Tile::NONE);
	map.setTile(pos.x, pos.y - 2, Tile::NONE);
	map.setTile(pos.x + 1, pos.y, Tile::RSLOPE_1);
	map.setTile(pos.x + 2, pos.y, Tile::SOLID_1);
	map.setTile(pos.x + 3, pos.y - 1, Tile::SOLID_1);
	map.setTile(pos.x + 3, pos.y, Tile::SOLID_1);
	map.setTile(pos.x + 2, pos.y - 1, Tile::RSLOPE_1);
	map.setTile(pos.x + 3, pos.y - 2, Tile::RSLOPE_1);
	map.setTile(pos.x + 4, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x + 5, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 4, pos.y, Tile::LSLOPE_1);
	map.setTile(pos.x - 5, pos.y, Tile::SOLID_1);
	map.setTile(pos.x - 5, pos.y - 1, Tile::LSLOPE_1);
	map.setTile(pos.x - 6, pos.y - 1, Tile::SOLID_1);
	map.setTile(pos.x - 6, pos.y, Tile::SOLID_1);
	map.setTile(pos.x - 6, pos.y - 2, Tile::LSLOPE_1);
	map.setTile(pos.x - 7, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 8, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 9, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 10, pos.y - 2, Tile::SOLID_1);
}