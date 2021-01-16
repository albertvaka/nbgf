#pragma once

#include "vec.h"
#include "entity.h"
#include "savestate.h"
#include "selfregister.h"
#include "skilltree.h"
#include "partsys.h"

//UNUSED
struct PowerUp : BoxEntity, SelfRegister<PowerUp>
{
	PowerUp(vec p) : BoxEntity(p+vec(8,-8), vec(16,16)) { }
	void Draw() const;
};

struct HealthUp : BoxEntity, SelfRegister<HealthUp>
{
	HealthUp(int id, vec p) : id(id), pickedUp(false), BoxEntity(p+vec(8,-8), vec(16, 16)) { }
	void Draw() const;
	void Update(float dt);

	void SaveGame(SaveState& state) const { 
		state.StreamPut("healthup_" + std::to_string(id)) << pickedUp; 
	};
	void LoadGame(const SaveState& state) { 
		// This works as we add more entities that aren't present in the saves beacuse the stream will be empty and nothing will be read
		state.StreamGet("healthup_" + std::to_string(id)) >> pickedUp;
	}

	int id;
	bool pickedUp;
};

struct BigItem : BoxEntity, SelfRegister<BigItem>
{
	BigItem(vec p, Skill s);
	void Draw();
	void DrawPedestal();

	Skill skill;
	PartSys particles;
};


