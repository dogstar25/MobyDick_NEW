#include "GameObjectManager.h"
#include "ResourceManager.h"

#include <fstream>
#include <iostream>
#include <cassert>



GameObjectManager& GameObjectManager::instance()
{
	static GameObjectManager singletonInstance;
	return singletonInstance;
}


GameObjectManager::GameObjectManager()
{

}

GameObjectManager::~GameObjectManager()
{

	//Use clear and swap to ensure free memory
	this->m_gameObjectDefinitions.clear();
	std::map<std::string, GameObjectDefinition>().swap(this->m_gameObjectDefinitions);
}


bool GameObjectManager::init()
{
	//load("gameObjectDefinitions/commonObjects");
	//load("levels/level1_Objects");
	//load("gameObjectDefinitions/particleObjects");
	//load("gameObjectDefinitions/compositeObjects");

	return true;
}

//GameObject GameObjectManager::buildGameObject(std::string gameObjectId, float xMapPos, float yMapPos, float angle)
//{
//
//
//
//
//
//
//}



void GameObjectManager::load(std::string gameObjectAssetsFilename)
{

	//Read file and stream it to a JSON object
	auto gameObjectAssetResult = mobydick::ResourceManager::getJSON(gameObjectAssetsFilename + ".json");
	if (!gameObjectAssetResult) {

		std::string errorMsg = std::format("GameObjectManager: Error loading game object definition {}", gameObjectAssetsFilename);
		SDL_Log(errorMsg.c_str());
		std::abort();

	}

	Json::Value scenesJSON = gameObjectAssetResult.value();

	std::string id, textureId;

	for (Json::Value itr : scenesJSON["gameObjects"])
	{
		std::string gameObjectType = itr["type"].asString();

		m_gameObjectDefinitions.emplace(gameObjectType, *(new GameObjectDefinition(itr)));
	}
}

/*
	Retrieve the GameObjetc Definition
*/
std::shared_ptr<GameObjectDefinition> GameObjectManager::getDefinition(std::string definitionId)
{

	assert(m_gameObjectDefinitions.find(definitionId) != this->m_gameObjectDefinitions.end() && "Missing DEFAULT gameObject. Configure a GameObject named DEFAULT");

	return std::make_shared<GameObjectDefinition>(m_gameObjectDefinitions[definitionId]);

}

bool GameObjectManager::hasDefinition(std::string definitionId)
{

	if (m_gameObjectDefinitions.find(definitionId) == this->m_gameObjectDefinitions.end())
	{
		return false;
	}
	else
	{
		return true;
	}

}






