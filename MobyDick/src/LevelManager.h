#pragma once

#include <string>
#include <vector>
#include <optional>
#include <SDL2/SDL.h>
#include <box2d/box2d.h>
#include <json/json.h>
#include <map>

#include "GameObject.h"
#include "Util.h"
#include "BaseConstants.h"
#include "triggers/Trigger.h"
#include "Scene.h"

//class Scene;

struct LevelObject
{
	std::string gameObjectType{};
	bool isPlayer{};
	GameLayer layer{ GameLayer::MAIN };
	int angleAdjustment {};
	bool cameraFollow{ false };
	std::string name{};
	std::optional<b2Vec2>size;
	std::optional<b2Vec2>locationAdjust;
	std::optional<SDL_Color> color;
	std::optional<int> disabledType;
	std::optional<float> weaponForce;
	std::optional<SDL_Color> weaponColor;
	std::optional<int> compositePieceLevelCap;
	std::optional<int> brainSensorSize;
	std::optional<float> containerRespawnTimer;
	std::optional<int> containerStartCount;
	std::optional<int> containerCapacity;

};

struct Objective
{
	int id{};
	float targetValue{};

};

struct TiledLayerDefinition
{
	int layerId{};
	std::string tiledObjectId{};
	SDL_Point tileSize{};

};



class LevelManager
{
public:

	static LevelManager& instance();

	std::string m_id{}; //probably same as the textureId since the map is represented by a texture
	int m_width{};
	int m_height{}; // in tile count
	int m_tileWidth{};
	int m_tileHeight{};
	SDL_Rect m_levelBounds{};
	std::optional<SDL_Color> m_wallColor;

	void loadLevel(std::string levelId, Scene* scene);
	void loadLevel(std::string levelId, std::string sceneId);
	std::optional<std::string> getNextLevelId(std::string levelId);

	void setLevelObjectArraySize(int width, int height);
	

	//Accessor Functions
	std::string description() {	return m_description; }

private:

	LevelManager();
	~LevelManager();

	std::string m_description{};
	std::string m_blueprintTexture{};
	Json::Value m_colorDefinedList{};
	Json::Value m_locationDefinedList{};
	Json::Value m_levelDefinition{};
	std::optional<std::string> m_backgroundMusicAssetId{};

	std::vector<std::vector< std::vector <LevelObject>>> m_levelObjects{};
	std::vector<std::string> m_levels{};
	std::map<int, TiledLayerDefinition> m_tiledLayerDefinitions;

	std::vector<LevelObject> _determineTile(int x, int y, SDL_Surface* bluePrintSurface);
	LevelObject _determineWallObject(int x, int y, SDL_Surface* bluePrintSurface);
	std::vector<LevelObject> _determineLocationDefinedObject(int x, int y);
	std::optional<LevelObject> _determineColorDefinedObject(SDL_Color color);
	void _loadDefinition(std::string levelId);
	void _buildLevelObjects(Scene* scene);
	void _buildLevelObjectives(Scene* scene);
	void _buildLevelStatusItems(std::string levelId);
	void _buildLevelTriggers(Scene* scene);
	void _buildTiledLayers(Scene* scene);
	void _buildParallax(Scene* scene);
	void _buildLevelCage(Scene* scene);
	void _buildNavigationMapItem(GameObject* gameObject, Scene* scene);
	void _buildDebugGridObjects(Scene* scene);
	void _startBackgroundMusic();
	bool _isColorDefinedObject(SDL_Color color);
	void _clear();
};

