#include "GameStateManager.h"
#include "SoundManager.h"
#include "BaseConstants.h"
#include <iostream>
#include <assert.h>
#include <filesystem>
#include <string>
#include <locale>
#include <codecvt>
#include "game.h"
#include "Util.h"
#include "ResourceManager.h"


extern std::unique_ptr<Game> game;


void GameStateManager::initializeGameDataFile()
{

	auto saveDataResult = mobydick::ResourceManager::saveUserPathDataJSON(Json::Value(), GAMEOBJECT_STATE_FILENAME);
	if (!saveDataResult) {

		SDL_Log("loadSettings: Error saving %s", GAME_SETTINGS_FILENAME.c_str());
		abort();

	}

	loadGamePrimerFile();
}

