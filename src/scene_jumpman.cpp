#include "scene_jumpman.h"
#include "input.h"
#include "imgui.h"
#include "bullet.h"
#include "assets.h"
#include "simplexnoise.h"

const float batClusterSize = 22.f;
const float sceneZoom = 2.5f;
const float chanceAngryBat = 0.2f;

JumpScene::JumpScene()
	: map(sf::Vector2i(1000, 23), 16)
	, player(&map)
{

	player.polvito.AddSprite(Assets::hospitalTexture, sf::IntRect(69, 50, 2, 2));
	bulletPartSys.AddSprite(Assets::marioTexture, sf::IntRect(0, 2 * 16, 16, 16)).setColor(sf::Color(255, 255, 255, 200));

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
}

void JumpScene::EnterScene() 
{
	Camera::SetZoom(sceneZoom);
	Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));

	//transition.setTime(2.0f);
	//transition.setPos(0.5f* GameData::JUMPMAN_ZOOM);
	//transition.goPos(GameData::JUMPMAN_ZOOM);

	randomSeed = Random::roll(0,10000);

	player.pos = vec(160, 160);
	player.Reset();

	map.Randomize(randomSeed);

	for (int x = 20; x < map.sizes.x; x+=2) { // don't spawn at the leftmost part of the map where the player starts, don't spawn two bats together
		for (int y = 1; y < map.sizes.y-5; y++) { //don't spawn at the bottom rows
			if (map.isColl(x, y)) {
				float noise = Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize); // returns a number between -1 and 1
				if (noise > 0.f) { 
					bool angry = (Random::rollf() < chanceAngryBat);
					new Bat(vec((x+0.5f) * map.unitsPerTile, (y+1.5f) * map.unitsPerTile), &player, &map, angry);
					map.set(x - 1, y + 1, false);
					map.set(x, y + 1, false);
					map.set(x + 1, y + 1, false);
					map.set(x - 1, y + 2, false);
					map.set(x, y + 2, false);
					map.set(x + 1, y + 2, false);
				}
			}
		}
	}

	std::cout << "seed=" << randomSeed << ", bats=" << Bat::getAll().size() << std::endl;

	sf::Vector2i pos = map.toTiles(player.pos);
	map.set(pos.x - 1, pos.y + 1, true);
	map.set(pos.x,     pos.y + 1, true);
	map.set(pos.x - 1, pos.y,     false);
	map.set(pos.x,     pos.y,     false);
	map.set(pos.x - 1, pos.y - 1, false);
	map.set(pos.x,     pos.y - 1, false);
	map.set(pos.x - 1, pos.y - 2, false);
	map.set(pos.x,     pos.y - 2, false);

}

void JumpScene::ExitScene()
{
	bulletPartSys.Clear();
	EntS<Bullet>::deleteAll();
	EntS<Bat>::deleteAll();
}

void JumpScene::Update(int dtMilis) {

	if (Keyboard::IsKeyJustPressed(GameKeys::RESTART) || (map.toTiles(player.pos + vec(0.01f, 0)).y >= map.sizes.y)) {
		ExitScene();
		EnterScene();
	}

	float dt = dtMilis / 1000.f;
	//transition.update(dt);
	player.Update(dt);

	Camera::ChangeZoomWithPlusAndMinus(10.f, dt);
	//if (!transition.reached()) {
	//	Camera::SetZoom(transition.getPos());
	//}
	vec camPos = (player.pos* 17 + Mouse::GetPositionInWorld()*2) / 19.f;
	//float minY = (Camera::GetCameraBounds().height / 2.f) - (1 * 16);
	//float maxY = ((25 + 1) * 16) - (Camera::GetCameraBounds().height / 2.f);
	//if (maxY < minY) {
	//	minY = maxY - 1;
	//}
	//Mates::Clamp(camPos.y, minY, maxY);
	camPos.y = (Camera::GetCameraBounds().height / 2.f) - map.unitsPerTile; //fixed Y axis
	float minX = (Camera::GetCameraBounds().width / 2.f) - (1 * 16);
	float maxX = ((1000 + 1) * 16) - (Camera::GetCameraBounds().width / 2.f);
	if (maxX < minX) {
		minX = maxX - 1;
	}
	Mates::Clamp(camPos.x, minX, maxX);
	
	// TODO: keep the camera so you see a bit more in the direction you are going (like in https://youtu.be/AqturoCh5lM?t=3801)
	Camera::SetCameraCenter(camPos);

	// TODO: Better selfregister that does all the push_backs/erases at once at the end of the frame
	for (Bullet* e  : EntS<Bullet>::getAll()) {
		e->Update(dt);
		if (!e->explode) {
			bulletPartSys.pos = e->pos + vec::Rand(-4, -4, 4, 4);
			bulletPartSys.Spawn(dt);
		}
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

	if (Keyboard::IsKeyPressed(DEBUG_EDIT_MODE) && (Mouse::IsPressed(sf::Mouse::Button::Left) || Mouse::IsPressed(sf::Mouse::Button::Right))) {
		bool what_to_set = Mouse::IsPressed(sf::Mouse::Button::Left);
		vec pos = Mouse::GetPositionInWorld();
		sf::Vector2i tile = map.toTiles(pos);
		map.set(tile.x, tile.y, what_to_set);
	}

	bulletPartSys.UpdateParticles(dt);
}

void JumpScene::Draw(sf::RenderTarget& window, bool debugDraw)
{
	window.clear(sf::Color(255 * 0.200f, 255 * 0.100f, 255 * 0.100f));

	if (debugDraw) {
		Simplex::DebugDraw(window, map.unitsPerTile, [this](int x, int y) {
			return Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize);
		});
	}

	map.Draw(window);

	for (Bat* e : EntS<Bat>::getAll()) {
		e->Draw(window);
		if (debugDraw && Camera::GetCameraBounds().IsInside(e->pos)) {
			e->drawBounds(window);
			e->DrawSenseArea(window);
		}
	}

	bulletPartSys.Draw(window);
	//bulletPartSys.DrawImGUI("BulletTrail");

	for (Bullet* e : EntS<Bullet>::getAll()) {
		e->Draw(window);
		if (debugDraw) {
			e->drawBounds(window);
		}
	}

	player.Draw(window);

	if (debugDraw) {
		player.bounds().Draw(window);
		Bounds(player.pos, vec(1, 1)).Draw(window, sf::Color::White);
		Bounds(player.center(), vec(1, 1)).Draw(window, sf::Color::White);
	}

	//ImGui::Begin(GameData::GAME_TITLE.c_str());
	//ImGui::SliderFloat("y", &player.pos.y, 0.f, 25 * 16.f);
	//ImGui::End();

	//player.polvito.DrawImGUI("Polvito");
}

