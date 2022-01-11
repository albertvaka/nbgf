#pragma once

#include <functional>
#include "entity.h"
#include "savestate.h"
#include "selfregister.h"

struct Trigger : BoxEntity, SelfRegister<Trigger>
{
	Trigger(const std::string& id, BoxBounds area, std::function<void(Trigger*, bool loadingSave)> callback)
		: BoxEntity(area)
		, id(id)
		, callback(callback)
		, singleTime(true)
		, active(true)
	{ }
	void Update(float dt);
	void Draw() const;

	void SaveGame(SaveState& state) const {
		if (singleTime) {
			state.StreamPut("trigger_" + id) << active;
		}
	};

	void LoadGame(const SaveState& state) {
		// This works for entities that aren't present in the saves beacuse the stream will be empty and nothing will be read
		state.StreamGet("trigger_" + id) >> active;
		if (!active) {
			callback(this, true);
		}
	}

	std::string id;
	std::function<void(Trigger*, bool)> callback;
	bool singleTime;
	bool active;
};
