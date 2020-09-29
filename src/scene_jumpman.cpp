#include "scene_jumpman.h"
#include "input.h"
#ifdef _IMGUI
#include "imgui.h"
#endif
#include "bullet.h"
#include "enemy_door.h"
#include "assets.h"
#include "parallax.h"
#include "bat.h"
#include "missile.h"
#include "bipedal.h"
#include "fxmanager.h"
#include "lava.h"
#include "savestation.h"
#include "debug.h"
#include "collide.h"
#include "rototext.h"
#include "goomba.h"

extern float mainClock;

const float camSpeed = 2000;
const float introDuration = 1.f;

#ifdef _DEBUG
static int currentPlacingTile = 1;
#endif

JumpScene::JumpScene()
	: map(TiledMap::map_size.x, TiledMap::map_size.y, Assets::marioTexture)
	, fogPartSys(Assets::fogTexture)
{
	Bullet::InitParticles();
	Missile::InitParticles();

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

	for (const Bounds& p : TiledAreas::parallax_forest) {
		new Parallax(p);
	}

	for (const auto& screen : TiledMap::screens) {
		screenManager.AddScreen(screen);
	}
}

void JumpScene::EnterScene()
{
	player.Reset(TiledEntities::spawn);

	map.LoadFromTiled();

	for (const vec& v : TiledEntities::bat) {
		new Bat(v, false, false);
	}

	new Bipedal(TiledEntities::boss_bipedal);

	for (const vec& v : TiledEntities::angrybat) {
		new Bat(v, true, false);
	}

	for (const vec& v : TiledEntities::batawake) {
		new Bat(v, false, true);
	}

	for (const vec& v : TiledEntities::goomba) {
		new Goomba(v, false);
	}

	for (const vec& v : TiledEntities::goombacharger) {
		new Goomba(v ,true);
	}


	//for (const vec& v : TiledEntities::save) {
	//	new SaveStation(v);
	//}

	for (const vec& v : TiledEntities::gunup) {
		new GunUp(v);
	}
	gunup_tancaporta = new GunUp(TiledEntities::gunup_tancaporta);
	int screen_gunup_tancaporta = screenManager.FindScreenContaining(gunup_tancaporta->pos);


	new BigItem(TiledEntities::walljump, Skill::WALLJUMP);
	new BigItem(TiledEntities::gun, Skill::GUN);

	for (const vec& v : TiledEntities::healthup) {
		new HealthUp(v);
	}

	for (const vec& v : TiledEntities::enemy_door) {
		EnemyDoor* d = new EnemyDoor(v);
		int door_screen = screenManager.FindScreenContaining(d->pos);
		if (door_screen < 0) {
			Debug::out << "Warning: Enemy door outside a screen";
		}
		for (const Bat* b : Bat::GetAll()) {
			if (b->screen == door_screen) {
				d->AddEnemy(b);
			}
		}
		if (door_screen == screen_gunup_tancaporta) {
			gunup_tancaporta_door = d;
		}
	}

	for (const Bounds& a : TiledAreas::lava) {
		Lava* lava = new Lava(a);
		if (a.Contains(TiledEntities::lava_initial_height)) {
			raising_lava = lava;
			raising_lava_target_height = lava->CurrentLevel();
			lava->SetLevel(TiledEntities::lava_initial_height.y, true);
		}
	}

	screenManager.UpdateCurrentScreen(player.pos);

	Camera::SetCenter(GetCameraTargetPos());

	FxManager::StartIntroTransition(introDuration);
}

vec JumpScene::GetCameraTargetPos() {
	// TODO: keep the camera so you see a bit more in the direction you are going (like in https://youtu.be/AqturoCh5lM?t=3801)
	vec camPos = (player.pos * 17 + Mouse::GetPositionInWorld() * 2) / 19.f;
	screenManager.ClampCameraToScreen(camPos);
	return camPos;
}

void JumpScene::UpdateCamera(float dt) {
	vec camPos = GetCameraTargetPos();
	vec oldPos = Camera::GetCenter();
	vec displacement = camPos - oldPos;
	displacement.Truncate(camSpeed * dt);
	Camera::SetCenter(oldPos + displacement + FxManager::GetScreenshake());
}

void JumpScene::ExitScene()
{
	Bullet::DeleteAll();
	Bullet::particles.Clear();
	Missile::DeleteAll();
	Missile::particles.Clear();
	Bat::DeleteAll();
	Goomba::DeleteAll();
	Bipedal::DeleteAll();
	Lava::DeleteAll();
	destroyedTiles.Clear();
	EnemyDoor::DeleteAll();
	BigItem::DeleteAll();
	HealthUp::DeleteAll();
	GunUp::DeleteAll();
}
void JumpScene::Update(float dt)
{
	FxManager::Update(dt);

	if (FxManager::IsIntroTransitionActive() || FxManager::IsOuttroTransitionActive()) {
		return;
	}
	else if (FxManager::IsOuttroTransitionDone()) {
		FxManager::ResetOuttroTransitionDone();
		// Restart scene
		ExitScene();
		EnterScene();
		return;
	}

	skillTree.Update(dt);
	if (skillTree.open) return;

	player.Update(dt);
	if (!player.alive) {
		FxManager::StartOuttroTransition(introDuration);
	}

	screenManager.UpdateCurrentScreen(player.pos);

	UpdateCamera(dt);

	for (Bipedal* e : Bipedal::GetAll()) {
		e->Update(dt);
	}

	for (Bullet* e  : Bullet::GetAll()) {
		e->Update(dt);
	}
	Bullet::DeleteNotAlive();

	for (Missile* e  : Missile::GetAll()) {
		e->Update(dt);
	}
	Missile::DeleteNotAlive();

	for (Bat* e : Bat::GetAll()) {
		e->Update(dt);
		for (Bullet* b : Bullet::GetAll()) {
			if (b->explode) continue;
			if (Collide(e, b)) {
				b->pos = e->pos;
				b->explode = true;
				e->alive = false;
				Bat::AwakeNearbyBats(e->pos);
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

	for (Goomba* e : Goomba::GetAll()) {
		e->Update(dt);
		for (Bullet* b : Bullet::GetAll()) {
			if (b->explode) continue;
			if (Collide(e->bounds(), b->bounds())) {
				b->pos = e->pos;
				b->explode = true;
				e->alive = false;
				Bat::AwakeNearbyBats(e->pos);
				break;
			}
		}
		if (!player.isInvencible()) {
			if (Collide(player.bounds(), e->bounds())) {
				player.takeDamage(e->pos);
			}
		}
	}


#ifdef _DEBUG
	const SDL_Scancode killall = SDL_SCANCODE_F11;
	const SDL_Scancode unlockbasics = SDL_SCANCODE_F8;
	const SDL_Scancode gotoplaces = SDL_SCANCODE_F9;
	const SDL_Scancode screen_left = SDL_SCANCODE_F6;
	const SDL_Scancode screen_right = SDL_SCANCODE_F7;
	const SDL_Scancode restart = SDL_SCANCODE_F5;
	if (Keyboard::IsKeyJustPressed(restart)) {
		FxManager::StartOuttroTransition(introDuration);
		return;
	}
	if (Keyboard::IsKeyJustPressed(gotoplaces)) {
		player.pos = vec(1748, 1084);
		for (Bipedal* e : Bipedal::GetAll()) {
			e->state = Bipedal::State::FIRING;
			e->timer = 0;
		}
	}
	if (Keyboard::IsKeyJustPressed(unlockbasics)) {
		skillTree.Enable(Skill::GUN);
		skillTree.Enable(Skill::WALLJUMP);
		skillTree.Enable(Skill::BREAK);
	}
	if (Keyboard::IsKeyJustPressed(screen_left)) {
		player.pos.x -= Window::GAME_WIDTH;
	}
	if (Keyboard::IsKeyJustPressed(screen_right)) {
		player.pos.x += Window::GAME_WIDTH;
	}
	if (Keyboard::IsKeyJustPressed(killall)) {
		for (Bat* e : Bat::GetAll()) {
			if (e->screen == screenManager.CurrentScreen()) {
				(new Bullet(e->pos, vec(), 1.5f))->explode = true;
				e->alive = false;
			}
		}
	}
	if (Debug::Draw && Keyboard::IsKeyPressed(SDL_SCANCODE_LSHIFT)) {
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

	for (EnemyDoor* ed : EnemyDoor::GetAll()) {
		ed->Update(dt); // Checks for enemies with alive = false, crashes if they have been deleted already
	}

	Bat::DeleteNotAlive(); //Must happen after enemydoor update
	Goomba::DeleteNotAlive();

	for (GunUp* g : GunUp::GetAll()) {
		if (Collide(g->bounds(), player.bounds())) {

			//TODO: PICK UP ANIMATION

			skillTree.gunpoints++;
			g->alive = false;

			if (g == gunup_tancaporta) {
				gunup_tancaporta_door->Lock();
			}
		}
	}

	GunUp::DeleteNotAlive();

	for (HealthUp* g : HealthUp::GetAll()) {
		if (Collide(g->bounds(), player.bounds())) {

			//TODO: PICK UP ANIMATION

			//TODO: Raise health

			g->alive = false;
		}
	}

	HealthUp::DeleteNotAlive();

	for (BigItem* g : BigItem::GetAll()) {
		if (Collide(g->bounds(), player.bounds())) {

			//TODO: PICK UP ANIMATION

			skillTree.Enable(g->skill);
			g->alive = false;

			switch(g->skill) {
			case Skill::WALLJUMP:
				raising_lava->SetLevel(raising_lava_target_height);
				rotoText.ShowMessage("WallJump");
				break;
			case Skill::GUN:
				rotoText.ShowMessage("Big F. Gun");
				for (const vec& v : TiledEntities::initial_batawake) {
					Bat* b = new Bat(v, false, true);
					gunup_tancaporta_door->AddEnemy(b);
				}
				break;
			default:
				break;
			}
		}
	}

	BigItem::DeleteNotAlive();

	Bullet::particles.UpdateParticles(dt);
	Missile::particles.UpdateParticles(dt);

	destroyedTiles.Update(dt);

	if (raising_lava->CurrentLevel() <= raising_lava_target_height+1.f) {
		raising_lava->SetLevel(TiledEntities::lava_initial_height.y);
	}

	for (Lava* l : Lava::GetAll()) {
		l->Update(dt);
	}

	contextActionButton = GameKeys::NONE;
	for (SaveStation* ss : SaveStation::GetAll()) {
		ss->Update(dt);
		if (ss->enabled && Collide(ss->bounds(), player.bounds())) {
			contextActionButton = GameKeys::ACTION;
			if (Input::IsJustPressed(0,GameKeys::ACTION)) {
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

	rotoText.Update(dt);

}

void JumpScene::Draw()
{
	if (skillTree.open) {
		skillTree.DrawMenu();
		return;
	}

	FxManager::BeginDraw();

	Window::Clear(31, 36, 50);

	for (const Parallax* p : Parallax::GetAll()) {
		p->Draw();
	}

	map.Draw();
	destroyedTiles.Draw();

	for (const SaveStation* ss : SaveStation::GetAll()) {
		ss->Draw();
	}

	for (const EnemyDoor* ed : EnemyDoor::GetAll()) {
		ed->Draw();
	}

	for (const Bat* e : Bat::GetAll()) {
		e->Draw();
		if (Debug::Draw && Camera::GetBounds().Contains(e->pos)) {
			e->DrawBounds();
			e->DrawSenseArea();
		}
	}

	for (const Goomba* e : Goomba::GetAll()) {
		e->Draw();
	}


	for (const Bipedal* e : Bipedal::GetAll()) {
		e->Draw();
	}
	Bullet::particles.Draw();
	//Bullet::particles.DrawImGUI("BulletTrail");

	Missile::particles.Draw();
	Missile::particles.DrawImGUI("MissileSmoke");

	for (const Bullet* e : Bullet::GetAll()) {
		e->Draw();
		if (Debug::Draw) {
			e->DrawBounds();
		}
	}
	
	for (const Missile* e : Missile::GetAll()) {
		e->Draw();
		if (Debug::Draw) {
			e->DrawBounds();
		}
	}


	for (const GunUp* g : GunUp::GetAll()) {
		g->Draw();
		if (Debug::Draw) {
			g->bounds().Draw();
		}
	}
	for (const HealthUp* g : HealthUp::GetAll()) {
		g->Draw();
		if (Debug::Draw) {
			g->bounds().Draw();
		}
	}
	for (BigItem* g : BigItem::GetAll()) {
		g->Draw();
		if (Debug::Draw) {
			g->bounds().Draw();
		}
	}

	player.Draw();

	if (contextActionButton != GameKeys::NONE) {
		AnimationType anim = BUTTON_A_PRESS; // TODO: switch depending on the key
		Window::Draw(Assets::hospitalTexture, player.bounds().TopRight() + vec(2, -6))
			.withRect(Animation::AnimFrame(anim, mainClock * 1000));
	}

	for (const Lava* l : Lava::GetAll()) {
		l->Draw();
	}
	
	rotoText.Draw(vec(0,-30));

	if (Debug::Draw) {
		player.bounds().Draw();
		//player.pos.Debuggerino(sf::Color::White);
		//player.bounds().Center().Debuggerino(sf::Color::Magenta);
	}

#ifdef _IMGUI
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
		ImGui::SliderFloat("lava", &(Lava::GetAll()[0]->targetY), (TiledMap::map_size.y - 1) * 16, (TiledMap::map_size.y - 1) * 16 - 1000);
		ImGui::End();

		//FxManager::DrawImgui();
	}
#endif

#ifdef _DEBUG
	if (Debug::Draw) {
		vec pos = Camera::GetBounds().TopLeft() + vec(0, 16);
		Bounds(pos, vec(Tile::size, Tile::size)).Draw(0,0,0);
		Window::Draw(Assets::marioTexture, pos)
			.withRect(Tile::tileToTextureRect[currentPlacingTile]);
	}
#endif

	//for (const Bounds& a : TiledAreas::parallax_forest) {
		//Assets::fogShader.Activate();
		//Assets::fogShader.SetUniform("offset", vec(mainClock*0.2f, 0.f));
		//Assets::fogShader.SetUniform("time", mainClock);
		// The texture is not used by the shader at all
		//Window::Draw(Assets::hospitalTexture, a);
		//Shader::Deactivate();
	//}

	//fogPartSys.Draw();
	//fogPartSys.DrawImGUI();

	FxManager::EndDraw();

	skillTree.DrawOverlay();

	//player.polvito.DrawImGUI("Polvito");
}
