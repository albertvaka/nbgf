#include "Skilltree.h"

#include <vector>
#include "assets.h"
#include "input.h"
#include "debug.h"
#include "imgui.h"

std::vector<bool> unlocked;

 std::vector< std::vector<Skill>> needs = {
	{},
	{},
	{},
	{},
	{Skill::BREAK},
	{Skill::BREAK},
	{Skill::BOUNCY},
	{Skill::BOUNCY, Skill::RANGE_1},
	{Skill::RANGE_1},
};

 std::vector<const char*> description = {
	"",
	"",
	"",
	"Break blocks\n\n\nShot at transparent blocks to break them.",
	"Bouncy shots\n\n\nShots bounce against walls once.",
	"Range I\n\n\nIncrease the range of your weapon.",
	"Rapid fire\n\n\nShot twice as fast.",
	"Damage I\n\n\nBeat up stronger foes.",
	"Range II\n\n\nIncrease the range of your weapon.",
 };

 std::vector<GPU_Rect> img = {
	{},
	{},
	{},
	{8*16,11*16,16,16},
	{10*16,11*16,16,16},
	{5*16,10*16,16,16},
	{7*16,11*16,16,16},
	{9*16,11*16,16,16},
	{6*16,10*16,16,16},
};

 std::vector< std::vector<Skill>> tree = {
	{Skill::NO,    Skill::NO,      Skill::RAPID_FIRE},
	{Skill::NO,    Skill::BOUNCY,  Skill::NO},
	{Skill::BREAK, Skill::NO,      Skill::DMG_1},
	{Skill::NO,    Skill::RANGE_1, Skill::NO},
	{Skill::NO,    Skill::NO,      Skill::RANGE_2},
};

veci PosInTree(Skill s) {
	for (int y = 0; y < tree.size(); y++) {
		for (int x = 0; x < tree[y].size(); x++) {
			if (tree[y][x] == s) {
				return { x,y };
			}
		}
	}
	Debug::out << "Skill not found";
	return { 0,0 };

}

vec PosInTreeToPosInScreen(veci p) {
	auto size = Camera::GetSize();
	vec dxv = vec(size.x / (tree[0].size()), (size.y / (tree.size())));
	return Camera::GetBounds().TopLeft() + (vec(p) * (dxv / 2)) + vec(dxv.x/2, dxv.y);
}

SkillTree::SkillTree()
	: textPoints(Assets::font_30)
	, textPressStart(Assets::font_30)
	, textDescription(Assets::font_30)
	, current(PosInTree(Skill::BREAK))
{
	enabled.resize(description.size(), false);
	textPressStart.setString("Press Start to assign points");
};

void SkillTree::Update(float dt) {
	if (Input::IsJustPressed(0,GameKeys::START)) {
		open = !open;
	}
	if (!open) {
		return;
	}

	if (Input::IsJustPressed(0,GameKeys::LEFT)) {
		if (current.x > 0) {
			if (prev_left != -1) {
				prev_right = current.y;
				current.y = prev_left;
				current.x--;
				prev_left = -1;
				goto found;
			}
			int current_Skill = int(tree[current.y][current.x]);
			std::vector<Skill>& n = needs[current_Skill];
			if (!n.empty()) {
				prev_right = current.y;
				current = PosInTree(n[0]); //current_Skill needs n[0]
			}
		}
	}
	if (Input::IsJustPressed(0,GameKeys::RIGHT)) {
		if (current.x < tree[current.y].size()) {
			if (prev_right != -1) {
				prev_left = current.y;
				current.y = prev_right;
				current.x++;
				prev_right = -1;
				goto found;
			}
			Skill current_Skill = tree[current.y][current.x];
			for (int s = 0; s < needs.size(); s++) {
				for (int n = 0; n < needs[s].size(); n++) {
					if (needs[s][n] == current_Skill) { //Skill s needs current_Skill
						prev_left = current.y;
						current = PosInTree(Skill(s));
						goto found;
					}
				}
			}
		}
	}
found:
	if (Input::IsJustPressed(0,GameKeys::UP)) {
		if (current.y > 0) {
			prev_left = -1;
			prev_right = -1;
			do {
				current.y--;
				if (current.y == 0) {
					if (current.x < tree[current.y].size() - 1) current.x++;
					while (tree[current.y][current.x] == Skill::NO) {
						current.y++;
					} 
					break;
				}
			} while (tree[current.y][current.x] == Skill::NO);
		}
	}
	if (Input::IsJustPressed(0,GameKeys::DOWN)) {
		if (current.y < tree.size() - 1) {
			prev_left = -1;
			prev_right = -1;
			do {
				current.y++;
				if (current.y == tree.size() - 1) {
					if (current.x < tree[current.y].size()-1) current.x++;
					while (tree[current.y][current.x] == Skill::NO) {
						current.y--;
					}
					break;
				}
			} while (tree[current.y][current.x] == Skill::NO);
		}
	}

	int skill = int(tree[current.y][current.x]);

	if (Input::IsJustPressed(0,GameKeys::ACTIVATE) && !enabled[skill]) {
		if (gunpoints > 0) {
			bool requirementsMet = true;
			for (Skill n : needs[skill]) {
				if (!enabled[int(n)]) {
					requirements_not_met_skill = skill;
					requirements_not_met_timer = 0.8f;
					requirementsMet = false;
					break;
				}
			}
			if (requirementsMet) {
				enabled[skill] = true;
				gunpoints--;
			}
		}
		else {
			not_enough_points_timer = 0.8f;
		}
	}

	if (requirements_not_met_timer > 0) {
		requirements_not_met_timer -= dt;
	}
	if (not_enough_points_timer > 0) {
		not_enough_points_timer -= dt;
	}

	textPoints.setString("Points to assign: " + std::to_string(gunpoints));
	textDescription.setString(description[skill]);
}
void SkillTree::DrawOverlay() {
	if (gunpoints > 0) {
		Window::Draw(textPressStart, Camera::GetBounds().TopLeft() + vec(10, 10)).withScale(0.3f);
	}
}


void SkillTree::DrawMenu() {
	//Assets::menuBgSprxte.setScale(1.f / GameData::GAME_ZOOM, 1.f / GameData::GAME_ZOOM);

	Window::Draw(Assets::menuBgTexture, Camera::GetBounds().TopLeft());

	ImGui::Begin("SkillTree");
	if (ImGui::Button("add point")) {
		gunpoints++;
	};
	ImGui::End();

	for (int s = 0; s < needs.size(); s++) {
		if (!needs[s].empty()) {
			vec from = PosInTreeToPosInScreen(PosInTree(Skill(s)));
			for (int n = 0; n < needs[s].size(); n++) {
				vec to = PosInTreeToPosInScreen(PosInTree(Skill(needs[s][n])));
				Window::DrawPrimitive::Line(from, to, 1, 255, 255, 255);
			}
		}
	}

	for (int y = 0; y < tree.size(); y++) {
		for (int x = 0; x < tree[y].size(); x++) {
			int skill = int(tree[y][x]);
			if (skill != int(Skill::NO)) {
				vec pos = PosInTreeToPosInScreen({ x,y });
				GPU_Rect rect = img[skill];
				SDL_Color color = { 100, 100, 250, 255 }; 
				if (enabled[skill]) {
					color = { 250, 100, 100, 255 };
				}
				Window::Draw(Assets::marioTexture, pos)
					.withRect(rect)
					.withOrigin(8,8)
					.withColor(color);
			}
		}
	}

	vec currentPos = PosInTreeToPosInScreen(current);
	Bounds::fromCenter(currentPos, vec(16, 16)).Draw(255,255,255);

	if (requirements_not_met_timer > 0) {
		if (int(requirements_not_met_timer * 10.f) % 2) {
			for (Skill n : needs[requirements_not_met_skill]) {
				if (!enabled[int(n)]) {
					vec pos = PosInTreeToPosInScreen(PosInTree(n));
					Bounds::fromCenter(pos, vec(16, 16)).Draw(255,0,0);
				}
			}
		}
	}

	if (not_enough_points_timer > 0.f && int(not_enough_points_timer * 10.f) % 2) {
		textPoints.setFillColor(255, 0, 0);
	} else {
		textPoints.setFillColor(255, 255, 255);
	}

	Window::Draw(textPoints, Camera::GetBounds().TopLeft() + vec(10, 10)).withScale(0.3f);
	Window::Draw(textDescription, Camera::GetBounds().TopLeft() + vec(50, 250)).withScale(0.3f);

}
