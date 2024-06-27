#pragma once
#include <vector>
#include <memory>
#include <optional>
#include <functional>
#include <thread>

#include <json/json.h>

#include "Scene.h"
#include "Util.h"
#include "BaseConstants.h"
#include "Timer.h"
#include "Camera.h"

inline constexpr int MAX_SCENES = 12;

struct SceneSnapshot {
	SceneState state{};
	std::shared_ptr<GameObject> cameraFollowedObject{};
	SDL_FPoint cameraLocation{};
	int inputControlMode{};
};

struct PlayerInputEvent
{

	unsigned char keyStates[SDL_NUM_SCANCODES];
	SDL_Event event;
};

class SceneManager
{

public:
	SceneManager() = default;
	~SceneManager();

	static SceneManager& instance();
	void init();
	void load(std::string sceneDefinitionsFilename);
	void run();
	std::optional<SceneAction> pollEvents();
	void popScene();
	void directScene(std::string cutSceneId);
	void toggleSetting(int settingType);
	void releaseDirectScene();
	Scene& pushScene(std::string sceneId, bool pausePreviousScene = false);
	Timer& gameTimer() { return m_gameTimer; }
	void testIMGUI();
	Scene& currentScene() { return m_scenes.back(); }
	void loadLevel(std::string levelId);
	void loadNextLevel();
	void loadCurrentLevel();
	void respawnPlayer();
	void deleteGameObject(std::string gameObjectName);
	void setMouseCursor(SDL_Cursor* mouseCursor) { m_currentMouseCursor = mouseCursor; }
	void quickSave();
	void quickLoad();
	void startNewGame();

	std::optional<SceneAction> getSceneKeyAction(SDL_Keycode);

	auto currentSceneIndex(){
		return m_currentSceneIndex;
	}
	auto setCurrentSceneIndex( int newCurrentSceneIndex) {
		m_currentSceneIndex = newCurrentSceneIndex;
	}

	auto& getDefinition(std::string definitionId) {

		return m_sceneDefinitions[definitionId];
	}

	auto& scenes() {
		return m_scenes;
	}

	auto& playerInputEvents() {
		return m_PlayerInputEvents;
	}

	//GameObject* addGameObject(std::shared_ptr<GameObject>gameObject, GameLayer layer);
	//GameObject* addGameObject(std::string gameObjectType, GameLayer layer, float xMapPos, float yMapPos, float angle = 0., bool cameraFollow = false);

private:
	
	std::vector<Scene> m_scenes;
	std::map<SDL_Keycode, SceneAction> m_globalKeyActions;
	std::vector<PlayerInputEvent> m_PlayerInputEvents;
	//std::vector<LevelTriggerEvent> m_LevelTriggerEvents;
	std::map<std::string, Json::Value>m_sceneDefinitions;
	std::map<std::string, SceneSnapshot>m_sceneSaveSnapshots{};
	
	//flags for different debug settings
	//std::bitset<8> m_debugSettings{};

	int m_currentSceneIndex{};
	Timer m_gameTimer{};
	int m_frameCount{};
	SDL_Cursor* m_currentMouseCursor;

	void _directorTakeOver();
	void _directorRelease();
	void _saveCurrentState(std::string sceneId);
	void _restoreSceneState(std::string sceneId);
	

};

