#include "scene_jumpman.h"
#include "input.h"
#include "imgui.h"
#include "bullet.h"
#include "assets.h"
#include "simplexnoise.h"
#include "debug.h"

const float batClusterSize = 22.f;
const float chanceAngryBat = 0.2f;
const float camSpeed = 2000;

extern sf::Clock mainClock;

#ifdef _DEBUG
static int currentPlacingTile = 1;
#endif

const bool random_mode = false;

JumpScene::JumpScene()
	: map(TiledMap::map_size.x, TiledMap::map_size.y)
{
	bulletPartSys.AddSprite(Assets::marioTexture, sf::IntRect(5, 37, 6, 6));

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
}

void JumpScene::EnterScene() 
{
	player.Reset();
	player.pos = TiledEntities::spawn;

	Camera::SetZoom(GameData::GAME_ZOOM);
	Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));
	//transition.setTime(2.0f);
	//transition.setPos(0.5f* GameData::JUMPMAN_ZOOM);
	//transition.goPos(GameData::JUMPMAN_ZOOM);
	
	if (random_mode) {
		RandomMap();
		return;
	}

	map.LoadFromTiled();

	for (const sf::Vector2f& v : TiledEntities::bat) {
		new Bat(v,false);
	}

	for (const sf::Vector2f& v : TiledEntities::angrybat) {
		new Bat(v, true);
	}

	for (const sf::Vector2f& v : TiledEntities::batawake) {
		(new Bat(v, false))->awakened=true;
	}

	for (const sf::Rect<float>& a : TiledAreas::lava) {
		new Lava(a.top, a.left, a.left+a.width);
	}

	screenManager.UpdateCurrentScreen(&player);
	Camera::SetCameraCenter(player.pos);
}

void JumpScene::ExitScene()
{
	bulletPartSys.Clear();
	EntS<Bullet>::deleteAll();
	EntS<Bat>::deleteAll();
	EntS<Lava>::deleteAll();
	destroyedTiles.Clear();
}

void JumpScene::Update(float dt)
{

	for (const Lava* l : EntS<Lava>::getAll()) {
		if (l->IsInside(player.pos - vec(0,3.f))) {
			player.dead = true;
			player.invencibleTimer = 1;
			player.pos.y += 6 * dt; //sink slowly in the lava
			player.bfgPos.y = -1000;
			player.onWall = JumpMan::ONWALL_NO;
		}
		if (l->IsInside(player.bounds().Center())) {
			ExitScene();
			EnterScene();
		}
	}

	if (Keyboard::IsKeyJustPressed(GameKeys::RESTART)) {
		ExitScene();
		EnterScene();
	}

	//transition.update(dt);
	player.Update(dt);

	Camera::ChangeZoomWithPlusAndMinus(10.f, dt);
	//if (!transition.reached()) {
	//	Camera::SetZoom(transition.getPos());
	//}

	screenManager.UpdateCurrentScreen(&player);

	// TODO: keep the camera so you see a bit more in the direction you are going (like in https://youtu.be/AqturoCh5lM?t=3801)
	vec camPos = (player.pos* 17 + Mouse::GetPositionInWorld()*2) / 19.f;
	screenManager.ClampCameraToScreen(camPos);

	vec oldPos = Camera::GetCameraCenter();
	vec displacement = camPos - oldPos;
	displacement.Truncate(camSpeed * dt);
	Camera::SetCameraCenter(oldPos + displacement);

	// TODO: Better selfregister that does all the push_backs/erases at once at the end of the frame
	for (Bullet* e  : EntS<Bullet>::getAll()) {
		e->Update(dt);
		if (e->explode) continue;

		for (Lava* l : EntS<Lava>::getAll()) {
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
		sf::Vector2i t = map.toTiles(e->pos+ toTheOutside);
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
	EntS<Bullet>::deleteNotAlive();

	for (Bat* e : EntS<Bat>::getAll()) {
		e->Update(dt);
		for (Bullet* b : EntS<Bullet>::getAll()) {
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
	EntS<Bat>::deleteNotAlive();

#ifdef _DEBUG
	if (Debug::Draw) {
		if (Mouse::GetScrollWheelMovement() < 0.f) {
			currentPlacingTile -= 1;
			if (currentPlacingTile < 1) currentPlacingTile = magic_enum::enum_count<Tile::Value>() - 1;
		} else if (Mouse::GetScrollWheelMovement() > 0.f) {
			currentPlacingTile += 1;
			if (currentPlacingTile >= magic_enum::enum_count<Tile::Value>()) currentPlacingTile = 1;
		}
		bool left = Mouse::IsPressed(sf::Mouse::Button::Left);
		bool right = Mouse::IsPressed(sf::Mouse::Button::Right);
		if (left || right) {
			Tile what_to_set = left ? Tile::Value(currentPlacingTile) : Tile::NONE;
			vec pos = Mouse::GetPositionInWorld();
			sf::Vector2i tile = map.toTiles(pos);
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
	for (Lava* l : EntS<Lava>::getAll()) {
		l->Update(dt);
	}
}

void JumpScene::Draw(sf::RenderTarget& window)
{
	window.clear(sf::Color(31, 36, 50));

	if (Debug::Draw) {
		Simplex::DebugDraw(window, Tile::size, [this](int x, int y) {
			return Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize);
		});
	}

	map.Draw(window);
	destroyedTiles.Draw(window);

	for (const Bat* e : EntS<Bat>::getAll()) {
		e->Draw(window);
		if (Debug::Draw && Camera::GetCameraBounds().IsInside(e->pos)) {
			e->drawBounds(window);
			e->DrawSenseArea(window);
		}
	}

	bulletPartSys.Draw(window);
	//bulletPartSys.DrawImGUI("BulletTrail");

	for (const Bullet* e : EntS<Bullet>::getAll()) {
		e->Draw(window);
		if (Debug::Draw) {
			e->drawBounds(window);
		}
	}

	player.Draw(window);


	for (const Lava* l : EntS<Lava>::getAll()) {
		l->Draw(window);
	}

	if (Debug::Draw) {
		player.bounds().Draw(window);
		//player.pos.Debuggerino(sf::Color::White);
		//player.bounds().Center().Debuggerino(sf::Color::Magenta);
	}



	ImGui::Begin("jumpman scene");
	//ImGui::SliderFloat("y", &player.pos.y, 0.f, 25 * 16.f);
	vec m = Mouse::GetPositionInWorld();
	sf::Vector2i t = map.toTiles(m);
	ImGui::Text("Mouse: %f,%f", m.x, m.y);
	ImGui::Text("Mouse on tile: %d,%d", t.x, t.y);
	ImGui::SliderFloat("lava", &(EntS<Lava>::getAll()[0]->targetY), (TiledMap::map_size.y - 1) * 16, (TiledMap::map_size.y - 1) * 16 - 1000);
	ImGui::End();

#ifdef _DEBUG
	if (Debug::Draw) {
		Assets::marioSprite.setTextureRect(Tile::tileToTextureRect[currentPlacingTile]);
		vec pos = Camera::GetCameraBounds().TopLeft() + vec(0, 16);
		Bounds(pos, vec(Tile::size, Tile::size)).Draw(window, sf::Color::Black, sf::Color::Magenta);
		Assets::marioSprite.setPosition(pos);
		window.draw(Assets::marioSprite);
	}
#endif

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
					new Bat(TileMap::fromTiles(x,y+2), angry);
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

	sf::Vector2i pos = map.toTiles(player.pos);
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
	map.setTile(pos.x + 1, pos.y, Tile::RIGHT_SLOPE_1);
	map.setTile(pos.x + 2, pos.y, Tile::SOLID_1);
	map.setTile(pos.x + 3, pos.y - 1, Tile::SOLID_1);
	map.setTile(pos.x + 3, pos.y, Tile::SOLID_1);
	map.setTile(pos.x + 2, pos.y - 1, Tile::RIGHT_SLOPE_1);
	map.setTile(pos.x + 3, pos.y - 2, Tile::RIGHT_SLOPE_1);
	map.setTile(pos.x + 4, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x + 5, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 4, pos.y, Tile::LEFT_SLOPE_1);
	map.setTile(pos.x - 5, pos.y, Tile::SOLID_1);
	map.setTile(pos.x - 5, pos.y - 1, Tile::LEFT_SLOPE_1);
	map.setTile(pos.x - 6, pos.y - 1, Tile::SOLID_1);
	map.setTile(pos.x - 6, pos.y, Tile::SOLID_1);
	map.setTile(pos.x - 6, pos.y - 2, Tile::LEFT_SLOPE_1);
	map.setTile(pos.x - 7, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 8, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 9, pos.y - 2, Tile::SOLID_1);
	map.setTile(pos.x - 10, pos.y - 2, Tile::SOLID_1);
}