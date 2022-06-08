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


class Scene;

struct LevelObject
{
	std::string gameObjectId {};
	int layer{ LAYER_MAIN };
	int angleAdjustment {};
	bool cameraFollow{ false };
	std::string name{};
};

struct Objective
{
	std::string name{};
	std::string contextManagerId{};
	float initialValue{};
	float targetValue{};
	float currentValue{};

};

class LevelManager
{
public:

	static LevelManager& instance();

	std::string m_id; //probably same as the textureId since the map is represented by a texture
	int m_width, m_height; // in tile count
	int m_tileWidth, m_tileHeight;
	SDL_Rect m_levelBounds;

	void update(Scene* scene);
	void load(std::string levelId, Scene* scene);

	void addLevelObject(int xIndex, int yIndex, LevelObject levelObject);
	void setLevelObjectArraySize(int width, int height);
	void refreshNavigationAccess(Scene* scene);
	const std::vector<Objective>& objectives() { return m_objectives; }

	//Accessor Functions
	std::string description() {	return m_description; }
	//const std::vector<Waypoint>& waypoints() { return m_waypoints; }

private:

	LevelManager();
	~LevelManager();

	std::string m_description;
	std::string m_blueprintTexture;
	Json::Value m_colorDefinedList;
	Json::Value m_locationDefinedList;
	std::vector<std::shared_ptr<Trigger>> m_levelTriggers;
	std::vector<Objective> m_objectives{};

	std::vector< std::vector <LevelObject>> m_levelObjects;

	std::optional<LevelObject> _determineTile(int x, int y, SDL_Surface* bluePrintSurface);
	LevelObject _determineWallObject(int x, int y, SDL_Surface* bluePrintSurface);
	std::optional<LevelObject> _determineLocationDefinedObject(int x, int y);
	std::optional<LevelObject> _determineColorDefinedObject(SDL_Color color);
	void _loadDefinition(std::string levelId);
	void _buildLevelObjects(Scene* scene);
	bool _isColorDefinedObject(SDL_Color color);
};

