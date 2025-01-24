#pragma once

#include <unordered_map>
#include <string>
#include <sstream>

struct SaveStream;

// Savefiles will be stored in the working dir instead of the user's config folder
#define SAVESTATE_SAVE_IN_LOCAL_DIR

struct SaveState
{
	// Open creates a SaveState and Loads its contents from disk
	[[nodiscard]] static SaveState Open(const std::string& gaemName, int stateNum = 0) { return SaveState(gaemName, stateNum); }

	[[nodiscard]] bool HasData() { return !state.empty(); }

	SaveState& Clear() {
		state.clear();
		return *this;
	}

	[[nodiscard]] bool Has(const std::string& id) const { return state.find(id) != state.cend(); }
	[[nodiscard]] std::string Get(const std::string& id) const { 
		auto data = state.find(id);
		if (data == state.cend()) {
			return "";
		}
		return data->second;
	}
	SaveState& Put(const std::string& id, const std::string& data) {
		state[id] = data;
		return *this;
	}

	[[nodiscard]] SaveStream StreamPut(const std::string& id);
	[[nodiscard]] std::istringstream StreamGet(const std::string& id) const { return std::istringstream(Get(id)); }

	void Save();

private:
	SaveState(const std::string& gaemName, int stateNum = 0) : gaemName(gaemName), stateNum(stateNum) {
		Load();
	}
	void Load();
	std::string GetSaveFilePath();

	SaveState& operator=(const SaveState&) = delete;
	SaveState(const SaveState&) = delete;

	std::unordered_map<std::string, std::string> state;
	std::string gaemName;
	int stateNum;
};

struct SaveStream
{
	SaveState* state;
	std::string id;
	std::ostringstream stream;
	SaveStream(SaveState* state, const std::string& id) : state(state), id(id) { }
	~SaveStream() {
		state->Put(id, stream.str()); 
	}
	template <typename T>
	SaveStream& operator<<(T&& val) {
		stream << " " << std::forward<T>(val);
		return *this;
	}
	SaveStream& operator=(const SaveStream&) = delete;
	SaveStream(const SaveStream&) = delete;
};


inline SaveStream SaveState::StreamPut(const std::string& id) {
	return SaveStream(this, id);
}
