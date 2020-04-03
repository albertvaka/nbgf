#include "vector.h"

#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

#ifdef _DEBUG

struct debugvec {
    debugvec(vec v, sf::Color c) : v(v), c(c) {}
    vec v; 
    sf::Color c;
};

std::vector<debugvec> debugvecs;

void vec::Debuggerino(sf::Color color) {
    debugvecs.emplace_back(*this, color);
}

void DrawDebugVecs(sf::RenderTarget* window) {
    sf::RectangleShape rs(sf::Vector2f(1, 1));
    rs.setFillColor(sf::Color::Transparent);
    rs.setOutlineThickness(1);
    rs.setOrigin(0.5f, 0.5f);
    for (debugvec v : debugvecs) {
        rs.setOutlineColor(v.c);
        rs.setPosition(v.v);
        window->draw(rs);
    }
}
void ClearDebugVecs() {
    debugvecs.clear();
}

#endif