#pragma once

#include <unordered_map>
#include <string>

struct StateSaver
{
	StateSaver(std::string gaemName, int stateNum = 0) : gaemName(gaemName), stateNum(stateNum) { }

	void SetState(int num) {
		Clear();
		stateNum = num;
	}

	[[nodiscard]] std::string Get(std::string id) { return state[id]; }
	void Put(std::string id, std::string data) { state[id] = data; }

	[[nodiscard]] bool HasData() { return !state.empty(); }
	void Clear() { state.clear(); }
	void Save();
	void Load();

private:
	std::string GetConfigFilePath();
	std::unordered_map<std::string, std::string> state;
	std::string gaemName;
	int stateNum;
};

