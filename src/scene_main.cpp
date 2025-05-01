#include "scene_main.h"

#include "imgui.h"
#include "raw_input.h"
#include "assets.h"
#include "window.h"
#include "imgui_convert.h"
#include "input.h"
#include "debug.h"
#include "camera.h"
#include <algorithm>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include "rand.h"
#include "animation.h"
#include "musicplayer.h"
#include "minion.h"
#include "gaem_constants.h"

namespace std
{
	template <>
	struct hash<veci>
	{
		size_t operator()(veci const& v) const noexcept
		{
			return (
				(200 + std::hash<int>()(v.x) * 200
					+ std::hash<int>()(v.y)
					)
				);
		}
	};
}

using namespace std;

const veci shopPos(30, 4);
const veci loveFactoryPos(27, 14);
const veci minePos[6] =
{
	veci(6, 3),
	veci(6, 5),
	veci(6, 7),

	veci(9, 4),
	veci(9, 6),
	veci(9, 8),
};

struct Food
{
	veci pos;
	bool eaten = false;

	Food(veci p) : pos(p)
	{
	}
};

Food* draggingFood = nullptr;

struct Key
{
	veci pos;

	Key(veci p) : pos(p)
	{
	}
};

struct GaemData
{
	int money;
	std::vector<Minion*> minions;
	std::vector<Food*> food;
	std::vector<Key*> keys;

	int minions_purchased = 0;
	int keys_purchased = 0;

	bool map_passable[40][23];

	bool unlocked_shapes[(int)MinionShape::SHAPE_COUNT];
	bool unlocked_colors[6];

};

GaemData gdata;

Minion* draggingMinion = nullptr;
Key* draggingKey = nullptr;

Minion* sexMinion1 = nullptr;
Minion* sexMinion2 = nullptr;

Minion* miningMinions[6] = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };

Minion* merchantWantsMinion = nullptr;

bool gameStarted = false;

int sexTime = -1;

float miningTime[6] = { -1, -1, -1, -1, -1, -1 };
int miningEffectTime[6] = { -99999, -99999, -99999, -99999, -99999, -99999 };
int miningCoinEffectTime[6] = { -1, -1, -1, -1, -1, -1 };
bool unlockedMines[6] = { 0,0,0,0,0,1 };

int merchantTime = 25000;
int merchantLeavingTime = 0;
int gameTime = 0;
int timeMinionSelled = -1;


veci Pathfinder_NextTile(GaemData* gd, veci from, veci to)
{

	std::unordered_set< veci > visited;
	std::unordered_map< veci, veci > comingFrom;
	comingFrom[from] = from;

	std::queue< veci > next_tiles;
	next_tiles.push(from);

	while (!next_tiles.empty())
	{
		veci next = next_tiles.front();
		next_tiles.pop();

		if (next == to)
		{ //Found!
			veci next_to_go = to;
			while (comingFrom[next_to_go] != from)
			{
				next_to_go = comingFrom[next_to_go];
			}
			return next_to_go;
		}

		visited.insert(next);

		bool passable = gd->map_passable[next.x][next.y];
		if (!passable)
		{
			continue;
		}


		if (next == from || passable)
		{
			veci p_left = next;
			p_left.x += 1;
			if (visited.find(p_left) == visited.end())
			{
				next_tiles.push(p_left);
				comingFrom[p_left] = next;
			}

			veci p_right = next;
			p_right.x -= 1;
			if (visited.find(p_right) == visited.end())
			{
				next_tiles.push(p_right);
				comingFrom[p_right] = next;
			}

			veci p_up = next;
			p_up.y += 1;
			if (visited.find(p_up) == visited.end())
			{
				visited.insert(p_up);
				next_tiles.push(p_up);
				comingFrom[p_up] = next;
			}

			veci p_down = next;
			p_down.y -= 1;
			if (visited.find(p_down) == visited.end())
			{
				visited.insert(p_down);
				next_tiles.push(p_down);
				comingFrom[p_down] = next;
			}

		}

	}
	return from;
}


int Tilemap_Distance(veci from, veci to)
{
	return std::abs(to.y - from.y) + std::abs(to.x - from.x);
}

void Minion_Update(Minion* m, GaemData* gd, int dt)
{
	/*
	static int timer_warrer = 0;
	timer_warrer += dt;
	if (timer_warrer > 500)
	{
		m->gens.shape = (MinionShape)(((int)m->gens.shape + 1) % ((int)MinionShape::SHAPE_COUNT));
		timer_warrer -= 500;
	}
	*/

	m->lifetime += dt;

	switch (m->state)
	{
	case MinionState::EGG:
	{
		if (m->lifetime > m->eggtime)
		{
			m->state = MinionState::ALIVE;
			m->anim = AnimationType::IDLE;
			m->pos_target = m->pos;
		}
	} break;

	case MinionState::ALIVE:
	{
		for (Food* food : gd->food)
		{
			if (!food->eaten && food != draggingFood && food->pos == m->pos)
			{
				food->eaten = true;
				m->timer = 0;
				m->state = MinionState::EATING;
				return;
			}
		}

		if (Rand::roll(100) < 10)
		{
			m->satiation -= dt;
		}

		if (m->satiation <= 0)
		{
			m->state = MinionState::RIP;
			m->anim = AnimationType::RIP;
		}

		m->wait_delay -= dt;
		if (m->wait_delay > 0)
		{
			return;
		}


		if (m->satiation < 5000 && gd->food.size() > 0)
		{
			for (Food* food : gd->food)
			{
				if (!food->eaten && Tilemap_Distance(food->pos, m->pos) < 15)
				{
					m->pos_target = food->pos;
					break;
				}
			}
		}
		if (m->pos_target == m->pos)
		{
			int x, y;
			do {
				x = Rand::roll(0, 40 - 1);
				y = Rand::roll(0, 23 - 1);
			} while (!gd->map_passable[x][y]);

			m->pos_target.x = x;
			m->pos_target.y = y;
		}

		gd->map_passable[m->pos.x][m->pos.y] = true;
		veci pos_maybe = Pathfinder_NextTile(gd, m->pos, m->pos_target);
		if (m->pos == pos_maybe)
		{
			//Stuck
		}
		if (gd->map_passable[pos_maybe.x][pos_maybe.y])
		{
			m->pos = pos_maybe;
		}
		gd->map_passable[m->pos.x][m->pos.y] = false;

		m->wait_delay = m->gens.walk_speed_delay;


	} break;


	case MinionState::EATING:
	{
		m->timer += dt;
		if (m->timer > 3000)
		{
			m->satiation = 10000;
			m->state = MinionState::ALIVE;
		}

	} break;

	case MinionState::MINING:
	{
		if (Rand::roll(100) < 20)
		{
			m->satiation -= dt;
		}
		if (m->satiation <= 0)
		{
			m->state = MinionState::RIP;
			m->anim = AnimationType::RIP;
		}
	} break;

	case MinionState::RIP:
	{
		//Be dead

	} break;


	}

}

void DrawMinionStats(Minion* m)
{
	if (m->state == MinionState::EGG) return;

	ImGui::SetNextWindowPosInGameCoords(vec(m->pos.x * 32 + 40, m->pos.y * 32));
	ImGui::Begin("stats", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
	ImGui::Text("Hunger");
	ImGui::ProgressBar(m->satiation / 10000.f);
	ImGui::Text("Mining");
	ImGui::ProgressBar((m->gens.mining_multiplier - MIN_MINING_MULTIPLIER) / (MAX_MINING_MULTIPLIER - MIN_MINING_MULTIPLIER));
	ImGui::Text("Speed");
	ImGui::ProgressBar(1 - (float(m->gens.walk_speed_delay - MIN_WALK_SPEED_DELAY) / (MAX_WALK_SPEED_DELAY - MIN_WALK_SPEED_DELAY)));
	ImGui::End();
}

struct Coin
{
	int x;
	float y;
	float t;
	bool alive = true;

	float r = Rand::rollf(0.01f, 0.45f);

	void Update(int dt)
	{
		t -= dt;

		if (t < 0)
		{
			alive = false;
		}

		y -= dt * r;

	}
};

void Merchant_Unlock(GaemData* gd, Minion* m)
{
	gd->unlocked_colors[m->gens.color_key] = true;
	gd->unlocked_shapes[(int)m->gens.shape] = true;
}



SceneMain::SceneMain()
{
	ImGui::GetStyle().WindowRounding = 7.0;
}

void SceneMain::EnterScene() 
{
	gdata.money = STARTING_MONEY;

	for (int i = 0; i < 6; ++i)
	{
		gdata.unlocked_colors[i] = false;
	}
	for (int i = 0; i < (int)MinionShape::SHAPE_COUNT; ++i)
	{
		gdata.unlocked_shapes[i] = false;
	}


	for (int i = 0; i < 40; ++i)
	{
		for (int j = 0; j < 23; ++j)
		{
			gdata.map_passable[i][j] = true;
		}
	}

	//Water
	for (int i = 0; i < 40; ++i)
	{
		gdata.map_passable[i][0] = false;
		gdata.map_passable[i][23 - 1] = false;
		gdata.map_passable[i][1] = false;
		gdata.map_passable[i][23 - 2] = false;
	}
	for (int i = 0; i < 23; ++i)
	{
		gdata.map_passable[0][i] = false;
		gdata.map_passable[40 - 1][i] = false;
		gdata.map_passable[1][i] = false;
		gdata.map_passable[40 - 2][i] = false;
	}
	for (int i = 2; i < 9; ++i)
	{
		for (int j = 15; j <= 20; ++j)
		{
			gdata.map_passable[i][j] = false;

		}
	}
	//Shop
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			gdata.map_passable[shopPos.x + i][shopPos.y + j] = false;
		}
	}
	//Love factory
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			gdata.map_passable[loveFactoryPos.x + i][loveFactoryPos.y + j] = false;
		}
	}

	//Mine

	for (int i = 0; i < 6; ++i)
	{
		gdata.map_passable[minePos[i].x][minePos[i].y] = false;
		gdata.map_passable[minePos[i].x + 1][minePos[i].y] = false;
	}


	// Map stuff
	gdata.map_passable[2][9] = false;
	gdata.map_passable[3][9] = false;
	gdata.map_passable[2][7] = false;

	gdata.map_passable[2][11] = false;
	gdata.map_passable[2][12] = false;
	gdata.map_passable[2][13] = false;
	gdata.map_passable[2][14] = false;
	gdata.map_passable[3][11] = false;
	gdata.map_passable[3][12] = false;
	gdata.map_passable[3][13] = false;
	gdata.map_passable[3][14] = false;
	gdata.map_passable[4][14] = false;


	for (int i = 9; i <= 12; i++) {
		for (int j = 13; j <= 20; j++) {
			gdata.map_passable[i][j] = false;
		}
	}
	gdata.map_passable[12][14] = true;
	gdata.map_passable[12][15] = true;
	gdata.map_passable[9][15] = true;
	gdata.map_passable[10][15] = true;
	gdata.map_passable[11][15] = true;
	gdata.map_passable[9][16] = true;
	gdata.map_passable[10][16] = true;
	gdata.map_passable[9][17] = true;
	gdata.map_passable[9][18] = true;
	gdata.map_passable[9][19] = true;
	gdata.map_passable[6][19] = true;
	gdata.map_passable[7][19] = true;
	gdata.map_passable[8][19] = true;
	gdata.map_passable[11][13] = true;
	gdata.map_passable[12][13] = true;


	for (int i = 29; i <= 37; i++) {
		for (int j = 2; j <= 3; j++) {
			gdata.map_passable[i][j] = false;
		}
	}
	gdata.map_passable[37][4] = false;


	Minion* m = new Minion(veci(20, 10));
	gdata.minions.push_back(m);
	Merchant_Unlock(&gdata, m);

	m = new Minion(veci(21, 12));
	gdata.minions.push_back(m);
	Merchant_Unlock(&gdata, m);

}

void SceneMain::ExitScene()
{
}

int dt;

void SceneMain::Update(float actual_dt)
{
	// Update happens in draw
	dt = actual_dt * 1000;

	// Handle window resizes by scaling the font
	// This only affects the font, though, not any images, paddings, etc.
	// And because of this we need to pass ImGuiWindowFlags_AlwaysAutoResize to all windows we create
	ImGui::GetIO().FontGlobalScale = Window::GetViewportScale();
}

void SceneMain::Draw()
{
	gameTime += dt;

	Window::Clear(0,0,0);

	Window::Draw(Assets::mapspr, Camera::Bounds());

	vec mousePos = Mouse::GetPositionInWindow();
	veci tileMousePos = veci((mousePos.x / 32), (mousePos.y / 32));

#if _DEBUG
	static bool dbg_target_pos = false;
	ImGui::Begin("Cosas", NULL, ImGuiWindowFlags_AlwaysAutoResize);

	if (ImGui::Button("$500 PAVITOS"))
	{
		gdata.money += 500;
	}

	ImGui::Text("MousePos  %f, %f", mousePos.x, mousePos.y);
	ImGui::Text("MousePos (tiled) %d, %d", tileMousePos.x, tileMousePos.y);
	ImGui::Text("Passable %d", gdata.map_passable[tileMousePos.x][tileMousePos.y]);
	ImGui::Checkbox("show target pos", &dbg_target_pos);

	if (ImGui::CollapsingHeader("Minions"))
	{
		for (int i = 0; i < gdata.minions.size(); ++i)
		{
			if (ImGui::TreeNode(("Minion " + std::to_string(i)).c_str()))
			{

				ImGui::Text((("Pos: ") + std::to_string(gdata.minions[i]->pos.x) + ", " + std::to_string(gdata.minions[i]->pos.y)).c_str());
				ImGui::Text((("TargetPos: ") + std::to_string(gdata.minions[i]->pos_target.x) + ", " + std::to_string(gdata.minions[i]->pos_target.y)).c_str());

				ImGui::InputInt("Shape ", (int*)&gdata.minions[i]->gens.shape);

				ImGui::TreePop();
			}
		}
	}

	Window::SetShowCursor(ImGui::IsWindowHovered());

	ImGui::End();
#endif

	if (!gameStarted)
	{
		if (gameTime / 300 % 2)
		{
			Window::Draw(Assets::titleSpr, Window::GAME_WIDTH / 2, Window::GAME_HEIGHT / 2 - 35)
				.withOriginAtSpriteCenter()
				.withScale(2);
		}
		else
		{
			Window::Draw(Assets::titleSpr2, Window::GAME_WIDTH / 2, Window::GAME_HEIGHT / 2 - 35)
				.withOriginAtSpriteCenter()
				.withScale(2);
		}

		Window::Draw(Assets::creditsSpr, Window::GAME_WIDTH / 2 + 200, Window::GAME_HEIGHT / 2 + 200)
			.withOriginAtSpriteCenter()
			.withScale(1.5f);

		Window::Draw(Assets::spr, vec(mousePos.x - 6, mousePos.y - 3))
			.withRect(tr_cursor)
			.withScale(2);

		if (Mouse::IsJustPressed())
		{
			gameStarted = true;
			MusicPlayer::Ensure(Assets::music);

		}
		return;
	}

	//bool mineUnderMouse = (tileMousePos == minePos);
	int whichMineUnderMouse = -1;
	//Mine
	for (int i = 0; i < 6; ++i)
	{
		if (tileMousePos == minePos[i])
		{
			whichMineUnderMouse = i;
		}

		Window::Draw(Assets::spr, vec(minePos[i]) * 32)
			.withRect(tr_goldplace)
			.withScale(2);

		Window::Draw(Assets::spr, vec(32,0) + vec(minePos[i]) * 32)
			.withRect(tr_goldmine)
			.withScale(2);

		if (!unlockedMines[i])
		{
			Window::Draw(Assets::spr, vec(minePos[i]) * 32)
				.withRect(tr_candado)
				.withScale(2);

		}
	}


	//Shop
	bool foodShopUnderMouse = tileMousePos == shopPos + veci(0, 2);
	bool eggShopUnderMouse = tileMousePos == shopPos + veci(1, 2);
	bool keyShopUnderMouse = tileMousePos == shopPos + veci(2, 2);
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Window::Draw(Assets::spr, vec(shopPos.x + i, shopPos.y + j)*32)
				.withRect(GPU_Rect(i * 16, (5 + j) * 16, 16, 16))
				.withScale(2);
		}
	}

	Window::Draw(Assets::spr, vec(32 * (shopPos.x + 2), 32 * (shopPos.y + 2)))
		.withRect(tr_key)
		.withScale(2);

	//Love factory
	bool fucking = sexMinion1 && sexMinion2;
	GPU_Rect rect = tr_lofmxn;
	if (fucking)
	{
		rect = (Animation__GetSprite(AnimationType::LOVE_MACHINE, gameTime));
	}
	Window::Draw(Assets::spr, loveFactoryPos * 32)
		.withRect(rect)
		.withScale(2);


	bool loveFactoryUnderMouse = false;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			const veci loveFactoryPartPos(loveFactoryPos.x + i, loveFactoryPos.y + j);
			if (tileMousePos == loveFactoryPartPos)
			{
				loveFactoryUnderMouse = true;
			}
		}
	}


	const veci merchantPos(4, 16);
	bool merchantUnderMouse = false;
	int merchantAnimTime = merchantLeavingTime > 0 ? (5000 - merchantLeavingTime) : merchantTime;
	vec merchantAnimDisplacement(merchantAnimTime / 10.f + 15, sin(gameTime / 200) * 2 + 4);

	static std::vector<Coin> coins;
	if (timeMinionSelled > 0)
	{
		timeMinionSelled -= dt;

		int r = Rand::roll(0, 3);

		while (r--)
		{
			Coin coin;
			coin.t = Rand::roll(200, 500);
			coin.x = Rand::roll(120, 260);
			coin.x -= coin.x % 15;
			coin.y = 550;


			coins.push_back(coin);
		}
	}
	bool anyAlive = false;
	for (Coin& coin : coins)
	{
		if (!coin.alive)
		{
			continue;
		}

		anyAlive = true;

		coin.Update(dt);

		Window::Draw(Assets::spr, vec(coin.x - merchantAnimDisplacement.x, coin.y))
			.withRect(Animation__GetSprite(AnimationType::COINZOR, coin.t))
			.withScale(2);
	}
	if (!anyAlive)
	{
		coins.clear();
	}


	if (merchantLeavingTime > 0)
	{
		Window::Draw(Assets::spr, vec(merchantPos.x * 32 + 15, merchantPos.y * 32 + 24) - merchantAnimDisplacement)
			.withRect(Animation__GetSprite(Minion_AnimForShape(AnimationType::IDLE, merchantWantsMinion->gens.shape), 0, 10000))
			.withColor(merchantWantsMinion->gens.color)
			.withScale(2);
	}

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			if (merchantLeavingTime > 0)
			{
				//Sorry
				if ((merchantTime / 500) % 2)
				{
					rect = GPU_Rect((8 + i) * 16, (13 + j) * 16, 16, 16);
				}
				else
				{
					rect = GPU_Rect((12 + i) * 16, (13 + j) * 16, 16, 16);
				}
			}
			else if (merchantTime > 0)
			{
				rect = GPU_Rect((0 + i) * 16, (13 + j) * 16, 16, 16);
			}
			else
			{
				rect = GPU_Rect((4 + i) * 16, (13 + j) * 16, 16, 16);
			}
			const veci merchantPartPos(merchantPos.x + i, merchantPos.y + j);
			Window::Draw(Assets::spr, vec(merchantPartPos.x * 32, merchantPartPos.y * 32) - merchantAnimDisplacement)
				.withRect(rect)
				.withScale(2);

			if (merchantTime <= 0 && tileMousePos == merchantPartPos)
			{
				merchantUnderMouse = true;
			}
		}
	}
	if (merchantTime <= 0)
	{
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				const veci merchantPartPos(merchantPos.x + i + 2, merchantPos.y + j - 1);
				Window::Draw(Assets::spr, vec(merchantPartPos.x * 32 + 10, merchantPartPos.y * 32) - merchantAnimDisplacement)
					.withRect(GPU_Rect((4 + i) * 16, (2 + j) * 16, 16, 16))
					.withScale(2);
			}
		}
		//Draw the minion that the merchant wants
		const veci merchantPartPos(merchantPos.x + 3, merchantPos.y);
		Window::Draw(Assets::spr, vec(merchantPartPos.x * 32 + 10, merchantPartPos.y * 32) - merchantAnimDisplacement)
			.withRect(Animation__GetSprite(Minion_AnimForShape(AnimationType::IDLE, merchantWantsMinion->gens.shape), gameTime, ANIM_IDLE_SPEED))
			.withColor(merchantWantsMinion->gens.color)
			.withScale(2);
	}
	else
	{
		//if (gdata.minions.size() > 1)
		{
			merchantTime -= dt;
		}

		if (merchantTime <= 0)
		{
			merchantWantsMinion = new Minion(veci(0, 0));

			int roll_color;
			do
			{
				roll_color = Rand::roll(6);
			} while (!gdata.unlocked_colors[roll_color]);


			int roll_shapes;
			do
			{
				roll_shapes = Rand::roll((int)MinionShape::SHAPE_COUNT);
			} while (!gdata.unlocked_shapes[roll_shapes]);

			merchantWantsMinion->gens.shape = (MinionShape)roll_shapes;
			merchantWantsMinion->gens.color = Genetics_RandomColor(roll_color);

		}
	}
	if (merchantLeavingTime > 0)
	{
		merchantLeavingTime -= dt;
		if (merchantLeavingTime < 0)
		{
			merchantLeavingTime = 0;
			delete merchantWantsMinion;
			merchantWantsMinion = nullptr;
		}
	}



	Food* foodUnderMouse = nullptr;
	std::vector<Food*> fooderino;
	for (Food* f : gdata.food)
	{
		if (f == draggingFood)
		{
			fooderino.push_back(f);
			continue;
		}

		Window::Draw(Assets::spr, vec(f->pos) * 32)
			.withRect(tr_food)
			.withScale(2);

		if (tileMousePos == f->pos)
		{
			foodUnderMouse = f;
		}

		if (!f->eaten)
		{
			fooderino.push_back(f);
		}
		else
		{
			delete f;
		}
	}
	gdata.food = fooderino;

	Key* keyUnderMouse = nullptr;
	for (Key* k : gdata.keys)
	{
		if (k == draggingKey)
		{
			continue;
		}

		Window::Draw(Assets::spr, vec(k->pos) * 32)
			.withRect(tr_key)
			.withScale(2);

		if (tileMousePos == k->pos)
		{
			keyUnderMouse = k;
		}

	}

	Minion* minionUnderMouse = nullptr;
	for (Minion* m : gdata.minions)
	{
		if (m == draggingMinion || m == sexMinion1 || m == sexMinion2)
		{
			continue;
		}

		Minion_Update(m, &gdata, dt);

		//Draw Minion
		SDL_Color color;
		if (m->state == MinionState::EGG)
		{
			Window::Draw(Assets::spr, vec(m->pos.x * 32, m->pos.y * 32))
				.withRect(Animation__GetSprite(AnimationType::EGG_MOTAS, m->lifetime))
				.withColor(m->gens.color)
				.withScale(2);
			color = { 255,255,255,255 };
		}
		else {
			color = m->gens.color;
		}
		if (m->state == MinionState::ALIVE)
		{
			rect = Animation__GetSprite(Minion_AnimForShape(m->anim, m->gens.shape), m->lifetime, m->gens.walk_speed_delay * 2);
		}
		else
		{
			rect = Animation__GetSprite(Minion_AnimForShape(m->anim, m->gens.shape), m->lifetime, ANIM_IDLE_SPEED);
		}

		Window::Draw(Assets::spr, vec(m->pos.x * 32, m->pos.y * 32))
			.withRect(rect)
			.withColor(color)
			.withScale(2);

		if (m->state == MinionState::ALIVE && m->satiation < 3500)
		{
			if ((gameTime / 350) % 3) {
				Window::Draw(Assets::spr, vec(m->pos.x * 32, m->pos.y * 32) + vec(-16, -8))
					.withRect(tr_food);
			}
		}

		if (m->state == MinionState::EATING)
		{
			float dx = sin(m->lifetime * 2.76 / 150.0f) * 10;
			float dy = cos(m->lifetime / 175.0f) * 7;

			Window::Draw(Assets::spr, vec(m->pos.x * 32, m->pos.y * 32) + vec(dx, dy))
				.withRect(tr_food)
				.withScale(2);
		}

#if _DEBUG
		if (dbg_target_pos && m->state == MinionState::ALIVE)
		{
			Window::Draw(Assets::spr, vec(m->pos_target.x * 32, m->pos_target.y * 32))
				.withRect(tr_equits)
				.withScale(2);
		}
#endif
		if (tileMousePos == m->pos)
		{
			minionUnderMouse = m;
		}
	}


	//Cursor
	Window::Draw(Assets::spr, vec(tileMousePos)*32)
		.withRect(tr_selection)
		.withScale(2);

	if (sexMinion1)
	{
		sexMinion1->pos = veci(loveFactoryPos.x, loveFactoryPos.y + 1);
		Window::Draw(Assets::spr, vec(sexMinion1->pos.x * 32, sexMinion1->pos.y * 32))
			.withRect(Animation__GetSprite(Minion_AnimForShape(AnimationType::LOVE_MAKING, sexMinion1->gens.shape), gameTime, fucking ? ANIM_IDLE_SPEED / 2 : ANIM_IDLE_SPEED))
			.withColor(sexMinion1->gens.color)
			.withScale(2);
	}
	if (sexMinion2)
	{
		sexMinion2->pos = veci(loveFactoryPos.x + 2, loveFactoryPos.y + 1);
		Window::Draw(Assets::spr, vec(sexMinion2->pos.x * 32, sexMinion2->pos.y * 32))
			.withRect(Animation__GetSprite(Minion_AnimForShape(AnimationType::LOVE_MAKING, sexMinion2->gens.shape), gameTime, fucking ? ANIM_IDLE_SPEED / 2 : ANIM_IDLE_SPEED))
			.withColor(sexMinion2->gens.color)
			.withScale(2);
	}
	//for (Minion* miningMinion : miningMinions)
	for (int i = 0; i < 6; ++i)
	{
		Minion* miningMinion = miningMinions[i];

		if (!miningMinion) continue;

		if (miningMinion->state == MinionState::MINING)
		{
			miningTime[i] += dt * miningMinion->gens.mining_multiplier;;
			if (miningTime[i] > TIME_MINNING_FOR_REWARD)
			{
				miningTime[i] -= TIME_MINNING_FOR_REWARD;
				gdata.money += MINNING_REWARD;
				miningEffectTime[i] = 100;
			}

			//Draw martillo
			Window::Draw(Assets::spr, vec(minePos[i].x * 32 + 32 - 9, minePos[i].y * 32 + 20))
				.withRect(tr_pick)
				.withRotationDegs((miningTime[i] / ((float)TIME_MINNING_FOR_REWARD)) * 90)
				.withOrigin(8, 16)
				.withScale(2);
		}

		if (miningEffectTime[i] > 0)
		{
			miningEffectTime[i] -= dt;
			Window::Draw(Assets::spr, vec(minePos[i].x * 32 + 32, minePos[i].y * 32))
				.withRect(Animation__GetSprite(AnimationType::PICK_EFFECT, 100 - miningEffectTime[i]))
				.withRotationDegs((miningTime[i] / ((float)TIME_MINNING_FOR_REWARD)) * 90)
				.withScale(2);
			if (miningEffectTime[i] < 0) miningCoinEffectTime[i] = 160;
		}
		if (miningCoinEffectTime[i] > 0)
		{
			miningCoinEffectTime[i] -= dt;
			Window::Draw(Assets::spr, vec(minePos[i].x * 32 + 32, minePos[i].y * 32 - 16 - 24 * ((100 - miningCoinEffectTime[i]) / 100.0f)))
				.withRect(Animation__GetSprite(AnimationType::COINZOR, 100 - miningCoinEffectTime[i]))
				.withRotationDegs((miningTime[i] / ((float)TIME_MINNING_FOR_REWARD)) * 90)
				.withScale(2);
		}

	}

	bool justGrabbed = false;
	//for (Minion* miningMinion : miningMinions)
	for (int i = 0; i < 6; ++i)
	{
		Minion* miningMinion = miningMinions[i];

		if (!miningMinion) continue;

		if (whichMineUnderMouse == i && !draggingFood && !draggingMinion && !draggingKey)
		{
			DrawMinionStats(miningMinion);
			if (Mouse::IsJustPressed())
			{
				justGrabbed = true;
				draggingMinion = miningMinion;
				draggingMinion->state = MinionState::ALIVE;
				miningMinions[i] = nullptr;
			}
		}
	}
	if (draggingMinion && !justGrabbed)
	{
		vec pos = vec(mousePos) + vec(16, -24);
		vec origin = vec(8, -16);
		float rotation = (sin(gameTime / 220.f) * 6);
		SDL_Color color;
		if (draggingMinion->state == MinionState::EGG)
		{
			Window::Draw(Assets::spr, pos)
				.withRect(Animation__GetSprite(AnimationType::EGG_MOTAS, 0))
				.withRotationDegs(rotation)
				.withOrigin(origin)
				.withColor(draggingMinion->gens.color)
				.withScale(2);
			color = { 255, 255, 255, 255 };
		}
		else {
			color = draggingMinion->gens.color;
		}
		Window::Draw(Assets::spr, pos)
			.withRect(Animation__GetSprite(Minion_AnimForShape(draggingMinion->anim, draggingMinion->gens.shape), 0, ANIM_IDLE_SPEED))
			.withRotationDegs(rotation)
			.withOrigin(origin)
			.withColor(color)
			.withScale(2);

		draggingMinion->pos = tileMousePos;
		DrawMinionStats(draggingMinion);

		if (merchantUnderMouse)
		{
			if (Mouse::IsJustPressed() && draggingMinion->state == MinionState::ALIVE)
			{
				if (merchantWantsMinion->gens.color == draggingMinion->gens.color
					&&
					merchantWantsMinion->gens.shape == draggingMinion->gens.shape)
				{
					merchantLeavingTime = COOLDOWN_MERCHANT / 2;
					merchantTime = COOLDOWN_MERCHANT;
					gdata.minions.erase(std::remove(gdata.minions.begin(), gdata.minions.end(), draggingMinion), gdata.minions.end());
					delete draggingMinion;
					draggingMinion = nullptr;
					gdata.money += SLAVERY_REWARD;

					timeMinionSelled = 1000;

				}
			}
		}
		else if (loveFactoryUnderMouse)
		{
			if (Mouse::IsJustPressed() && draggingMinion->state == MinionState::ALIVE)
			{
				if (sexMinion1 == nullptr)
				{
					sexMinion1 = draggingMinion;
					draggingMinion = nullptr;
				}
				else if (sexMinion2 == nullptr)
				{
					sexMinion2 = draggingMinion;
					draggingMinion = nullptr;
					sexTime = LOVE_MAKING_TIME;
				}
			}
		}
		else if (whichMineUnderMouse != -1)
		{
			if (Mouse::IsJustPressed() && draggingMinion->state == MinionState::ALIVE)
			{
				if (miningMinions[whichMineUnderMouse] == nullptr && unlockedMines[whichMineUnderMouse])
				{
					miningTime[whichMineUnderMouse] = 0;
					miningMinions[whichMineUnderMouse] = draggingMinion;
					draggingMinion->state = MinionState::MINING;
					draggingMinion = nullptr;
				}
			}
		}
		else if (gdata.map_passable[tileMousePos.x][tileMousePos.y] && !foodUnderMouse && !minionUnderMouse && !keyUnderMouse)
		{
			if (Mouse::IsJustPressed())
			{
				draggingMinion->pos = tileMousePos;
				gdata.map_passable[tileMousePos.x][tileMousePos.y] = false;
				draggingMinion = nullptr;
			}
		}

	}
	else if (draggingFood)
	{
		vec pos = vec(mousePos) + vec(16, -32);
		vec origin = vec(8, -16);
		float rotation = (sin(gameTime / 250.f) * 8);
		Window::Draw(Assets::spr, pos)
			.withRect(tr_food)
			.withOrigin(origin)
			.withRotationDegs(rotation)
			.withScale(2);

		if ((gdata.map_passable[tileMousePos.x][tileMousePos.y] || minionUnderMouse) && !foodUnderMouse && !keyUnderMouse && whichMineUnderMouse == -1)
		{
			if (Mouse::IsJustPressed())
			{
				draggingFood->pos = tileMousePos;
				draggingFood = nullptr;
			}
		}
	}
	else if (draggingKey)
	{
		Window::Draw(Assets::spr, vec(mousePos))
			.withRect(tr_key)
			.withScale(2);

		if (Mouse::IsJustPressed())
		{
			if (whichMineUnderMouse != -1 && !unlockedMines[whichMineUnderMouse])
			{
				unlockedMines[whichMineUnderMouse] = true;
				gdata.keys.erase(std::remove(gdata.keys.begin(), gdata.keys.end(), draggingKey), gdata.keys.end());
				delete draggingKey;
				draggingKey = nullptr;

			}
			else if (gdata.map_passable[tileMousePos.x][tileMousePos.y] && !keyUnderMouse && !minionUnderMouse && !foodUnderMouse && whichMineUnderMouse == -1)
			{
				draggingKey->pos = tileMousePos;
				draggingKey = nullptr;
			}
		}
	}
	else
	{
		if (minionUnderMouse)
		{
			if (Mouse::IsJustPressed())
			{
				draggingMinion = minionUnderMouse;
				gdata.map_passable[minionUnderMouse->pos.x][minionUnderMouse->pos.y] = true;
			}
			else
			{
				DrawMinionStats(minionUnderMouse);
			}
		}
		else if (foodUnderMouse)
		{
			if (Mouse::IsJustPressed())
			{
				draggingFood = foodUnderMouse;
			}
		}
		else if (keyUnderMouse)
		{
			if (Mouse::IsJustPressed())
			{
				draggingKey = keyUnderMouse;
			}
		}
		else if (foodShopUnderMouse)
		{
			ImGui::SetNextWindowPosInGameCoords(vec(tileMousePos.x * 32, tileMousePos.y * 32 + 40));
			ImGui::Begin("moneys food", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
			static string price_food_tag = "FOOD - $" + to_string(PRICE_FOOD);
			ImGui::Text(price_food_tag.c_str());
			ImGui::End();

			if (Mouse::IsJustPressed())
			{
				if (gdata.money >= PRICE_FOOD)
				{
					gdata.money -= PRICE_FOOD;
					draggingFood = new Food(tileMousePos);
					gdata.food.push_back(draggingFood);
				}
			}
		}
		else if (eggShopUnderMouse)
		{
			static string price_egg_tag;
			price_egg_tag = "EGG - $" + to_string(PRICE_EGG + gdata.minions_purchased * PRICE_EGG_INCREMENT);
			ImGui::SetNextWindowPosInGameCoords(vec(tileMousePos.x * 32, tileMousePos.y * 32 + 40));
			ImGui::Begin(price_egg_tag.c_str(), 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text(price_egg_tag.c_str());
			ImGui::End();

			if (Mouse::IsJustPressed())
			{
				if (gdata.money >= PRICE_EGG + gdata.minions_purchased * PRICE_EGG_INCREMENT)
				{
					gdata.money -= PRICE_EGG + gdata.minions_purchased * PRICE_EGG_INCREMENT;
					draggingMinion = new Minion(tileMousePos);

					Merchant_Unlock(&gdata, draggingMinion);

					gdata.minions.push_back(draggingMinion);
					gdata.minions_purchased++;
				}
			}
		}
		else if (keyShopUnderMouse) {
			static string price_egg_tag;
			price_egg_tag = "KEY - $" + to_string(PRICE_KEY + gdata.keys_purchased * PRICE_KEY_INCREMENT);
			ImGui::SetNextWindowPosInGameCoords(vec(tileMousePos.x * 32, tileMousePos.y * 32 + 40));
			ImGui::Begin(price_egg_tag.c_str(), 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize);
			ImGui::Text(price_egg_tag.c_str());
			ImGui::End();

			if (Mouse::IsJustPressed())
			{
				if (gdata.money >= PRICE_KEY + gdata.keys_purchased * PRICE_KEY_INCREMENT)
				{
					gdata.money -= PRICE_KEY + gdata.keys_purchased * PRICE_KEY_INCREMENT;
					draggingKey = new Key(veci(0, 0));

					gdata.keys.push_back(draggingKey);
					gdata.keys_purchased++;
				}
			}
		}
	}

	if (sexTime > 0)
	{
		sexTime -= dt;
		if (sexTime <= 0)
		{
			Minion* minion = new Minion(loveFactoryPos + veci(1, 2));
			minion->generateGenetics(sexMinion1, sexMinion2);

			sexMinion1 = nullptr;
			sexMinion2 = nullptr;

			gdata.minions.push_back(minion);
		}
	}

	//ImGui::ShowDemoWindow();

	//Draw hud coin
	Window::Draw(Assets::spr, 12, 13)
		.withRect(tr_coin)
		.withScale(2);
	
	{
		Text txt_money(Assets::font_30, Assets::font_30_outline_2);
		string str = "";
		int mm = gdata.money;
		for (int i = 0; i < 5; ++i)
		{
			str += std::to_string(mm % 10);
			mm = mm / 10;
		}
		std::reverse(str.begin(), str.end());

		txt_money.SetString(str);
		txt_money.SetFillColor({ 255,255,0,0 });
		txt_money.SetOutlineColor({0,0,0,0});

		Window::Draw(txt_money, 12 + 32 + 8, 0)
			.withOrigin(0, -txt_money.Size().y/2);
	}

	//Draw cursor
	if (draggingFood || draggingMinion || draggingKey)
	{
		Window::Draw(Assets::spr, vec(mousePos.x - 1, mousePos.y - 15))
			.withRect(tr_cursor_grab)
			.withScale(2);
	}
	else
	{
		Window::Draw(Assets::spr, vec(mousePos.x - 6, mousePos.y - 3))
			.withRect(tr_cursor)
			.withScale(2);
	}
}
