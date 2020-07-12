#include "skilltree.h"

#include <vector>
#include "assets.h"
#include "camera.h"
#include "input.h"
#include "magic_enum.h"
#include "debug.h"

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

 std::vector<Bounds> skillBounds; // bounds in screen of each skill icon

veci PosInTree(Skill s) {
	for (int y = 0; y < int(tree.size()); y++) {
		for (int x = 0; x < int(tree[y].size()); x++) {
			if (tree[y][x] == s) {
				return { x,y };
			}
		}
	}
	return { -1,-1 };

}

vec PosInTreeToPosInScreen(veci p) {
	auto size = Camera::GUI::GetSize();
	vec dxv = vec(size.x / (tree[0].size()), (size.y / (tree.size())));
	return vec(p.x * dxv.x / 2, p.y * dxv.y /2) + vec(dxv.x/2, dxv.y);
}

SkillTree::SkillTree()
	: textPoints(Assets::font_30)
	, textPressStart(Assets::font_30)
	, textDescription(Assets::font_30)
	, current(PosInTree(Skill::BREAK))
{
	for (Skill s : magic_enum::enum_values<Skill>()) {
		vec pos = PosInTreeToPosInScreen(PosInTree(s));
		skillBounds.push_back(Bounds(pos-vec(8,8), vec(16,16)));
	}

	enabled.resize(description.size(), false);
	textPressStart.SetString("Press Start to assign points");
};

Skill MouseSelectedSkill() {
	vec pos = Mouse::GetPositionInWindow();
	int i = 0;
	for (const Bounds& b : skillBounds) {
		if (b.Contains(pos)) {
			return Skill(i);
		}
		i++;
	}
	return Skill::NO;
}

void SkillTree::Update(float dt) {
	if (Input::IsJustPressed(0,GameKeys::START)) {
		open = !open;
	}
	if (!open) {
		return;
	}

	bool clickedOnSkill = false;
	vec delta = Mouse::GetDeltaMovement();
	if (Mouse::IsJustPressed() || fabs(delta.x) > 0.f || fabs(delta.y) > 0.f) {
		Skill skill = MouseSelectedSkill();
		if (skill != Skill::NO) {
			veci posi = PosInTree(skill);
			prev_left = -1;
			prev_right = -1;
			current = posi;
			if (Mouse::IsJustPressed()) {
				clickedOnSkill = true;
			}
		}
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
		if (current.x < int(tree[current.y].size())) {
			if (prev_right != -1) {
				prev_left = current.y;
				current.y = prev_right;
				current.x++;
				prev_right = -1;
				goto found;
			}
			Skill current_Skill = tree[current.y][current.x];
			for (size_t s = 0; s < needs.size(); s++) {
				for (size_t n = 0; n < needs[s].size(); n++) {
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
					if (current.x < int(tree[current.y].size()) - 1) current.x++;
					while (tree[current.y][current.x] == Skill::NO) {
						current.y++;
					} 
					break;
				}
			} while (tree[current.y][current.x] == Skill::NO);
		}
	}
	if (Input::IsJustPressed(0,GameKeys::DOWN)) {
		if (current.y < int(tree.size()) - 1) {
			prev_left = -1;
			prev_right = -1;
			do {
				current.y++;
				if (current.y == int(tree.size()) - 1) {
					if (current.x < int(tree[current.y].size()) - 1) current.x++;
					while (tree[current.y][current.x] == Skill::NO) {
						current.y--;
					}
					break;
				}
			} while (tree[current.y][current.x] == Skill::NO);
		}
	}

	int skill = int(tree[current.y][current.x]);

	if ((Input::IsJustPressed(0,GameKeys::ACTION) || clickedOnSkill) && !enabled[skill]) {
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

	textPoints.SetString("Points to assign: " + std::to_string(gunpoints));
	textDescription.SetString(description[skill]);
}
void SkillTree::DrawOverlay() {
	if (gunpoints > 0) {
		Window::Draw(textPressStart, Camera::GetBounds().TopLeft() + vec(10, 10)).withScale(0.3f);
	}
}


void SkillTree::DrawMenu() {

	Camera::GUI::Begin();

	Window::Draw(Assets::menuBgTexture, vec(0,0))
		.withScale(float(Window::GAME_WIDTH)/Assets::menuBgTexture->w, float(Window::GAME_HEIGHT)/Assets::menuBgTexture->h);

	for (size_t s = 0; s < needs.size(); s++) {
		if (!needs[s].empty()) {
			vec from = PosInTreeToPosInScreen(PosInTree(Skill(s)));
			for (size_t n = 0; n < needs[s].size(); n++) {
				vec to = PosInTreeToPosInScreen(PosInTree(Skill(needs[s][n])));
				Window::DrawPrimitive::Line(from, to, 1, 255, 255, 255);
			}
		}
	}

	for (int y = 0; y < int(tree.size()); y++) {
		for (int x = 0; x < int(tree[y].size()); x++) {
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
		textPoints.SetFillColor(255, 0, 0);
	} else {
		textPoints.SetFillColor(255, 255, 255);
	}

	Window::Draw(textPoints, vec(10, 10)).withScale(0.3f);
	Window::Draw(textDescription, vec(50, 250)).withScale(0.3f);

	Camera::GUI::End();

}
