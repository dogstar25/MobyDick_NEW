#include "GameConfig.h"

#include "game.h"
#include "EnumMap.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <print>
#include "ResourceManager.h"


extern std::unique_ptr<Game> game;

GameConfig::GameConfig()
{


}

GameConfig::~GameConfig()
{

}

std::expected<void , std::string> GameConfig::init(std::string configFile)
{

	//Read and process the game config file
	auto result = mobydick::ResourceManager::getJSON("config/" + configFile + ".json");
	if (!result) {
		return std::unexpected(result.error());
	}

	Json::Value configJSON = result.value();

	m_gameTitle = configJSON["gameTitle"].asString();
	m_gameLoopStep = configJSON["gameLoopStep"].asFloat();
	m_dynamicTextRefreshDelay = configJSON["dynamicTextRefreshDelay"].asFloat();
	m_debugPanelLocation.x = configJSON["debugPanel"]["xPos"].asInt();
	m_debugPanelLocation.y = configJSON["debugPanel"]["yPos"].asInt();
	m_debugPanelFontSize = configJSON["debugPanel"]["fontSize"].asInt();
	m_soundChannels = configJSON["sound"]["numberOfChannels"].asInt();
	m_scaleFactor = configJSON["physics"]["box2dScale"].asFloat();
	m_rendererType = static_cast<RendererType>(game->enumMap()->toEnum(configJSON["rendererType"].asString()));
	m_openGLBatching = configJSON["openGLBatching"].asBool();
	m_installGameOrg = configJSON["gameInstallOrg"].asString();
	m_installGameName = configJSON["gameInstallName"].asString();

	m_debugGrid = configJSON["debugGrid"].asBool();

	Json::Value windowJSON = configJSON["window"];
	m_defaultTileSize = { windowJSON["defaultTileSize"]["width"].asInt(), windowJSON["defaultTileSize"]["height"].asInt() };
	m_targetScreenResolution = { windowJSON["targetScreenResolution"]["width"].asInt(), windowJSON["targetScreenResolution"]["height"].asInt() };
	m_fallbackScreenResolution = { windowJSON["fallbackScreenResolution"]["width"].asInt(), windowJSON["fallbackScreenResolution"]["height"].asInt() };

	m_windowFullscreen = configJSON["window"]["fullscreen"].asBool();

	return{};
}

GameConfig& GameConfig::instance()
{

	static GameConfig singletonInstance;
	return singletonInstance;

}

