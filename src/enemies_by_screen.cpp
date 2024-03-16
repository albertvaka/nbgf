#include "enemies_by_screen.h"

#include <SDL.h>

#include <unordered_map>
#include <algorithm>

static std::vector<Entity*> emptyVec;
static std::unordered_map<int, std::vector<Entity*>> maperino;

void EnemiesByScreen::AssertEmpty() {
	SDL_assert(maperino.empty());
}

void EnemiesByScreen::Add(int screen, Entity * e) {
	maperino[screen].push_back(e);
}

void EnemiesByScreen::Remove(int screen, Entity* e) {
	auto p = maperino.find(screen);
	SDL_assert(p != maperino.end());

	std::vector<Entity*>& list = p->second;
	auto pp = std::find(list.begin(), list.end(), e);
	SDL_assert(pp != list.end());

	list.erase(pp);

	if (list.empty()) {
		maperino.erase(p);
	}
}

const std::vector<Entity*>& EnemiesByScreen::Get(int screen) {
	auto p = maperino.find(screen);
	if (p != maperino.end()) {
		return p->second;
	} else {
		return emptyVec;
	}
}
