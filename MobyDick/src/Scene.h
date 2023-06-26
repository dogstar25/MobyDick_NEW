#pragma once

#include <memory>
#include <vector>
#include <bitset>
#include <optional>
#include <unordered_map>

#include <box2d/box2d.h>
#include <json/json.h>
#include "Util.h"
#include "BaseConstants.h"
#include "GameObject.h"
#include "GameConfig.h"
#include "ObjectPoolManager.h"
#include "triggers/Trigger.h"

class CutScene;
struct Objective;

struct SceneAction
{
	int actionCode{ 0 };
	std::string actionId;
};

struct PhysicsConfig
{
	b2Vec2 gravity{};
	float timeStep{};
	int velocityIterations{};
	int positionIterations{};
};

struct Parallax
{
	int layer{};
	float rate{1};
};

struct NavigationMapItem
{
	bool passable{ true };
	std::optional<std::weak_ptr<GameObject>>gameObject{};
};


void _updatePhysics(b2World* physicsWorld);

class Scene
{

public:
	Scene(std::string sceneId);
	~Scene();

	void loadLevel(std::string levelId);
	void loadNextLevel();
	void loadCurrentLevel();
	void reset();
	void clear();
	void render();
	void update();
	void clearEvents();

	GameObject* addGameObject(std::string gameObjectType, GameLayer layer, float xMapPos, float yMapPos, 
		float angle=0., bool cameraFollow=false, std::string name="");
	GameObject* addGameObject(std::string gameObjectType, GameLayer layer, PositionAlignment windowPosition, 
		float adjustX=0., float adjustY=0., float angle=0.,
		bool cameraFollow=false, std::string name="");
	void addGameObject(std::shared_ptr<GameObject> gameObject, GameLayer layer);

	void addGameObjectIndex(std::shared_ptr<GameObject> gameObject);
	void addNavigationMapItem(NavigationMapItem& navigationMapItem, int x, int y);
	void addLevelObjective(Objective objective);
	void addLevelTrigger(std::shared_ptr<Trigger> trigger);
	void addKeyAction(SDL_Keycode, SceneAction);
	void applyCurrentControlMode();
	std::optional<std::shared_ptr<GameObject>> getGameObject(std::string id);
	std::vector<std::shared_ptr<GameObject>> getGameObjectsByName(std::string name);
	std::optional<std::shared_ptr<GameObject>> getFirstGameObjectByName(std::string name);
	std::vector<std::shared_ptr<GameObject>> getGameObjectsByTrait(int trait);
	std::optional<std::shared_ptr<GameObject>> getFirstGameObjectByTrait(int trait); //use when you know there's only one
	std::optional<std::shared_ptr<GameObject>> getFirstGameObjectByType(std::string type); //use when you know there's only one
	std::optional<std::string> getNextLevel();
	
	void stepB2PhysicsWorld() {
		m_physicsWorld->Step(m_physicsConfig.timeStep,
			m_physicsConfig.velocityIterations,
			m_physicsConfig.positionIterations);
	}

	const std::array <std::vector<std::shared_ptr<GameObject>>, GameLayer::COUNT>& gameObjects() {
		return m_gameObjects;
	}

	//Accessor Functions
	std::string id() {	return m_id;}
	int parentSceneIndex() { return m_parentSceneIndex; }
	b2World* physicsWorld() { return m_physicsWorld;	}
	int inputControlMode() { return m_inputControlMode; }
	const std::vector<Objective>& objectives() { return m_levelObjectives; }

	std::vector < std::vector<NavigationMapItem>>& navigationMap() { return m_navigationMap; }
	void setNavigationMapArraySize(int width, int height);

	void setState(SceneState state) { m_state = state; }
	SceneState state() { return m_state; }

	std::optional<std::shared_ptr<CutScene>> cutScene() { return m_cutScene; }

	ObjectPoolManager& objectPoolManager() { return m_objectPoolManager; }
	PhysicsConfig physicsConfig() { return m_physicsConfig; }
	bool hasPhysics() { return m_hasPhysics; }
	int gameObjectCount() { return m_gameObjectCount; }
	void incrementGameObjectCount() { m_gameObjectCount += 1; }

	void setInputControlMode(int inputControlMode);
	std::optional<SceneAction> getkeycodeAction(SDL_Keycode keycode);
	void direct();
	void setCutScene(std::shared_ptr<CutScene>cutScene);
	void deleteCutScene();
	void addParallaxItem(Parallax& parallaxItem);
	std::optional<Parallax> getParallax(GameLayer layer);

	void deleteIndex(std::string gameObjectIndex);
	void setPlayerOriginalSpawnPoint(float x, float y) { m_playerOrigSpawnPoint = {x,y}; }
	SDL_FPoint playerOriginalSpawnPoint() { return m_playerOrigSpawnPoint; }
	void resetTrigger(std::string trigger);

	void setDebugSetting(int setting);
	bool isDebugSetting(int setting);
	void resetGridDisplay();
	void updateGridDisplay(int xPos, int yPos, int operation, SDL_Color color);

	bool navigationMapChanged() {
		return m_navigationMapChanged;
	}

private:

	std::string m_id;

	int m_gameObjectCount{};
	int m_inputControlMode{};
	int m_parentSceneIndex{};
	bool m_hasPhysics{};
	SDL_FPoint m_playerOrigSpawnPoint{};
	bool m_navigationMapChanged{};

	SceneState m_state{};
	std::optional<std::shared_ptr<CutScene>> m_cutScene{};
	b2World* m_physicsWorld{};
	PhysicsConfig m_physicsConfig{};
	ObjectPoolManager m_objectPoolManager{};

	std::unordered_map<std::string, std::weak_ptr<GameObject>> m_gameObjectLookup;
	std::array <std::vector<std::shared_ptr<GameObject>>, GameLayer::COUNT> m_gameObjects;

	std::vector < std::vector<NavigationMapItem>> m_navigationMap{};

	std::bitset<8> m_sceneTags;
	std::bitset<8> m_sceneDebugSettings{};
	std::map<SDL_Keycode, SceneAction> m_sceneKeyActions;

	//Level related members
	std::string m_currentLevelId;
	std::vector<std::shared_ptr<Trigger>> m_levelTriggers;
	std::vector<Objective> m_levelObjectives;
	std::map<int, Parallax>m_parallaxRates;

	void _processGameObjectInterdependecies();
	void _buildPhysicsWorld(Json::Value physicsJSON);
	void _buildSceneGameObjects(Json::Value sceneJSON);
	void _removeFromWorldPass();
	void _showNavigationMap();
	bool _updateNavigationMap();
	

};

