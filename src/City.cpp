#include "City.h"
#include "camera.h"
#include "assets.h"
#include "debug.h"
#include "window_conf.h"


const int tileSize = 16;

City::City(const std::vector< std::vector<CityTiles> > &tiles): mTiles(tiles) {
}
City::~City() {
}

void City::Draw() {
    for (int r = 0; r < mTiles.size(); r++) {
        const auto row = mTiles[r];
        const int cellsPerRow = row.size();
        for(int c = 0; c < row.size(); c++) {
            const auto cell = row[c];
            const int rowIndex = cell/cellsPerRow;
            const int tileIndex = cell%cellsPerRow;
            Window::Draw(Assets::terrainTexture, vec(c*16*Window::MAP_SCALE, r*16*Window::MAP_SCALE))
                .withRect(16*rowIndex, 16*tileIndex, 16, 16)
                .withScale(Window::MAP_SCALE);
        }
    }
}