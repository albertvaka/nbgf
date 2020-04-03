#include "scene_jumpman.h"
#include "input.h"
#include "imgui.h"
#include "bullet.h"
#include "assets.h"
#include "simplexnoise.h"

const float batClusterSize = 22.f;
const float sceneZoom = 3.f;
const float chanceAngryBat = 0.2f;

extern sf::Clock mainClock;

JumpScene::JumpScene()
	: map(sf::Vector2i(1000, 19), 16)
	, player(&map)
{
	Window::SetWindowSize(sf::Vector2u(21*16 * sceneZoom * 16.f / 9, 21*16* sceneZoom));

	player.polvito.AddSprite(Assets::hospitalTexture, sf::IntRect(69, 50, 2, 2));
	bulletPartSys.AddSprite(Assets::marioTexture, sf::IntRect(5, 37, 6, 6));
	lavaPartSys.AddSprite(Assets::marioTexture, sf::IntRect(5+16, 37, 6, 6));
	lavaPartSys.AddSprite(Assets::marioTexture, sf::IntRect(38, 37, 5, 5));

	float yBottom = map.boundsInWorld().Bottom();
	lavaPartSys.pos.y = yBottom - 2;
	lavaPartSys.min_interval = 4.f;
	lavaPartSys.max_interval = 6.5f;
	lavaPartSys.min_vel = vec(-15, -55);
	lavaPartSys.max_vel = vec(15, -40);
	lavaPartSys.min_ttl = 2.f;
	lavaPartSys.max_ttl = 2.f;
	lavaPartSys.min_scale = 1.f;
	lavaPartSys.max_scale = 1.5f;
	lavaPartSys.alpha_vel = -0.5f;
	lavaPartSys.scale_vel = -0.4f;
	lavaPartSys.acc = vec(0,60.f);

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
	Camera::SetZoom(sceneZoom);
	Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));

	//transition.setTime(2.0f);
	//transition.setPos(0.5f* GameData::JUMPMAN_ZOOM);
	//transition.goPos(GameData::JUMPMAN_ZOOM);

	randomSeed = Random::roll(0,10000);

	player.pos = vec(160, 160);
	player.Reset();

	map.Randomize(randomSeed);

	Bat::jumpman = &player;
	Bat::tilemap = &map;
	for (int x = 20; x < map.sizes.x; x+=2) { // don't spawn at the leftmost part of the map where the player starts, don't spawn two bats together
		for (int y = -1; y < map.sizes.y-5; y++) { //don't spawn at the bottom rows
			if (map.isColl(x, y)) {
				float noise = Simplex::raw_noise_2d(randomSeed + x / batClusterSize, y / batClusterSize); // returns a number between -1 and 1
				if (y == -1) noise -= 0.66f;
				if (noise > 0.f) { 
					bool angry = (Random::rollf() < chanceAngryBat);
					new Bat(vec((x+0.5f) * map.unitsPerTile, (y+1.5f) * map.unitsPerTile), angry);
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

	if (Keyboard::IsKeyJustPressed(GameKeys::RESTART) || (map.toTiles(player.pos + vec(0.1f, 0)).y >= map.sizes.y)) {
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

	const float chunkSize = 15.f;
	Bounds screen = Camera::GetCameraBounds();
	float left = (Mates::fastfloor(screen.Left() / chunkSize) - 1) * chunkSize;
	float right = (Mates::fastfloor(screen.Right() / chunkSize) + 1) * chunkSize;
	for (float x = left; x < right; x += chunkSize) {
		lavaPartSys.pos.x = x;
		lavaPartSys.Spawn(dt);
	}
	lavaPartSys.UpdateParticles(dt);
}

void JumpScene::Draw(sf::RenderTarget& window, bool debugDraw)
{
	window.clear(sf::Color(50, 25, 25));

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

	lavaPartSys.Draw(window);
	if (!debugDraw) {
		DrawLava(window);
	}

	if (debugDraw) {
		player.bounds().Draw(window);
		Bounds(player.pos, vec(1, 1)).Draw(window, sf::Color::White);
		Bounds(player.center(), vec(1, 1)).Draw(window, sf::Color::White);
	}

	lavaPartSys.DrawImGUI("Lava");


	//ImGui::Begin(GameData::GAME_TITLE.c_str());
	//ImGui::SliderFloat("y", &player.pos.y, 0.f, 25 * 16.f);
	//ImGui::End();

	//player.polvito.DrawImGUI("Polvito");
}

void JumpScene::DrawLava(sf::RenderTarget& window)
{
	static float waveAmplitude = 1.f;
	static float chunkSize = 5.4f;
	static float waveHeight = 2.8f;
	static float height = 22.f;
	static float speed = 3.0f;
	static bool blockyWaves = true;

	ImGui::Begin("Waves");
	ImGui::SliderFloat("speed", &speed, 0.f, 10.f);
	ImGui::SliderFloat("height", &height, 0.f, 40.f);
	ImGui::SliderFloat("waveHeight", &waveHeight, 0.f, 10.f);
	ImGui::SliderFloat("chunkSize", &chunkSize, 0.1f, 10.f);
	ImGui::SliderFloat("waveAmplitude", &waveAmplitude, 0.f, 5.f);
	ImGui::Checkbox("blockyWaves", &blockyWaves);
	ImGui::End();

	float time = mainClock.getElapsedTime().asSeconds() * speed;
	Bounds screen = Camera::GetCameraBounds();
	float yBottom = map.boundsInWorld().Bottom() + 16;

	sf::ConvexShape poly;
	poly.setPointCount(4);

	float left = (Mates::fastfloor(screen.Left() / chunkSize) - 1) * chunkSize;
	float right = (Mates::fastfloor(screen.Right() / chunkSize) + 1) * chunkSize;
	for (float x0 = left; x0 < right; x0 += chunkSize)
	{
		float xf = x0 + chunkSize;
		float xf4sin = blockyWaves ? x0 : xf;
		//float iF = blockyWaves ? i0 : i0 + 1.f;
		poly.setPosition(vec(0, yBottom));
		poly.setPoint(0, vec(x0, 0));
		poly.setPoint(1, vec(x0, - height - waveHeight * sin(x0 * waveAmplitude + time)));
		poly.setPoint(2, vec(xf, - height - waveHeight * sin(xf4sin * waveAmplitude + time)));
		poly.setPoint(3, vec(xf, 0));

		poly.setPosition(vec(0, yBottom));
		poly.setFillColor(sf::Color(220, 10, 10)); // Top layer
		window.draw(poly);

		poly.setPosition(vec(0, yBottom + 5.f));
		poly.setFillColor(sf::Color(120, 0, 0)); // Line
		window.draw(poly);

		poly.setPosition(vec(0, yBottom + 6.f));
		poly.setFillColor(sf::Color(250, 140, 50)); // Bottom layer
		window.draw(poly);
	}

}
