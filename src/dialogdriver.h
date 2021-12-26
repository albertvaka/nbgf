#pragma once

#include "dialogbox.h"
#include "assets.h"
#include "anim_lib.h"
#include <map>
#include <unordered_map>

inline std::vector<std::string> keysFromMap(const std::unordered_map<std::string, std::string>& maperino) {
	std::vector<std::string> keys;
	for (const auto& item : maperino) {
		keys.push_back(item.first);
	}
	return keys;
}

struct DialogEntry
{
	DialogEntry(const DialogCharacter& character, const std::string& text, const std::unordered_map<std::string, std::string> choicesToEntries = {})
		: character(character) , text(text) , choicesToEntries(choicesToEntries) , choices(keysFromMap(choicesToEntries))
	{ }
	const DialogCharacter& character;
	const std::string text;
	const std::unordered_map<std::string, std::string> choicesToEntries;
	const std::vector<std::string> choices;
};

struct Dialog {
	Dialog(const std::map<std::string, DialogEntry>& entries)
		: entries(entries)
	{ }
	[[nodiscard]] const DialogEntry& JumpToFirstEntry() {
		currentEntry = entries.cbegin();
		return currentEntry->second;
	}
	[[nodiscard]] bool IsLastEntry() {
		return std::next(currentEntry) == entries.cend();
	}
	[[nodiscard]] const DialogEntry& JumpToEntry(std::string id) {
		currentEntry = entries.find(id);
		return currentEntry->second;
	}
	[[nodiscard]] const DialogEntry& NextEntry() {
		currentEntry++;
		return currentEntry->second;
	}
private:
	std::map<std::string, DialogEntry>::const_iterator currentEntry;
	const std::map<std::string, DialogEntry> entries;
};

struct DialogDriver
{
	DialogBox dialogBox;
	Dialog* currentDialog;

	void StartDialog(Dialog& dialog) {
		currentDialog = &dialog;
		const DialogEntry& entry = currentDialog->JumpToFirstEntry();
		ShowEntryInDialogBox(entry);
	}

	bool IsOpen() {
		return dialogBox.IsOpen();
	}

	void Draw() const {
		dialogBox.Draw();
	}

	void Update(float dt) 
	{
		if (currentDialog)
		{
			bool advance = dialogBox.Update(dt);
			if (advance) {
				if (currentDialog->IsLastEntry())
				{
					dialogBox.Close();
				}
				else 
				{
					const DialogEntry& entry = currentDialog->NextEntry();
					ShowEntryInDialogBox(entry);
				}
			}
		}
	}

private:
	void ShowEntryInDialogBox(const DialogEntry& entry)
	{
		dialogBox.ShowMessage(entry.character, entry.text, currentDialog->IsLastEntry());
		if (!entry.choices.empty()) {
			dialogBox.AddChoices(entry.choices);
		}
	}
};


