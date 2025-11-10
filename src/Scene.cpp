#include "Scene.h"


#include "EnumMap.h"

#include "GameObjectManager.h"
#include "SoundManager.h"
#include "game.h"
#include "LevelManager.h"
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game> game;

Scene::Scene(std::string sceneId)
{

	//Get the scene definition
	Json::Value definitionJSON = SceneManager::instance().getDefinition(sceneId);
	m_id = sceneId;

	//Start the scene in run mode
	m_state = SceneState::RUN;

	//Physics if its needed for the scene
	if (definitionJSON.isMember("physics")) {
		m_hasPhysics = true;
		Json::Value physicsJSON = definitionJSON["physics"];
		_buildPhysicsWorld(physicsJSON);
	}

	
	//Allocate the arrays for all of the gameObjects
	auto maxObjects = definitionJSON["maxObjects"].asInt();
	for (auto& gameLayer : m_gameObjects)
	{
		gameLayer.reserve(maxObjects);
	}

	//Build ObjectPoolManager
	m_objectPoolManager.init(definitionJSON, this);

	//Set the mouse mode
	auto inputControlMode = game->enumMap()->toEnum(definitionJSON["inputControlMode"].asString());
	m_inputControlMode = inputControlMode;
	setInputControlMode(inputControlMode);

	//GameObjects that are defined at the scene level, not a level built scene
	_buildSceneGameObjects(definitionJSON);

	//Tags
	for (Json::Value itrTag : definitionJSON["tags"]) {

		std::size_t tag = game->enumMap()->toEnum(itrTag.asString());
		m_sceneTags.set(tag);
	}

	//Keycode actions
	for (Json::Value keyActionJSON : definitionJSON["keyActions"]) {

		auto keyCode = game->enumMap()->toEnum(keyActionJSON["keyCode"].asString());

		auto action = game->enumMap()->toEnum(keyActionJSON["sceneAction"]["action"].asString());
		auto actionId = keyActionJSON["sceneAction"]["actionId"].asString();

		SceneAction sceneAction = { int(action) , actionId };
		addKeyAction(keyCode, sceneAction);
	}

	//Debug Mode
	if (m_hasPhysics == true)
	{
		m_debugDraw.setDrawShapes(true);
		m_debugDraw.setDrawBounds(false);
		m_debugDraw.setUseDrawingBounds(false);
	}

	//If there is background music specified then play it
	if (definitionJSON.isMember("backgroundMusicId")) {

		std::string backGroundMusicId = definitionJSON["backgroundMusicId"].asString();
		game->soundManager()->playMusic(backGroundMusicId, -1);
	}

}

Scene::~Scene()
{
	clear();

}

void Scene::setDebugSetting(int setting) {

	m_sceneDebugSettings.flip(setting);

	//If this is the debug navigation grid then reset it
	if (setting == DebugSceneSettings::SHOW_NAVIGATION_DEBUG_MAP) {
		resetGridDisplay();
	}


}

bool Scene::isDebugSetting(int setting)
{

	return m_sceneDebugSettings.test(setting);

}

void Scene::loadLevel(std::string levelId)
{

	m_currentLevelId = levelId;

	//Stop music from previous level
	game->soundManager()->stopMusic();

	game->_displayLoadingMsg();

	reset();

	//Build all of the gameObjects from the level definition
	LevelManager::instance().loadLevel(levelId, this);

	//Run GameObject code that requires ALL gameObjects to be created first, for interdependency logic
	_processGameObjectInterdependecies();

}

void Scene::loadNextLevel()
{

	std::optional<std::string> nextLevelId = getNextLevel();
	if (nextLevelId.has_value()) {
		loadLevel(nextLevelId.value());
	}
	else {
		
		util::sendSceneEvent(SCENE_ACTION_ADD_AND_PAUSE, "SCENE_YOU_WIN");

	}

}

std::optional<std::string> Scene::getNextLevel()
{

	return LevelManager::instance().getNextLevelId(m_currentLevelId);

}

int Scene::incrementRenderSequence(GameObject* gameObject)
{
	m_renderSequence += 1;

	gameObject->setRenderOrder(m_renderSequence);

	return m_renderSequence;

}

void Scene::loadCurrentLevel()
{

	loadLevel(m_currentLevelId);

}

std::optional<SceneAction> Scene::getkeycodeAction(SDL_Keycode keycode) {
	if (m_sceneKeyActions.find(keycode) != m_sceneKeyActions.end()) {
		return m_sceneKeyActions.at(keycode);
	}
	else {
		return std::nullopt;
	}
}

void Scene::reset()
{

	clear();

	//Rebuild everything
	game->_displayLoadingMsg();

	Json::Value sceneJSON = SceneManager::instance().getDefinition(m_id);

	if (sceneJSON.isMember("physics")) {
		m_hasPhysics = true;
		Json::Value physicsJSON = sceneJSON["physics"];
		_buildPhysicsWorld(physicsJSON);
	}

	m_objectPoolManager.init(sceneJSON, this);

	//GameObjects that are defined at the scene level, not a level built scene
	_buildSceneGameObjects(sceneJSON);

	//Debug Mode
	if (m_hasPhysics == true)
	{
		
		//DebugDraw::instance().SetFlags(DebugDraw::e_shapeBit);
		//m_physicsWorld->SetDebugDraw(&DebugDraw::instance());
	}

	//DebugSettings
	m_sceneDebugSettings.reset();

}

void Scene::clear()
{

	Camera::instance().init();

	//Clear everything
	m_objectPoolManager.clear();
	m_levelTriggers.clear();
	m_levelObjectives.clear();

	m_gameObjectLookup.clear();
	m_player.reset();

	for (int x = 0; x < GameLayer::GameLayer_COUNT; x++)
	{
		m_gameObjects[x].clear();
	}

	if (m_hasPhysics) {
		b2DestroyWorld(m_physicsWorld);
		m_physicsWorld = b2_nullWorldId;
	}

}

void Scene::update() {


	//Direct the scne if it has a cutScene assigned
	if (m_cutScene.has_value() == true) {
		direct();
	}

	Camera::instance().update();
	game->soundManager()->update();

	if (hasPhysics()) {
		stepB2PhysicsWorld();
	}


	//Update each gameObject in all layers
	for (auto& gameObjects : m_gameObjects)
	{

		for (int i = 0; i < gameObjects.size(); i++)
		{

			gameObjects[i]->update();
		}
	}

	//Clear all events
	SceneManager::instance().playerInputEvents().clear();

	//Check all level triggers
	for (const auto& trigger : m_levelTriggers) {

		if (trigger->hasMetCriteria(this)) {
			trigger->execute();
		}
	}

	// Remove all objects that should be removed in first pass
	_removeFromWorldPass();

	//Update the navigationMap
	game->navigationManager()->updateNavigationMap();

	//If the Debug display navigation grid is turned on then build it
	if (isDebugSetting(DebugSceneSettings::SHOW_NAVIGATION_DEBUG_MAP)) {
		_showNavigationMap();
	}

}

void Scene::render() {

	//Render all of the layers
	for (auto& gameLayer : m_gameObjects)
	{

		//Render all of the GameObjects in this layer
		for (auto& gameObject : gameLayer)
		{

			gameObject->render();

		}

	}

	_resetRenderSequence();
	
	//If we have an object being dragged then render it here so that it is always  on top
	if (m_draggingObject && m_draggingObject.value().expired() == false) {

		m_draggingObject.value().lock()->enableRender();
		m_draggingObject.value().lock()->render();
		m_draggingObject.value().lock()->disableRender();
	}

	//DebugDraw
	if (m_hasPhysics && isDebugSetting(DebugSceneSettings::SHOW_PHYSICS_DEBUG) == true)
	{

		m_debugDraw.draw(m_physicsWorld);

	}

}

void Scene::setDraggingObject(std::weak_ptr<GameObject> gameObject)
{

	//When we stop dragging the dragged object then we have to reset it and make sure it
	//render is enabled because we turn off rendering fron the main loop so that it doesnt render twice
	if (gameObject.expired() && m_draggingObject.has_value() && m_draggingObject.value().expired() == false) {

		m_draggingObject.value().lock()->enableRender();
		m_draggingObject.reset();

	}
	else if (gameObject.expired()) {

		m_draggingObject = std::nullopt;
	}
	else {
		m_draggingObject = gameObject;
		m_draggingObject.value().lock()->disableRender();
	}

}

void Scene::resetGridDisplay()
{

	for (const auto& gameObject : m_gameObjects[GameLayer::GRID_DISPLAY]) {

		gameObject->disableRender();

	}

}

void Scene::direct()
{

	m_cutScene.value()->run(this);

}

void Scene::setCutScene(std::shared_ptr<CutScene>cutScene)
{

	m_cutScene = cutScene;

}

void Scene::deleteCutScene()
{

	m_cutScene.reset();

}

std::shared_ptr<GameObject> Scene::createGameObject(std::string gameObjectType, GameObject* parent, float xMapPos, float yMapPos, float angleAdjust, 
	Scene* parentScene,	GameLayer layer, bool cameraFollow, std::string name, b2Vec2 sizeOverride)
{

	std::shared_ptr<GameObject> gameObject = 
		std::make_shared<GameObject>(gameObjectType, parent, xMapPos, yMapPos, angleAdjust, parentScene, layer, cameraFollow, name, sizeOverride);

	//Add to the main gameObject lookup collection where every gameObject must live
	if (layer != GameLayer::GRID_DISPLAY) {
		m_gameObjectLookup.emplace(std::pair<std::string, std::shared_ptr<GameObject>>(gameObject->id(), gameObject));
	}

	//If this is the player object, then store it for easy access
	if (gameObject->hasTrait(TraitTag::player)) {

		m_player = gameObject;
	}

	return gameObject;
}

GameObject* Scene::addGameObject(std::string gameObjectType, GameObject* parent, GameLayer layer, float xMapPos, float yMapPos, float angle,
	bool cameraFollow, std::string name, b2Vec2 sizeOverride)
{

	std::shared_ptr<GameObject> gameObject = createGameObject(gameObjectType, parent, xMapPos, yMapPos, angle, this, layer, cameraFollow, 
		name, sizeOverride);

	m_gameObjects[layer].emplace_back(gameObject);
		
	return gameObject.get();

}

GameObject* Scene::addGameObject(std::string gameObjectType, GameObject* parent, GameLayer layer, PositionAlignment windowPosition, float adjustX, float adjustY,
	float angle, bool cameraFollow, std::string name, b2Vec2 sizeOverride)
{

	std::shared_ptr<GameObject> gameObject = createGameObject(gameObjectType, parent, (float)-5, (float)-5, angle, this, layer, cameraFollow, 
		name, sizeOverride);

	gameObject->setPosition(windowPosition, adjustX, adjustY);
	gameObject->setWindowRelativePosition(windowPosition, adjustX, adjustY);

	m_gameObjects[layer].emplace_back(gameObject);

	return gameObject.get();

}

void Scene::addGameObject(std::shared_ptr<GameObject> gameObject, GameLayer layer)
{

	gameObject->setParentScene(this);
	//gameObject->setLayer(layer);
	this->m_gameObjects[layer].push_back(gameObject);

	return;

}

void Scene::addGameObjectFromPool(std::shared_ptr<GameObject> gameObject, GameLayer layer)
{

	gameObject->setParentScene(this);
	this->m_gameObjects[layer].push_back(gameObject);

	//Add index 
	m_gameObjectLookup.emplace(std::pair<std::string, std::shared_ptr<GameObject>>(gameObject->id(), gameObject));

	return;

}

void Scene::addKeyAction(SDL_Keycode keyCode, SceneAction sceneAction)
{
	m_sceneKeyActions.emplace(keyCode, sceneAction);
}

void Scene::applyCurrentControlMode()
{

	setInputControlMode(m_inputControlMode);

}

std::optional<Parallax> Scene::getParallax(GameLayer layer)
{

	if (m_parallaxRates.find(layer) != m_parallaxRates.end()) {
		return m_parallaxRates.at(layer);
	}
	else {
		return std::nullopt;
	}

}


void Scene::clearEvents()
{

}

void Scene::setInputControlMode(int inputControlMode)
{

	m_inputControlMode = inputControlMode;

	if (inputControlMode == CONTROL_MODE_PLAY) {
		SDL_ShowCursor(false);
		SDL_SetRelativeMouseMode(SDL_TRUE);
	}
	else if (inputControlMode == CONTROL_MODE_SELECT) {
		SDL_ShowCursor(true);
		SDL_SetRelativeMouseMode(SDL_FALSE);
	}

}


void Scene::_processGameObjectInterdependecies()
{

	for (auto& gameObject : m_gameObjectLookup) {

		if (gameObject.second.expired() == false) {

			gameObject.second.lock()->postInit();

			//Now that all gameobjects are created we can set the shared pointer for the object to follow for the camera
			//ToDo:Maybe this should move to the postInit() of the gameObject
			if (gameObject.second.lock()->name() == Camera::instance().getFollowMeName() && !Camera::instance().getFollowMeObject()) {
				Camera::instance().setFollowMe(gameObject.second.lock());
			}

		}

	}

}

void Scene::_buildPhysicsWorld(Json::Value physicsJSON)
{

	m_physicsConfig.gravity = { physicsJSON["gravity"]["x"].asFloat(), physicsJSON["gravity"]["y"].asFloat() };
	m_physicsConfig.timeStep = physicsJSON["timeStep"].asFloat();
	m_physicsConfig.subSteps = physicsJSON["subSteps"].asInt();

	//Build the box2d physics world
	b2WorldDef worldDef = b2DefaultWorldDef();
	worldDef.gravity = m_physicsConfig.gravity;

	//enkit lets us setup a multithread situation for box2d to do its Step call much faster
	m_enki.configure(worldDef);

	//Create the box2d physics world
	m_physicsWorld = b2CreateWorld(&worldDef);

	//Add a collision contact filter for box2d callbacks
	b2World_SetCustomFilterCallback(m_physicsWorld, &Scene::_shouldCollide, this);
	
}

bool Scene::_shouldCollide(b2ShapeId shapeAId, b2ShapeId shapeBId, void* context)
{
	auto scene = static_cast<Scene*>(context);
	return game->contactFilter()->instance().ShouldCollide(shapeAId, shapeBId, scene);

}


void Scene::_buildSceneGameObjects(Json::Value definitionJSON)
{
	GameObject* gameObject{};

	for (Json::Value gameObjectJSON : definitionJSON["gameObjects"]) {

		std::string gameObjectType = gameObjectJSON["gameObjectType"].asString();
		assert(!gameObjectType.empty() && "Empty GameObejectType in Scene definition for scene ");

		GameLayer layer = (GameLayer)game->enumMap()->toEnum(gameObjectJSON["layer"].asString());

		//name
		std::string name = gameObjectJSON["name"].asString();

		//Determine location
		auto locationJSON = gameObjectJSON["location"];
		if (locationJSON.isMember("windowPosition")) {

			PositionAlignment windowPosition = static_cast<PositionAlignment>(game->enumMap()->toEnum(gameObjectJSON["location"]["windowPosition"].asString()));

			if (locationJSON.isMember("adjust")) {
				auto adjustX = locationJSON["adjust"]["x"].asFloat();
				auto adjustY = locationJSON["adjust"]["y"].asFloat();
				gameObject = addGameObject(gameObjectType, nullptr, layer, windowPosition, adjustX, adjustY, 0.,false,name);
			}
			else {
				gameObject = addGameObject(gameObjectType, nullptr, layer, windowPosition, 0., 0., 0., false, name);
			}
			
		}
		else {
			auto locationX = gameObjectJSON["location"]["x"].asFloat();
			auto locationY = gameObjectJSON["location"]["y"].asFloat();
			gameObject = addGameObject(gameObjectType, nullptr, layer, locationX, locationY, 0., false, name);
		}

	}
}

std::optional<std::shared_ptr<GameObject>> Scene::getGameObject(std::string id)
{
	std::optional<std::shared_ptr<GameObject>> foundGameObject{};


	auto search = m_gameObjectLookup.find(id);
	if (search != m_gameObjectLookup.end() && search->second.expired() == false) {
		foundGameObject = search->second.lock();
	}

	return foundGameObject;
}

std::optional<std::shared_ptr<GameObject>> Scene::extractGameObject(std::string id)
{
	std::optional<std::shared_ptr<GameObject>> foundGameObject{};

	auto deleteMeObject = createGameObject("DELETE_ME_OBJECT", nullptr, (float)-1.0, (float)-1.0, (float)0, this);
	deleteMeObject->setRemoveFromWorld(true);

	//Using a swap so that we contain all deletions of objects at the _removeFromWorldPass level
	//This allows us to move an object from the main world collection to a parent object and it can be called from a 
	//box2d callback
	for (auto& gameObjectsLayers : m_gameObjects) {

		for (auto& gameObject : gameObjectsLayers) {

			if (gameObject->id() == id) {

				foundGameObject = gameObject;
				gameObject.swap(deleteMeObject);
			}

		}
	}

	return foundGameObject;
}

std::vector<std::shared_ptr<GameObject>> Scene::getGameObjectsByName(std::string name)
{
	std::vector<std::shared_ptr<GameObject>> foundGameObjects;

	auto it = m_gameObjectLookup.begin();
	while (it != m_gameObjectLookup.end()) {

		if (it->second.expired() == false && it->second.lock()->name() == name) {
			foundGameObjects.push_back(it->second.lock());
		}

		++it;
	}

	return foundGameObjects;
}

std::optional<std::shared_ptr<GameObject>> Scene::getFirstGameObjectByName(std::string name)
{
	std::optional<std::shared_ptr<GameObject>> foundGameObject{};

	auto it = m_gameObjectLookup.begin();
	while (it != m_gameObjectLookup.end()) {

		if (it->second.expired() == false && it->second.lock()->name() == name) {
			foundGameObject = it->second.lock();
			break;
		}

		++it;
	}

	return foundGameObject;
}

std::vector<std::shared_ptr<GameObject>> Scene::getGameObjectsByTrait(int trait, bool includePooled)
{
	std::vector<std::shared_ptr<GameObject>> foundGameObjects;

	auto it = m_gameObjectLookup.begin();
	while (it != m_gameObjectLookup.end()) {

		if (it->second.expired() == false && it->second.lock()->hasTrait(trait) ) {

			if (includePooled == false && it->second.lock()->isOffGrid() ==false) {

				foundGameObjects.push_back(it->second.lock());
			}
		}

		++it;
	}

	return foundGameObjects;
}

std::optional<std::shared_ptr<GameObject>> Scene::getFirstGameObjectByTrait(int trait)
{
	std::optional<std::shared_ptr<GameObject>> foundGameObject{};

	auto it = m_gameObjectLookup.begin();
	while (it != m_gameObjectLookup.end()) {

		if (it->second.expired() == false && it->second.lock()->hasTrait(trait)) {
			foundGameObject = it->second.lock();
			break;
		}

		++it;
	}

	return foundGameObject;
}

std::optional<std::shared_ptr<GameObject>> Scene::getFirstGameObjectByType(std::string type)
{
	std::optional<std::shared_ptr<GameObject>> foundGameObject{};

	auto it = m_gameObjectLookup.begin();
	while (it != m_gameObjectLookup.end()) {

		if (it->second.expired() == false && it->second.lock()->type() == type) {
			foundGameObject = it->second.lock();
			break;
		}

		++it;
	}

	return foundGameObject;
}

void Scene::deleteIndex(std::string gameObjectKey)
{
	//std::cout << "Erased from lookup map" << gameObjectKey << std::endl;
	m_gameObjectLookup.erase(gameObjectKey);
	
}


void Scene::resetTrigger(std::string triggerName)
{

	for (auto trigger : m_levelTriggers) {

		if (trigger->getName() == triggerName) {

			trigger->reset();

		}
	}

}


void Scene::_removeFromWorldPass()
{
	//Loop through all layers and remove any gameObject that has been marked to remove
	for (auto& gameObjects : m_gameObjects) {

		auto it = gameObjects.begin();
		while (it != gameObjects.end()) {

			//Remove gameObject iteself if flagged
			if (it->get()->removeFromWorld() == true) {

				if (it->get()->hasTrait(TraitTag::pooled)) {

					it->get()->reset();

				}

				//std::cout << "Erased from Main collection " << it->get()->name() << std::endl;
				it = gameObjects.erase(it);
				
			}
			else {
				++it;
			}
		}

		gameObjects.shrink_to_fit();

	}

	//Also loop through entire lookup map and delete any expired weak pointers.
	//The strong pointers of the various gameObjects, some of which are dependents inside of other objects,
	//may have been deleted and their weak_ptr reference should be cleaned up in the lookup table also
	for (auto it = m_gameObjectLookup.cbegin(); it != m_gameObjectLookup.cend();)
	{
		if (it->second.expired() == true)
		{
			it = m_gameObjectLookup.erase(it);
		}
		else
		{
			++it;
		}
	}

}

void Scene::addLevelObjective(Objective objective)
{
	m_levelObjectives.emplace_back(objective);
}

void Scene::addLevelTrigger(std::shared_ptr<Trigger> trigger)
{
	m_levelTriggers.emplace_back(trigger);
}


void Scene::addParallaxItem(Parallax& parallaxItem)
{
	//we want to store these by index so they are retrieved on the render quickly
	m_parallaxRates.emplace(parallaxItem.layer, parallaxItem);

}

void Scene::updateGridDisplay(int xPos, int yPos, int operation, SDL_Color color)
{
	//ToDo:This can be changed to use multithreasing
	//use  std::foreach(std::execution::par, m_gameObjects[GameLayer::GRID_DISPLAY.begin, m_gameObjects[GameLayer::GRID_DISPLAY.end, 
    //[this](const std::shared_ptr<GameObject>)
	//{
	// put below code here
	//});

	for (const auto& gameObject : m_gameObjects[GameLayer::GRID_DISPLAY]) {

		int x = (int)gameObject->getOriginalTilePosition().x;
		int y = (int)gameObject->getOriginalTilePosition().y;

		if (x == xPos && y == yPos) {

			if (operation == TURN_OFF) {
				gameObject->disableRender();
			}
			else{
				gameObject->setColor(color);
				gameObject->enableRender();
			}

		}

	}

}



void Scene::_showNavigationMap()
{

	for (const auto& gameObject : m_gameObjects[GameLayer::GRID_DISPLAY]) {

		//Get the x,y object from the navigation map
		int x = (int)gameObject->getOriginalTilePosition().x;
		int y = (int)gameObject->getOriginalTilePosition().y;

		if (game->navigationManager()->navigationMap()[x][y].passable == false) {

			gameObject->enableRender();
				
		}
		else {
			gameObject->disableRender();
		}


	}

}

void Scene::stepB2PhysicsWorld() {

	b2World_Step(m_physicsWorld, m_physicsConfig.timeStep, 4);

	game->contactHandler()->handleContacts(m_physicsWorld);
	game->contactHandler()->handleSensors(m_physicsWorld);
	


}
