#include "savestate.h"

#include <fstream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <filesystem>

#include "debug.h"

#ifdef _DEBUG
//#define SAVE_IN_LOCAL_DIR
#endif

inline std::string osConfigDir()
{
#if _WIN32
	return getenv("LOCALAPPDATA");
#elif defined(__APPLE__)
	return "~/Library/Preferences";
#elif defined(__EMSCRIPTEN__)
	return "???";
#else
	return getenv("XDG_CONFIG_HOME");
#endif
}

std::string SaveState::GetConfigFilePath() {
#ifndef SAVE_IN_LOCAL_DIR
	std::string basePath = osConfigDir() + "/" + gaemName + "/";
	std::filesystem::create_directories(basePath);
#else
	std::string basePath = "./";
#endif
	return basePath + "save" + std::to_string(stateNum) + ".txt";
}

void SaveState::Save() {
	std::ofstream file(GetConfigFilePath(), std::ofstream::out | std::ofstream::trunc);
	if (file.fail()) {
		Debug::out << "Could not open " << GetConfigFilePath() << " for writing: " << strerror(errno);
		return;
	}
	for (auto const& [id, data] : state) {
		file << id << " " << data << std::endl;
	}
	file.close();
}

void SaveState::Load() {
	Clear();

	if (!std::filesystem::exists(GetConfigFilePath())) {
		//Debug::out << "Save doesn't exist";
		return;
	}

	std::ifstream file(GetConfigFilePath(), std::ifstream::in);
	if (file.fail()) {
		Debug::out << "Could not open " << GetConfigFilePath() << " for reading: " << strerror(errno);
		return;
	}
	std::string data;
	std::string id;
	while (file >> id) {
		std::getline(file, data);
		state[id] = data;
	}
	file.close();
}