#include "scene_jumpman.h"
#include "input.h"
#include "imgui.h"
#include "bullet.h"

JumpScene::JumpScene()
	: map(sf::Vector2i(1000, 25), 16)
	, player(&map)
{

}

void JumpScene::EnterScene() 
{
	Camera::SetZoom(GameData::GAME_ZOOM);
	Camera::SetCameraCenter(vec(GameData::WINDOW_WIDTH / (2*GameData::GAME_ZOOM), GameData::WINDOW_HEIGHT/(2*GameData::GAME_ZOOM)));

	texture.loadFromFile("data/spritesheet.png");
	sprite.setTexture(texture);
	
	marioTexture.loadFromFile("data/mario.png");
	marioSprite.setTexture(marioTexture);

	transition.setTime(2.0f);
	transition.setPos(0.6f* GameData::GAME_ZOOM);
	transition.goPos(GameData::GAME_ZOOM);

	player.pos = vec(160, 160);
	player.polvito.AddSprite(texture, sf::IntRect(69, 50, 2, 2));
	player.Reset();

	bulletPartSys.AddSprite(marioTexture, sf::IntRect(0, 2 * 16, 16, 16)).setColor(sf::Color(255,255,255,200));
	
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

	new Bat(vec(60, 10));
	new Bat(vec(100, 50));
	new Bat(vec(1000, 50));

	map.Randomize(time(NULL));

	sf::Vector2i pos = map.tilePos(player.pos);
	map.set(pos.x - 1, pos.y + 1, true);
	map.set(pos.x    , pos.y + 1, true);
	map.set(pos.x - 1, pos.y    , false);
	map.set(pos.x    , pos.y    , false);
	map.set(pos.x - 1, pos.y - 1, false);
	map.set(pos.x    , pos.y - 1, false);

}

void JumpScene::ExitScene()
{
	bulletPartSys.Clear();
	EntS<Bullet>::deleteAll();
	EntS<Bat>::deleteAll();
}

void JumpScene::Update(int dtMilis) {

	if (Keyboard::IsKeyJustPressed(GameKeys::RESTART) || (map.tilePos(player.pos + vec(0.01f, 0)).y >= map.sizes.y)) {
		ExitScene();
		EnterScene();
	}

	float dt = dtMilis / 1000.f;
	transition.update(dt);
	player.Update(dt);

	Camera::ChangeZoomWithPlusAndMinus(10.f, dt);
	if (!transition.reached()) {
		Camera::SetZoom(transition.getPos());
	}
	//Camera::MoveCameraWithArrows(50.f, dt);
	vec camPos = (player.pos* 17 + Mouse::GetPositionInWorld()*2) / 19.f;
	float minY = (Camera::GetCameraBounds().height / 2.f) - (1 * 16);
	float maxY = ((25 + 1) * 16) - (Camera::GetCameraBounds().height / 2.f);
	if (maxY < minY) {
		minY = maxY - 1;
	}
	Mates::Clamp(camPos.y, minY, maxY);
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
		e->Update(&player, dt);
		for (Bullet* b : EntS<Bullet>::getAll()) {
			if (b->explode) continue;
			if (Collide(e, b)) {
				b->pos = e->pos;
				b->explode = true;
				e->alive = false;
			}
		}
	}
	EntS<Bat>::deleteNotAlive();

	if (Keyboard::IsKeyPressed(DEBUG_EDIT_MODE) && (Mouse::IsPressed(sf::Mouse::Button::Left) || Mouse::IsPressed(sf::Mouse::Button::Right))) {
		bool what_to_set = Mouse::IsPressed(sf::Mouse::Button::Left);
		vec pos = Mouse::GetPositionInWorld();
		sf::Vector2i tile = map.tilePos(pos);
		map.set(tile.x, tile.y, what_to_set);
	}

	bulletPartSys.UpdateParticles(dt);
}

void JumpScene::Draw(sf::RenderTarget& window) 
{

	static bool debugDraw = false;
	if (Keyboard::IsKeyJustPressed(DEBUG_BOUNDS)) {
		debugDraw = !debugDraw;
	}
	window.clear(sf::Color(255*0.200f, 255 * 0.100f, 255 * 0.100f));

	map.Draw(sprite, window);

	for (Bat* e : EntS<Bat>::getAll()) {
		e->Draw(marioSprite, window);
		if (debugDraw) {
			e->drawBounds(window);
			e->DrawSenseArea(window);
		}
	}

	bulletPartSys.Draw(window);
	//bulletPartSys.DrawImGUI("BulletTrail");

	for (Bullet* e : EntS<Bullet>::getAll()) {
		e->Draw(sprite, window);
		if (debugDraw) {
			e->drawBounds(window);
		}
	}

	player.Draw(marioSprite, window);

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

