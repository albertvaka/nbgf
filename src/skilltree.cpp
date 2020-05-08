#include "skilltree.h"

#include <vector>
#include "assets.h"
#include "input.h"
#include "debug.h"
#include "imgui.h"

std::vector<bool> unlocked;

 std::vector< std::vector<SkillTree::Skill>> needs = {
	{},
	{SkillTree::BREAK},
	{SkillTree::BREAK},
	{SkillTree::BOUNCY},
	{SkillTree::BOUNCY, SkillTree::RANGE_1},
	{SkillTree::RANGE_1},
};

 std::vector<const char*> description = {
	"Break blocks\n\n\nShot at transparent blocks to break them.",
	"Bouncy shots\n\n\nShots bounce against walls once.",
	"Range I\n\n\nIncrease the range of your weapon.",
	"Rapid fire\n\n\nShot twice as fast.",
	"Damage I\n\n\nBeat up stronger foes.",
	"Range II\n\n\nIncrease the range of your weapon.",
 };

 std::vector<GPU_Rect> img = {
	{8*16,11*16,16,16},
	{10*16,11*16,16,16},
	{5*16,10*16,16,16},
	{7*16,11*16,16,16},
	{9*16,11*16,16,16},
	{6*16,10*16,16,16},
};

 std::vector< std::vector<int>> tree = {
	{-1,               -1,                 SkillTree::RAPID_FIRE},
	{-1,               SkillTree::BOUNCY,  -1},
	{SkillTree::BREAK, -1,                 SkillTree::DMG_1},
	{-1,               SkillTree::RANGE_1, -1},
	{-1,               -1,                 SkillTree::RANGE_2},
};

SkillTree::TreePos posInTree(SkillTree::Skill s) {
	for (int j = 0; j < tree.size(); j++) {
		for (int i = 0; i < tree[j].size(); i++) {
			if (tree[j][i] == s) {
				return { i,j };
			}
		}
	}
	Debug::out << "Skill not found";
	return { 0,0 };

}

vec posInTreeToPosInScreen(SkillTree::TreePos p) {
	auto size = Camera::GetSize();
	vec div = vec(size.x / (tree[0].size()), (size.y / (tree.size())));
	return Camera::GetBounds().TopLeft() + (vec(p.i, p.j) * (div / 2)) + vec(div.x/2, div.y);
}

SkillTree::SkillTree()
	: textPoints(Assets::font_30)
	, textPressStart(Assets::font_30)
	, textDescription(Assets::font_30)
{
	enabled.resize(description.size(), false);

	current = posInTree(SkillTree::BREAK);
	textPressStart.setString("Press start to assign points");
};

void SkillTree::Update(float dt) {
	if (Input::IsJustPressed(0,GameKeys::START)) {
		open = !open;
	}
	if (!open) {
		return;
	}

	if (Input::IsJustPressed(0,GameKeys::LEFT)) {
		if (current.i > 0) {
			if (prev_left != -1) {
				prev_right = current.j;
				current.j = prev_left;
				current.i--;
				prev_left = -1;
				goto found;
			}
			int current_skill = tree[current.j][current.i];
			std::vector<SkillTree::Skill>& n = needs[current_skill];
			if (!n.empty()) {
				prev_right = current.j;
				current = posInTree(n[0]); //current_skill needs n[0]
			}
		}
	}
	if (Input::IsJustPressed(0,GameKeys::RIGHT)) {
		if (current.i < tree[current.j].size()) {
			if (prev_right != -1) {
				prev_left = current.j;
				current.j = prev_right;
				current.i++;
				prev_right = -1;
				goto found;
			}
			int current_skill = tree[current.j][current.i];
			for (int s = 0; s < needs.size(); s++) {
				for (int n = 0; n < needs[s].size(); n++) {
					if (needs[s][n] == current_skill) { //skill s needs current_skill
						prev_left = current.j;
						current = posInTree(Skill(s));
						goto found;
					}
				}
			}
		}
	}
found:
	if (Input::IsJustPressed(0,GameKeys::UP)) {
		if (current.j > 0) {
			prev_left = -1;
			prev_right = -1;
			do {
				current.j--;
				if (current.j == 0) {
					if (current.i < tree[current.j].size() - 1) current.i++;
					while (tree[current.j][current.i] == -1) {
						current.j++;
					} 
					break;
				}
			} while (tree[current.j][current.i] == -1);
		}
	}
	if (Input::IsJustPressed(0,GameKeys::DOWN)) {
		if (current.j < tree.size() - 1) {
			prev_left = -1;
			prev_right = -1;
			do {
				current.j++;
				if (current.j == tree.size() - 1) {
					if (current.i < tree[current.j].size()-1) current.i++;
					while (tree[current.j][current.i] == -1) {
						current.j--;
					}
					break;
				}
			} while (tree[current.j][current.i] == -1);
		}
	}

	int skill = tree[current.j][current.i];

	if (Input::IsJustPressed(0,GameKeys::ACTIVATE) && !enabled[skill]) {
		if (gunpoints > 0) {
			bool requirementsMet = true;
			for (int n : needs[skill]) {
				if (!enabled[n]) {
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
	//Assets::menuBgSprite.setScale(1.f / GameData::GAME_ZOOM, 1.f / GameData::GAME_ZOOM);

	Window::Draw(Assets::menuBgTexture, Camera::GetBounds().TopLeft());

	ImGui::Begin("SkillTree");
	if (ImGui::Button("add point")) {
		gunpoints++;
	};
	ImGui::End();

	for (int s = 0; s < needs.size(); s++) {
		if (!needs[s].empty()) {
			vec from = posInTreeToPosInScreen(posInTree(Skill(s)));
			for (int n = 0; n < needs[s].size(); n++) {
				vec to = posInTreeToPosInScreen(posInTree(Skill(needs[s][n])));
				Window::DrawPrimitive::Line(from, to, 1, 255, 255, 255);
			}
		}
	}

	for (int j = 0; j < tree.size(); j++) {
		for (int i = 0; i < tree[j].size(); i++) {
			if (tree[j][i] != -1) {
				vec pos = posInTreeToPosInScreen({ i,j });
				SDL_Color color = { 100, 100, 250, 255 }; 
				if (enabled[tree[j][i]]) {
					color = { 250, 100, 100, 255 };
				}
				GPU_Rect rect = img[tree[j][i]];

				Window::Draw(Assets::marioTexture, pos)
					.withRect(rect)
					.withOrigin(8,8)
					.withColor(color);
			}
		}
	}

	vec currentPos = posInTreeToPosInScreen(current);
	Bounds::fromCenter(currentPos, vec(16, 16)).Draw(255,255,255);

	if (requirements_not_met_timer > 0) {
		if (int(requirements_not_met_timer * 10.f) % 2) {
			for (int n : needs[requirements_not_met_skill]) {
				if (!enabled[n]) {
					vec pos = posInTreeToPosInScreen(posInTree(Skill(n)));
					Bounds::fromCenter(pos, vec(16, 16)).Draw(255,0,0);
				}
			}
		}
	}

	textPoints.setFillColor(255, 255, 255);
	if (not_enough_points_timer > 0.f) {
		if (int(not_enough_points_timer * 10.f) % 2) {
			textPoints.setFillColor(255, 0, 0);
		}
	}

	Window::Draw(textPoints, Camera::GetBounds().TopLeft() + vec(10, 10)).withScale(0.3f);
	Window::Draw(textDescription, Camera::GetBounds().TopLeft() + vec(50, 250)).withScale(0.3f);

}
