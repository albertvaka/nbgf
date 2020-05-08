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
	"Break blocks\n\nBreak some blocks with your gun.",
	"Bouncy shots\n\nShots can bounce against walls once.",
	"Rapid fire\n\nShot twice as fast.",
	"Range+\n\nIncreases the range of your weapon.",
	"Damage+\n\nDamage foes with stronger defenses.",
	"Range++\n\nIncreases the range of your weapon.",
 };

 std::vector< sf::IntRect> img = {
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
	auto size = Camera::GetCameraSize();
	vec div = vec(size.x / (tree[0].size()), (size.y / (tree.size())));
	return Camera::GetCameraBounds().TopLeft() + (vec(p.i, p.j) * (div / 2)) + vec(div.x/2, div.y);
}

SkillTree::SkillTree()
{
	enabled.resize(description.size(), false);

	textPoints.setFont(Assets::font);
	textPressStart.setFont(Assets::font);
	textDescription.setFont(Assets::font);

	const float scale = 0.3f;
	textPoints.setScale(scale, scale);
	textPressStart.setScale(scale, scale);
	textDescription.setScale(scale, scale);

	current = posInTree(SkillTree::BREAK);
	textPressStart.setString("Press start to assign points");
};

void SkillTree::Update(float dt) {
	if (Keyboard::IsKeyJustPressed(GameKeys::START)) {
		open = !open;
	}
	if (!open) {
		return;
	}

	if (Keyboard::IsKeyJustPressed(GameKeys::LEFT)) {
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
	if (Keyboard::IsKeyJustPressed(GameKeys::RIGHT)) {
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
	if (Keyboard::IsKeyJustPressed(GameKeys::UP)) {
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
	if (Keyboard::IsKeyJustPressed(GameKeys::DOWN)) {
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

	if (Keyboard::IsKeyJustPressed(GameKeys::ACTION) && !enabled[skill]) {
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
void SkillTree::DrawOverlay(sf::RenderTarget& window) {
	sf::Sprite& sprite = Assets::marioSprite;
	if (gunpoints > 0) {
		textPressStart.setPosition(Camera::GetCameraBounds().TopLeft() + vec(10, 10));
		window.draw(textPressStart);
	}
}


void SkillTree::DrawMenu(sf::RenderTarget& window) {
	//Assets::menuBgSprite.setScale(1.f / GameData::GAME_ZOOM, 1.f / GameData::GAME_ZOOM);

	sf::Sprite& sprite = Assets::marioSprite;

	Assets::menuBgSprite.setPosition(Camera::GetCameraBounds().TopLeft());
	window.draw(Assets::menuBgSprite);
	
	sf::VertexArray lines(sf::Lines);

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
				lines.append(from);
				lines.append(to);
			}
		}
	}
	window.draw(lines);

	sprite.setOrigin(8, 8);
	for (int j = 0; j < tree.size(); j++) {
		for (int i = 0; i < tree[j].size(); i++) {
			if (tree[j][i] != -1) {
				sprite.setPosition(posInTreeToPosInScreen({ i,j }));
				sprite.setTextureRect(img[tree[j][i]]);

				bool canpick = true;
				
				if (enabled[tree[j][i]]) {
					sprite.setColor(sf::Color(250, 100, 100, 255));
				} else {
					sprite.setColor(sf::Color(100, 100, 250, 255));
				}
				window.draw(sprite);
			}
		}
	}

	vec currentPos = posInTreeToPosInScreen(current);
	Bounds::fromCenter(currentPos, vec(16, 16)).Draw(window, sf::Color::White);

	if (requirements_not_met_timer > 0) {
		if (int(requirements_not_met_timer * 10.f) % 2) {
			for (int n : needs[requirements_not_met_skill]) {
				if (!enabled[n]) {
					vec pos = posInTreeToPosInScreen(posInTree(Skill(n)));
					Bounds::fromCenter(pos, vec(16, 16)).Draw(window, sf::Color::Red);
				}
			}
		}
	}

	sprite.setColor(sf::Color(255, 255, 255, 255));

	
	textPoints.setPosition(Camera::GetCameraBounds().TopLeft() + vec(10, 10));
	textDescription.setPosition(Camera::GetCameraBounds().TopLeft() + vec(50, 250));


	textPoints.setFillColor(sf::Color(255, 255, 255, 255));
	if (not_enough_points_timer > 0.f) {
		if (int(not_enough_points_timer * 10.f) % 2) {
			textPoints.setFillColor(sf::Color(255, 0, 0, 255));
		}
	}

	window.draw(textPoints);
	window.draw(textDescription);

}
