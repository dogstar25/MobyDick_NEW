#include "GameStateManager.h"
#include "SoundManager.h"
#include "BaseConstants.h"
#include <iostream>
#include <shlobj.h>
#include <assert.h>
#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>
#include <Windows.h>
#include "game.h"
#include "Util.h"


extern std::unique_ptr<Game> game;

GameStateManager::GameStateManager()
{
	PWSTR userFolderPath;
	HRESULT result = SHGetKnownFolderPath(FOLDERID_Profile, 0, NULL, &userFolderPath);

    assert(result == S_OK && "Error getting userdata directory for savegame file!");

    m_saveGamePath = util::wideStringToString(userFolderPath);

    CoTaskMemFree(static_cast<LPVOID>(userFolderPath));

}

void GameStateManager::initializeGameDataFile()
{

	std::string gameSaveFilename = m_saveGamePath + GAMEOBJECT_STATE_FILENAME;

	if (util::fileExists(gameSaveFilename) == false) {
		std::ofstream m_gameFile(m_saveGamePath + GAMEOBJECT_STATE_FILENAME);

		m_gameFile.close();
		assert(m_gameFile && "Error creating game file");

		loadGamePrimerFile();
	}
}

