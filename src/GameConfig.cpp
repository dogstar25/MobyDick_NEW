#include "GameConfig.h"

#include "game.h"
#include "EnumMap.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <print>


extern std::unique_ptr<Game> game;

GameConfig::GameConfig()
{


}

GameConfig::~GameConfig()
{

}

std::expected<void , std::string> GameConfig::init(std::string configFile)
{

	//Set the working directory for the game
    //Uses a CMAKE define in the Games levels CMakeLists
	auto result = setWorkingDirectory();
	if (!result) {
		return result;
	}

	//Read and process the game config file
	Json::Value root;
	std::ifstream ifs("assets/config/" + configFile + ".json");
	ifs >> root;

	m_gameTitle = root["gameTitle"].asString();
	m_gameLoopStep = root["gameLoopStep"].asFloat();
	m_dynamicTextRefreshDelay = root["dynamicTextRefreshDelay"].asFloat();
	m_debugPanelLocation.x = root["debugPanel"]["xPos"].asInt();
	m_debugPanelLocation.y = root["debugPanel"]["yPos"].asInt();
	m_debugPanelFontSize = root["debugPanel"]["fontSize"].asInt();
	m_soundChannels = root["sound"]["numberOfChannels"].asInt();
	m_scaleFactor = root["physics"]["box2dScale"].asFloat();
	m_rendererType = static_cast<RendererType>(game->enumMap()->toEnum(root["rendererType"].asString()));
	m_openGLBatching = root["openGLBatching"].asBool();

	m_debugGrid = root["debugGrid"].asBool();

	Json::Value windowJSON = root["window"];
	m_defaultTileSize = { windowJSON["defaultTileSize"]["width"].asInt(), windowJSON["defaultTileSize"]["height"].asInt() };
	m_targetScreenResolution = { windowJSON["targetScreenResolution"]["width"].asInt(), windowJSON["targetScreenResolution"]["height"].asInt() };
	m_fallbackScreenResolution = { windowJSON["fallbackScreenResolution"]["width"].asInt(), windowJSON["fallbackScreenResolution"]["height"].asInt() };

	m_windowFullscreen = root["window"]["fullscreen"].asBool();

	return{};
}

GameConfig& GameConfig::instance()
{

	static GameConfig singletonInstance;
	return singletonInstance;

}

std::expected<void, std::string> GameConfig::setWorkingDirectory()
{
    namespace fs = std::filesystem;

	#ifndef GAME_WORKING_DIR

		return std::unexpected("No GAME_WORKING_DIR define setup in your Games CMAKELists.txt file!");

	#endif

    // Try automatically walking upward until "assets" is found
	fs::path pathCheckPrefix = fs::current_path();
	fs::path pathCheck = pathCheckPrefix / GAME_WORKING_DIR / "assets";
    for (int i = 0; i < 5; ++i) // climb up to 5 levels max
    {
        if (fs::exists(pathCheck))
        {
            //We found the gameTitle/assets directory, now remove the assets directory part for our working directory
            fs::current_path(pathCheck.parent_path());
            std::println("[Init] Changed working directory to: {}", fs::current_path().string());
			return{};
        }

		pathCheckPrefix = pathCheckPrefix.parent_path();
		pathCheck = pathCheckPrefix / GAME_WORKING_DIR / "assets";

    }

    std::println("[Init] Warning: could not locate assets folder near {}", fs::current_path().string());
	return std::unexpected("[error]:No assets directory found for this game!");

}
