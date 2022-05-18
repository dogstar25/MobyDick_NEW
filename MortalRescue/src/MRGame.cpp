#include "MRGame.h"

#include "GameConstants.h"
#include "GameObjectManager.h"
#include "SoundManager.h"
#include "LevelManager.h"
#include "MRContextManager.h"
#include "Camera.h"
#include "Clock.h"
#include "ContactFilter.h"
#include "ContactListener.h"
#include "components/DroneBrainComponent.h"
#include "IMGui/IMGuiUtil.h"

#include "EnumMaps.h"
#include "ColorMap.h"

#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_sdlrenderer.h"

using namespace std::chrono_literals;

MRGame::MRGame()
{

}

MRGame::~MRGame()
{

}

/*
Initialize Game
*/
bool MRGame::init(ContactListener* contactListener, ContactFilter* contactFilter, 
	ComponentFactory* componentFactory, ActionFactory* actionFactory, ParticleEffectsFactory* particleEffectsFactory, 
	CutSceneFactory* cutSceneFactory, IMGuiFactory* iMGuiFactory, ContextManager* contextManager)
{

	//Get all of the configuration values
	GameConfig::instance().init("gameConfig");

	Game::init(contactListener, contactFilter, componentFactory, actionFactory, particleEffectsFactory, cutSceneFactory, iMGuiFactory, contextManager);

	std::cout << "Mortal Rescue Begins\n";

	m_gameState = GameState::PLAY;

	//Add Game Specific stuff
	_addGameCollisionTags();
	_addGameComponentTypes();
	_addGameColors();
	_addGameTraits();

	//Initialize world
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0)
	{

		//Init font library
		TTF_Init();

		//Create the game window
		uint16 windowFlags = 0 | SDL_WINDOW_OPENGL;
		if (GameConfig::instance().windowFullscreen() == true) 
		{
			windowFlags = windowFlags | SDL_WINDOW_FULLSCREEN;
		}
		else
		{
			windowFlags = windowFlags | SDL_WINDOW_RESIZABLE;
		}
		m_window = SDL_CreateWindow(GameConfig::instance().gameTitle().c_str(),
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			GameConfig::instance().windowWidth(),
			GameConfig::instance().windowHeight(),
			windowFlags);

		//Initialize the texture manager
		m_renderer->init(m_window);

		//Test for IMGUI
		ImGui::MobyDickInit(this);

		//Display basic loading message
		_displayLoadingMsg();

		TextureManager::instance().init();
		TextureManager::instance().load("textureAtlasAssets");

		//Initialize the SceneManager
		SceneManager::instance().init();
		SceneManager::instance().load("gameScenes");

		_displayLoadingMsg();

		//Initialize the Game Object Manager
		GameObjectManager::instance().init();
		GameObjectManager::instance().load("gameObjectDefinitions/commonObjects");
		GameObjectManager::instance().load("gameObjectDefinitions/wallObjects");
		GameObjectManager::instance().load("gameObjectDefinitions/particleObjects");
		GameObjectManager::instance().load("gameObjectDefinitions/compositeObjects");
		GameObjectManager::instance().load("gameObjectDefinitions/guiObjects");
		GameObjectManager::instance().load("gameObjectDefinitions/hudObjects");

		_displayLoadingMsg();

		//Load a first scene
		Scene& scene = SceneManager::instance().pushScene("SCENE_PLAY");
		scene.loadLevel("level1");

		//Initialize the sound manager
		SoundManager::instance().initSound();
		SoundManager::instance().playMusic("music_ambience_1", -1);

		//Initialize the clock object
		Clock::instance().init();

		//scene.addGameObject("PARTICLE_EMITTER_SPARK", LAYER_MENU, 13, 13);
		//scene.addGameObject("BOWMAN", LAYER_MENU, 3, 3);


	}

	return true;
}


void MRGame::_addGameCollisionTags()
{

	//Add all game specific enum traslations needed
	EnumMap::instance().addEnumItem("ContactTag::BUTTON", ContactTag::BUTTON);
	EnumMap::instance().addEnumItem("ContactTag::DEFLECT_EFFECT", ContactTag::DEFLECT_EFFECT);
	EnumMap::instance().addEnumItem("ContactTag::DRONE_BRAIN", ContactTag::DRONE_BRAIN);
	EnumMap::instance().addEnumItem("ContactTag::DRONE_SHIELD", ContactTag::DRONE_SHIELD);
	EnumMap::instance().addEnumItem("ContactTag::DRONE_WEAPON", ContactTag::DRONE_WEAPON);
	EnumMap::instance().addEnumItem("ContactTag::DRONE_FRAME", ContactTag::DRONE_FRAME);
	EnumMap::instance().addEnumItem("ContactTag::ENEMY_BULLET", ContactTag::ENEMY_BULLET);
	EnumMap::instance().addEnumItem("ContactTag::FRIENDLY_BULLET", ContactTag::FRIENDLY_BULLET);
	EnumMap::instance().addEnumItem("ContactTag::HEAVY_PARTICLE", ContactTag::HEAVY_PARTICLE);
	EnumMap::instance().addEnumItem("ContactTag::LIGHT_PARTICLE", ContactTag::LIGHT_PARTICLE);
	EnumMap::instance().addEnumItem("ContactTag::PLAYER_COLLISION", ContactTag::PLAYER_COLLISION);;
	EnumMap::instance().addEnumItem("ContactTag::PLAYER_TOUCH", ContactTag::PLAYER_TOUCH);;
	EnumMap::instance().addEnumItem("ContactTag::PLAYER_HITBOX", ContactTag::PLAYER_HITBOX);;
	EnumMap::instance().addEnumItem("ContactTag::PLAYER_BULLET", ContactTag::PLAYER_BULLET);
	EnumMap::instance().addEnumItem("ContactTag::SHIELD_SCRAP", ContactTag::SHIELD_SCRAP);
	EnumMap::instance().addEnumItem("ContactTag::SURVIVOR", ContactTag::SURVIVOR);
	EnumMap::instance().addEnumItem("ContactTag::MEDKIT", ContactTag::MEDKIT);
	EnumMap::instance().addEnumItem("ContactTag::WALL", ContactTag::WALL);
	EnumMap::instance().addEnumItem("ContactTag::WEAPON_PICKUP", ContactTag::WEAPON_PICKUP);
	EnumMap::instance().addEnumItem("ContactTag::ESCAPE_STAIRS", ContactTag::ESCAPE_STAIRS);
	EnumMap::instance().addEnumItem("ContactTag::WALL_PIECE", ContactTag::WALL_PIECE);



}

void MRGame::_addGameComponentTypes()
{
	EnumMap::instance().addEnumItem("BRAIN_DRONE_COMPONENT", ComponentTypes::BRAIN_DRONE_COMPONENT);
	EnumMap::instance().addEnumItem("WEAPON_PISTOL_COMPONENT", ComponentTypes::WEAPON_PISTOL_COMPONENT);
	EnumMap::instance().addEnumItem("TURRET_BRAIN_COMPONENT", ComponentTypes::TURRET_BRAIN_COMPONENT);
	EnumMap::instance().addEnumItem("SURVIVOR_BRAIN_COMPONENT", ComponentTypes::SURVIVOR_BRAIN_COMPONENT);
}

void MRGame::_addGameTraits()
{
	EnumMap::instance().addEnumItem("escape", TraitTag::escape);

}

void MRGame::_addGameColors()
{

	//greens
	ColorMap::instance().addColor("Colors::FOREST", Colors::FOREST);
	ColorMap::instance().addColor("Colors::EMERALD", Colors::EMERALD);
	ColorMap::instance().addColor("Colors::PARAKEET", Colors::PARAKEET);

	//reds
	ColorMap::instance().addColor("Colors::WINE", Colors::WINE);

	//yellows
	ColorMap::instance().addColor("Colors::GOLD", Colors::GOLD);

	//blues
	ColorMap::instance().addColor("Colors::CYAN", Colors::CYAN);
	ColorMap::instance().addColor("Colors::MARINE", Colors::MARINE);
	ColorMap::instance().addColor("Colors::SAPPHIRE", Colors::SAPPHIRE);

	//Dark
	ColorMap::instance().addColor("Colors::CHARCOAL", Colors::CHARCOAL);
	ColorMap::instance().addColor("Colors::MIRKWOOD", Colors::MIRKWOOD);



}


