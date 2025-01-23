#include "savestate.h"

#include <fstream>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <stdio.h>
#include <stdlib.h>

#include "debug.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>

std::string SaveState::GetSaveFilePath()
{
	// directory /saves gets mounted in main.cpp
	return "/saves/" + gaemName + "_" + std::to_string(stateNum);
}

void emscripten_sync_to_persistent_storage()
{
	MAIN_THREAD_EM_ASM(
		FS.syncfs(function (err) { if (err) console.log(err); });
	);
}

#else

inline std::string osConfigDir()
{
#if _WIN32
	return getenv("LOCALAPPDATA");
#elif defined(__APPLE__)
	return std::filesystem::path(getenv("HOME")) / "Library" / "Preferences";
#else
	return getenv("XDG_CONFIG_HOME");
#endif
}

std::string SaveState::GetSaveFilePath()
{
#ifndef SAVESTATE_SAVE_IN_LOCAL_DIR
	std::string basePath = osConfigDir() + "/" + gaemName + "/";
	std::filesystem::create_directories(basePath);
#else
	std::string basePath = "./";
#endif
	return basePath + "save" + std::to_string(stateNum) + ".save";
}

#endif

void SaveState::Save()
{
	std::string path = GetSaveFilePath();
	Debug::out << "Saving to " << path;
	std::ofstream file(path, std::ofstream::out | std::ofstream::trunc);
	if (file.fail()) {
		Debug::out << "Could not open " << path << " for writing: " << strerror(errno);
		return;
	}
	for (auto const& [id, data] : state) {
		file << id << " " << data << std::endl;
	}
	file.close();

#ifdef __EMSCRIPTEN__
	emscripten_sync_to_persistent_storage();
#endif
}

void SaveState::Load()
{
	Clear();

	std::string path = GetSaveFilePath();

#if !defined(__EMSCRIPTEN__)
	if (!std::filesystem::exists(path)) {
		//Debug::out << "Save doesn't exist";
		return;
	}
#endif

	std::ifstream file(path, std::ifstream::in);
	if (file.fail()) {
		Debug::out << "Could not open " << path << " for reading: " << strerror(errno);
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
