#pragma once

#include <vector>
#include "TileUtils.h"

struct City {

	City(const std::vector< std::vector<CityTiles> > &tiles);
	~City();

    std::vector< std::vector<CityTiles> > mTiles;

	void Update(float dt);
	void Draw();
};
