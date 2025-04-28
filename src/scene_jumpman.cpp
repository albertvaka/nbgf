#include "scene_jumpman.h"
#include "plant.h"
#include "input.h"
#include "imgui.h"
#include "assets.h"
#include "debug.h"
#include "musicplayer.h"
#include "doggo.h"
#include "stats_tracker.h"
#include "cistell.h"
#include "sound.h"

extern float mainClock;

const int kInitialMoney = 120;
const int kMoneySellTomatoes = 30;
const int kMoneyBuyPlant = 100; //Super const, actually
const float kDoggoInterval = 90; //seconds
const float kinitialDoggoInterval = 40; //seconds

static JumpMan* AddPlayer(int id) {
	auto* player = new JumpMan(id);
	player->pos = TiledEntities::spawn;
	player->Carry(new Cistell());
	return player;
}

JumpScene::JumpScene(int first_player)
	: map(TiledMap::map_size.x, TiledMap::map_size.y)
	, totalMoneyText(Assets::font_30, Assets::font_30_outline)
	, moneyText(Assets::font_30, Assets::font_30_outline)
	, first_player(first_player)
{
	map.LoadFromTiled();
}

void JumpScene::EnterScene() 
{
	lost = false;
	for (int i = 0; i < Input::kMaxPlayers; ++i) {
		players[i] = nullptr;
	}
	auto* player = new JumpMan(first_player);
	player->pos = TiledEntities::spawn;
	players[first_player] = player;

	StatsTracker::Restart();

#ifdef _DEBUG
	new Plant(vec(8.0f, 8.0f) + TileMap::alignToTiles(310.0f, 260.0f));
#endif

	moneys = kInitialMoney;

	timerDoggo = kDoggoInterval - kinitialDoggoInterval;

	auto* cistell = new Cistell();
	cistell->carrier = nullptr;
	cistell->pos = vec(550,355);

	npc.Reset();

	MusicPlayer::Play(Assets::sceneMusic[current_music]);
}

void JumpScene::ExitScene()
{
	JumpMan::DeleteAll();
	Cistell::DeleteAll();
	Plant::DeleteAll();
	Doggo::DeleteAll();
	Doggo::ResetDestinations();
}

void JumpScene::Update(float dt)
{
	StatsTracker::Update(dt);

	if (Input::IsJustPressedAnyPlayer(GameKeys::NEXT_TRACK)) {
		current_music = 1-current_music;
		MusicPlayer::Play(Assets::sceneMusic[current_music]);
	}
	if (Input::IsJustPressedAnyPlayer(GameKeys::MUTE)) {
		if (MusicPlayer::IsPlaying()) MusicPlayer::Pause();
		else MusicPlayer::Resume();
	}

	//Camera::MoveCameraWithArrows(50, dt);
	for (auto* player : JumpMan::GetAll()) {
    if (Input::IsJustPressed(player->id, GameKeys::RESTART)) {
      ExitScene();
      EnterScene();
		  return; //stop this loop, since the players we were iterating no longer exist
		}
	}

	if (lost) {
		rototext.Update(dt);
		return;
	}

	for (int i = 0; i < Input::kMaxPlayers; ++i) {
    contextActionButton[i] = GameKeys::NONE;
	}

	for (int i = 0; i < Input::kMaxPlayers; ++i) {
		if (players[i] == nullptr && 
				(Input::IsJustPressed(i, GameKeys::ACTIVATE) || Input::IsJustPressed(i, GameKeys::START))) {
				Input::ConsumeJustPressed(i, GameKeys::ACTIVATE);
		  players[i] = AddPlayer(i);
		}
	}

	for (auto* player : JumpMan::GetAll()) {
    player->Update(dt);
	}
	for (Doggo* doggo : Doggo::GetAll()) {
		for (auto* player : JumpMan::GetAll()) {
			if (player->IsCarrying(JumpMan::Holdable::Basket) && player->cistell->contents == Cistell::TOMATOES && Collide(player->Bounds(), doggo->Bounds())) {
        contextActionButton[player->id] = GameKeys::ACTIVATE;
        if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
          Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
          // Assets::DoggoEatingSound.Play();
          doggo->Feed();
          player->cistell->contents = Cistell::EMPTY;
          break;
        }
			}
		}

		doggo->Update(dt);
		if (doggo->menjar <= 0) {
			doggo->menjar = 1;
			lost = true;
			StatsTracker::Stop();
			rototext.ShowMessage("A DOGGO STARVED");
		}
	}

	//Debug::out << Camera::GetCameraCenter();


	npc.Update(dt);
	for (auto* cistell : Cistell::GetAll()) {
    cistell->Update(dt);
	}

	timerDoggo += dt;
	if (timerDoggo >= kDoggoInterval) {
		timerDoggo = 0;
		new Doggo();
	}

	// Sell tomatoes.
	for (auto* player : JumpMan::GetAll()) {
		if (player->IsCarrying(JumpMan::Holdable::Basket) && player->cistell->contents == Cistell::TOMATOES && Collide(player->Bounds(), TiledAreas::truck)) {
		  contextActionButton[player->id] = GameKeys::ACTIVATE;
		  if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
			Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
			moneys += kMoneySellTomatoes;
			++StatsTracker::tomatoes_delivered;
			if (Rand::OnceEvery(2)) {
			  Assets::soundSell1.Play();
			}
			else {
			  Assets::soundSell2.Play();
			}
			moneyTextTimer = 0.5f;
			moneyText.SetFillColor(0, 255, 0);
			moneyText.SetString("$" + std::to_string(kMoneySellTomatoes));
			player->cistell->contents = Cistell::EMPTY;
		  }
		}
	}

	// All actions with the Player already carrying a Bucket.
	for (auto* player : JumpMan::GetAll()) {
		auto* cistell = player->cistell;
		if (cistell == nullptr) {
			continue;
		}

    for (auto* plant : Plant::GetAll()) {
      // Agafar tom�quets.
      if (Collide(plant->Bounds(), cistell->Bounds())) {
        // Agafar Tom�euts.
        if ((!cistell->contents) && plant->HasTomato()) {
          // TODO: Treure aix� si la collita passa automaticament.
          contextActionButton[player->id] = GameKeys::ACTIVATE;
          if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
            Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
            plant->PickTomato();
            cistell->contents = Cistell::TOMATOES;
            Assets::soundBucketTomatoes.Play();
          }
        }
        // Regar plantes.
        else if (cistell->contents == Cistell::WATER) {
          // TODO: Agafar la planta que necessiti m�s aigua?
          contextActionButton[player->id] = GameKeys::ACTIVATE;
          if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
            Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
            plant->SetHitByWater();
            cistell->contents = Cistell::EMPTY;
            Assets::soundWater.Play();
          }
        }

      }
    }
    // Agafar aigua.
    if (Collide(TiledAreas::water, cistell->Bounds()) && !cistell->contents) {
      contextActionButton[player->id] = GameKeys::ACTIVATE;
      if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
        Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
        cistell->contents = Cistell::WATER;
        Assets::soundBucketWater.Play();
      }
    }
    // Drop Bucket
    if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE) && player->grounded) {
      Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
      player->DropItem();
      Assets::soundPickupDrop.Play();
    }
	}


	// Pick up Bucket by Player
	for (auto* player : JumpMan::GetAll()) {
		for (auto* cistell : Cistell::GetAll()) {
			if (Collide(player->Bounds(), cistell->Bounds()) && player->CanCarry() && !cistell->IsBeingCarried()) {
        contextActionButton[player->id] = GameKeys::ACTIVATE;
        if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
					player->Carry(cistell);
          Assets::soundPickupDrop.Play();
				}
			}
		}
	}

	for (auto* plant : Plant::GetAll()) {
		// Check if any player can pick up the plant.
		for (auto* player : JumpMan::GetAll()) {
      if (Collide(plant->Bounds(), player->Bounds())) {
        bool can_carry = !plant->IsBeingCarried() && player->CanCarry();
        bool can_drop = plant->IsCarriedBy(player) && player->grounded;
        if (can_carry) {
          contextActionButton[player->id] = GameKeys::ACTIVATE;
        }
        if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
          if (can_drop) {
            Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
            player->DropItem();
            Assets::soundPickupDrop.Play();
          } else if (can_carry) {
            Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
            player->Carry(plant);
            Assets::soundPickupDrop.Play();
          }
        }
      }
		}

		plant->SetHitByLight(Collide(plant->Bounds(), TiledAreas::sun));
		plant->Update(dt);
	}


	// Buy plants from NPCs.
	for (auto* player : JumpMan::GetAll()) {
    if (npc.isSelling() && Collide(player->Bounds(), TiledAreas::npc) && player->IsCarrying(JumpMan::Holdable::None)) {
      contextActionButton[player->id] = GameKeys::ACTIVATE;
      if (Input::IsJustPressed(player->id, GameKeys::ACTIVATE)) {
        Input::ConsumeJustPressed(player->id, GameKeys::ACTIVATE);
        if (moneys >= kMoneyBuyPlant) {
          moneys -= kMoneyBuyPlant;
          moneyTextTimer = 0.5f;
		  moneyText.SetFillColor(255, 0, 0);
		  moneyText.SetString("-$" + std::to_string(kMoneyBuyPlant));
          ++StatsTracker::plants_purchased;
          player->Carry(new Plant(vec()));
          Assets::soundBuy.Play();
        }
        else {
          cantbuyTimer = dt*1000 + 1000;
        }
      }
    }
	}

	moneyTextTimer -= dt;

#ifdef _DEBUG
	const SDL_Scancode DEBUG_FILL_CISTELL = SDL_SCANCODE_F3;
	const SDL_Scancode DEBUG_DOGGO = SDL_SCANCODE_EQUALS;
	const SDL_Scancode DEBUG_MAKE_PLANTS_HAPPY = SDL_SCANCODE_F8;
	const SDL_Scancode DEBUG_GET_MONEY = SDL_SCANCODE_F6;

	if (Keyboard::IsKeyJustPressed(DEBUG_GET_MONEY)) {
		moneys += 100;
		moneyTextTimer = 0.5f;
		moneyText.SetFillColor(0, 255, 0);
		moneyText.SetString("$100");
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_MAKE_PLANTS_HAPPY)) {
		for (Plant* plant : Plant::GetAll()) {
			plant->water = 50;
			plant->light = 50;
		}
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_DOGGO)) {
		new Doggo();
	}

	if (Keyboard::IsKeyJustPressed(DEBUG_FILL_CISTELL)) {

		for (Cistell* cistell : Cistell::GetAll()) {
			if (cistell->contents == Cistell::EMPTY) {
				cistell->contents = Cistell::TOMATOES;
			}
			else if (cistell->contents == Cistell::TOMATOES) {
				cistell->contents = Cistell::WATER;
			}
			else if (cistell->contents == Cistell::WATER) {
				cistell->contents = Cistell::EMPTY;
			}
		}
	}
#endif

}

// El sol
// <-0s---------------------------------------------------13s------>15s
// |<-----------Sol0--------->                             |
// |                   <--------Sol1----------->           |
// |                                    <--------Sol2------+------->
// +--Sol2--->                                             |
static const float SunLimits[4][2] =
{
  {0.0f, 7.0f},
  {4.0f, 11.0f},
  {8.0f, 15.0f},
  {-4.0f, 3.0f}, // Repetir Sol2 per al principi
};

static void DrawSun()
{
	float time = mainClock;
	time -= int(time / 13) * 13;

	for (int i = 0; i < 4; ++i) {
		if (time >= SunLimits[i][0] && time <= SunLimits[i][1]) {
			const float mid = (SunLimits[i][0] + SunLimits[i][1]) / 2.0f;
			const float half_size = (SunLimits[i][1] - SunLimits[i][0]) / 2.0f;
			float ratio = (time < mid ? (time - SunLimits[i][0]) / half_size : 1.0f - (time - mid) / half_size);
			assert(ratio >= 0 && ratio <= 1.0f);

			Window::Draw(Assets::sunTextures[std::min(i, 2)], vec(217.0f, 0.0f))
				.withColor(255,255,255,50 * ratio);
		}
	}
}

void JumpScene::Draw()
{
	Window::Clear(34, 32, 52);

	Window::Draw(Assets::casaTexture, vec(0, 0));

	if (Debug::Draw) {
		map.Draw();
	}

	int globalMillis = int(mainClock * 1000);

	//Fountain
	Window::Draw(Assets::spritesTexture, vec(413, 329))
		.withRect(Animation::GetRectAtTime(FOUNTAIN, globalMillis))
		.withScale(1.8f, 1.45f);

	npc.Draw();

	for (Doggo* doggo : Doggo::GetAll()) {
		if (doggo->wantFood) {
			doggo->Draw();
		}
	}

	for (auto* plant : Plant::GetAll()) {
		plant->Draw();
	}

	for (const auto* cistell : Cistell::GetAll()) {
		cistell->Draw();
	}
	for (const auto* player : JumpMan::GetAll()) {
		player->Draw();
	}
	for (const auto* cistell : Cistell::GetAll()) {
		if (cistell->IsBeingCarried()) {
			cistell->Draw();
		}
	}
	for (auto* plant : Plant::GetAll()) {
		if (plant->IsBeingCarried()) {
			plant->Draw();
			break;
		}
	}

	//Truck
	Window::Draw(Assets::spritesTexture, vec(702, 297))
		.withRect(0, 8 * 16, 11 * 16 + 4, 72);

	for (Doggo* doggo : Doggo::GetAll()) {
		if (!doggo->wantFood) {
			doggo->Draw();
		}
	}

	// Sunny sun.
	DrawSun();

	for (auto* player : JumpMan::GetAll()) {
		if (contextActionButton[player->id] != GameKeys::NONE) {
			const AnimationFrame(&anim)[3] = BUTTON_A_PRESS; // TODO: switch depending on the key
			Window::Draw(Assets::hospitalTexture, player->Bounds().TopRight() + vec(2, -6))
				.withRect(Animation::GetRectAtTime(anim, globalMillis));
		}
	}


	if (cantbuyTimer > globalMillis && (globalMillis / 100) % 2) {
		totalMoneyText.SetFillColor(255, 0, 0);
	}
	else {
		totalMoneyText.SetFillColor(0, 255, 255);
	}
	totalMoneyText.SetString("$" + std::to_string(moneys));
	Window::Draw(totalMoneyText, vec(10,10))
		.withScale(0.8f);

	// TODO: Move text as a property of the player
	for (const auto* player : JumpMan::GetAll()) {
		if (moneyTextTimer > 0) {
			vec size = moneyText.Size();
			vec pos = player->Bounds().Center() - vec(size.x / 4, player->Bounds().height / 2 + size.y - 35 * moneyTextTimer);
			Window::Draw(moneyText, pos).withScale(0.45f);
		}
	}


	if (Debug::Draw) {
		TiledAreas::sun.DebugDraw();
		TiledAreas::npc.DebugDraw();
		TiledAreas::truck.DebugDraw();
		TiledAreas::water.DebugDraw();

		for (const auto* player : JumpMan::GetAll()) {
			player->Bounds().DebugDraw();
			for (const auto& plant : Plant::GetAll()) {
				plant->Bounds().DebugDraw();
			}
			//player.pos.DebugDraw(sf::Color::White);
			//player.Bounds().Center().DebugDraw(sf::Color::Magenta);
		}
	}

	if (lost) {
		StatsTracker::Draw();
		rototext.Draw();
	}
	//player.polvito.DrawImGUI("Polvito");
}


