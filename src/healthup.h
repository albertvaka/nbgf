#pragma once

#include "vec.h"
#include "entity.h"
#include "savestate.h"
#include "selfregister.h"

struct HealthUp : BoxEntity, SelfRegister<HealthUp>
{
	HealthUp(int id, vec p);
	void Update(float dt);
	void Draw() const;

	void SaveGame(SaveState& state) const {
		state.StreamPut("healthup_" + std::to_string(id)) << pickedUp; 
	};

	void LoadGame(const SaveState& state) {
		// This works as we add more entities that aren't present in the saves beacuse the stream will be empty and nothing will be read
		state.StreamGet("healthup_" + std::to_string(id)) >> pickedUp;
	}

	int id;
	bool pickedUp;

private:
	bool IsBehindBreakable() const;
};
