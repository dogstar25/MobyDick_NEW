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

#include <thread>

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
	int subSteps{};
};

struct Parallax
{
	int layer{};
	float rate{1};
};

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

	static bool _shouldCollide(b2ShapeId shapeAId, b2ShapeId shapeBId, void* context);

	std::shared_ptr<GameObject> createGameObject(std::string gameObjectType, GameObject* parent, float xMapPos, float yMapPos, float angleAdjust, 
		Scene* parentScene, GameLayer layer = GameLayer::MAIN, bool cameraFollow = false, std::string name = "", b2Vec2 sizeOverride = { 0.,0. });

	GameObject* addGameObject(std::string gameObjectType, GameObject* parent, GameLayer layer, float xMapPos, float yMapPos,
		float angle=0., bool cameraFollow=false, std::string name="", b2Vec2 sizeOverride = { 0.,0. });

	GameObject* addGameObject(std::string gameObjectType, GameObject* parent, GameLayer layer, PositionAlignment windowPosition,
		float adjustX=0., float adjustY=0., float angle=0.,bool cameraFollow = false, std::string name = "", b2Vec2 sizeOverride = { 0.,0. });

	void addGameObject(std::shared_ptr<GameObject> gameObject, GameLayer layer);
	void addGameObjectFromPool(std::shared_ptr<GameObject> gameObject, GameLayer layer);

	void addGameObjectIndex(std::shared_ptr<GameObject> gameObject);
	void addLevelObjective(Objective objective);
	void addLevelTrigger(std::shared_ptr<Trigger> trigger);
	void addKeyAction(SDL_Keycode, SceneAction);
	void applyCurrentControlMode();
	
	std::optional<std::shared_ptr<GameObject>> getGameObject(std::string id);
	std::optional<std::shared_ptr<GameObject>> extractGameObject(std::string id);
	std::vector<std::shared_ptr<GameObject>> getGameObjectsByName(std::string name);
	std::optional<std::shared_ptr<GameObject>> getFirstGameObjectByName(std::string name);
	std::vector<std::shared_ptr<GameObject>> getGameObjectsByTrait(int trait, bool includePooled = false);
	std::optional<std::shared_ptr<GameObject>> getFirstGameObjectByTrait(int trait); //use when you know there's only one
	std::optional<std::shared_ptr<GameObject>> getFirstGameObjectByType(std::string type); //use when you know there's only one
	std::optional<std::string> getNextLevel();
	void flashContactListener();

	int getRenderSequence() { return m_renderSequence; }
	int incrementRenderSequence(GameObject* gameObject);
	
	
	void stepB2PhysicsWorld();

	const std::array <std::vector<std::shared_ptr<GameObject>>, GameLayer::GameLayer_COUNT>& gameObjects() {
		return m_gameObjects;
	}

	//Accessor Functions
	std::string id() {	return m_id;}
	std::shared_ptr<GameObject> player() { return m_player; }
	int parentSceneIndex() { return m_parentSceneIndex; }
	b2WorldId physicsWorld() { return m_physicsWorld;	}
	DebugDraw debugDraw() { return m_debugDraw; }
	const std::vector<Objective>& objectives() { return m_levelObjectives; }

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
	void showNavigationMap();
	void updateGridDisplay(int xPos, int yPos, int operation, SDL_Color color);

	void setDraggingObject(std::weak_ptr<GameObject> gameObject);

	std::unordered_map<std::string, std::weak_ptr<GameObject>>& getGameObjectLookup() { return m_gameObjectLookup; }

private:

	std::string m_id;

	int m_renderSequence{};
	int m_gameObjectCount{};
	int m_inputControlMode{};
	int m_parentSceneIndex{};
	bool m_hasPhysics{};
	SDL_FPoint m_playerOrigSpawnPoint{};
	std::shared_ptr<GameObject> m_player;

	SceneState m_state{};
	std::optional<std::shared_ptr<CutScene>> m_cutScene{};
	b2WorldId m_physicsWorld;
	DebugDraw m_debugDraw;
	PhysicsConfig m_physicsConfig{};
	ObjectPoolManager m_objectPoolManager{};

	std::unordered_map<std::string, std::weak_ptr<GameObject>> m_gameObjectLookup;
	std::array <std::vector<std::shared_ptr<GameObject>>, GameLayer::GameLayer_COUNT> m_gameObjects;

	std::bitset<8> m_sceneTags;
	std::bitset<8> m_sceneDebugSettings{};
	std::map<SDL_Keycode, SceneAction> m_sceneKeyActions;

	//Level related members
	std::string m_currentLevelId;
	std::vector<std::shared_ptr<Trigger>> m_levelTriggers;
	std::vector<Objective> m_levelObjectives;
	std::map<int, Parallax>m_parallaxRates;

	std::optional<std::weak_ptr<GameObject>> m_draggingObject{};

	void _processGameObjectInterdependecies();
	void _buildPhysicsWorld(Json::Value physicsJSON);
	void _buildSceneGameObjects(Json::Value sceneJSON);
	void _removeFromWorldPass();
	void _showNavigationMap();
	bool _updateNavigationMap();
	void _resetRenderSequence() { m_renderSequence = 0; }
	
};

