#include "savestate.h"

#include <fstream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <filesystem>

#include "debug.h"

#if !(MACOS_VER_MAJOR==10 && MACOS_VER_MINOR<=14)
	// std::filesystem is only supported on MacOS 10.15+
	#define SAVESTATE_SAVE_IN_LOCAL_DIR
#endif

inline std::string osConfigDir()
{
#if _WIN32
	return getenv("LOCALAPPDATA");
#elif defined(__APPLE__)
	return "~/Library/Preferences";
#elif defined(__EMSCRIPTEN__)
	return "/"; // TODO
#else
	return getenv("XDG_CONFIG_HOME");
#endif
}

std::string SaveState::GetSaveFilePath() {
#ifndef SAVESTATE_SAVE_IN_LOCAL_DIR
	std::string basePath = osConfigDir() + "/" + gaemName + "/";
	std::filesystem::create_directories(basePath);
#else
	std::string basePath = "./";
#endif
	return basePath + "save" + std::to_string(stateNum) + ".save";
}

void SaveState::Save() {
	std::ofstream file(GetSaveFilePath(), std::ofstream::out | std::ofstream::trunc);
	if (file.fail()) {
		Debug::out << "Could not open " << GetSaveFilePath() << " for writing: " << strerror(errno);
		return;
	}
	for (auto const& [id, data] : state) {
		file << id << " " << data << std::endl;
	}
	file.close();
}

void SaveState::Load() {
	Clear();

#if !(MACOS_VER_MAJOR==10 && MACOS_VER_MINOR<=14)
	if (!std::filesystem::exists(GetSaveFilePath())) {
		//Debug::out << "Save doesn't exist";
		return;
	}
#endif

	std::ifstream file(GetSaveFilePath(), std::ifstream::in);
	if (file.fail()) {
		Debug::out << "Could not open " << GetSaveFilePath() << " for reading: " << strerror(errno);
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