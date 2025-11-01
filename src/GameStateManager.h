#pragma once
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <json/json.h>



const std::string GAMEOBJECT_STATE_FILENAME = "gameObjectState.dat";

class GameStateManager
{
public:
	GameStateManager() = default;
	~GameStateManager() {};

	void initializeGameDataFile();

	virtual void saveGame() = 0;
	virtual void save(Json::Value gameData) = 0;

	virtual void loadGame() = 0;
	virtual Json::Value getSaveData() = 0;

	virtual void loadGamePrimerFile() = 0;
	virtual void _applyDataToGame(Json::Value) = 0;
	virtual void _buildSaveGameData(Json::Value& saveGameData) = 0;

protected:

	Json::Value m_loadedSaveDataJSON{};
	std::string m_beginPrimerFilename{};



};
