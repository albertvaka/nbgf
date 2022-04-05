#pragma once

#include <functional>
#include "entity.h"
#include "savestate.h"
#include "selfregister.h"

struct Trigger : BoxEntity, SelfRegister<Trigger>
{
	// Single shot: Will only be triggered once. State can be saved to a SaveState. Restoring a save also calls the callback.
	Trigger(const std::string& id, BoxBounds area, std::function<void(Trigger*, bool loadingSave)> callback)
		: BoxEntity(area)
		, id(id)
		, callback(callback)
		, singleTime(true)
		, active(true)
	{ }
	// Multi-shot: Can be triggered multiple times
	Trigger(BoxBounds area, std::function<void(Trigger*)> callback)
		: BoxEntity(area)
		, id("")
		, callback([callback](Trigger* t, bool _) {callback(t);})
		, singleTime(false)
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
		if (singleTime) {
			// This works for entities that aren't present in the saves beacuse the stream will be empty and nothing will be read
			state.StreamGet("trigger_" + id) >> active;
			if (!active) {
				callback(this, true);
			}
		}
	}

	std::string id;
	std::function<void(Trigger*, bool)> callback;
	bool singleTime;
	bool active;
};
