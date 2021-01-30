#pragma once

#include "scene.h"
#include "partsys.h"
#include "text.h"
#include "player.h"

struct SceneMain : Scene {

	

	SceneMain();

	void EnterScene() override;
	void ExitScene() override;
	void Update(float dt) override;
	void Draw() override;

	void SpawnBuildings();
	void SpawnPeople();
	void SpawnCity();
	void SpawnWaypoint();


};


