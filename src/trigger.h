#pragma once

#include <functional>
#include "entity.h"
#include "savestate.h"
#include "selfregister.h"

struct Trigger : BoxEntity, SelfRegister<Trigger>
{
	Trigger(const std::string& id, BoxBounds area, std::function<void(Trigger*)> callback, bool singleTime = false)
		: BoxEntity(area)
		, id(id)
		, callback(callback)
		, singleTime(singleTime)
		, active(true)
	{ }
	void Update(float dt);
	void Draw() const;

	void SaveGame(SaveState& state) const {
		if (singleTime) {
			state.StreamPut("singletrigger_" + id) << active;
		}
	};

	void LoadGame(const SaveState& state) {
		// This works for entities that aren't present in the saves beacuse the stream will be empty and nothing will be read
		state.StreamGet("singletrigger_" + id) >> active;
	}

	std::string id;
	std::function<void(Trigger*)> callback;
	bool singleTime;
	bool active;
};
