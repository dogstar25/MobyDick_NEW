#include "EnumMap.h"
#include "hud/HudItemFactory.h"
#include "Renderer.h"
#include "opengl/GLRenderer.h"
#include "components/PhysicsComponent.h"
#include "ContextManager.h"


EnumMap::EnumMap()
{

	//Component Types
	m_enumMap["ACTION_COMPONENT"] = (int)ComponentTypes::ACTION_COMPONENT;
	m_enumMap["ANIMATION_COMPONENT"] = (int)ComponentTypes::ANIMATION_COMPONENT;
	m_enumMap["ATTACHMENTS_COMPONENT"] = (int)ComponentTypes::ATTACHMENTS_COMPONENT;
	m_enumMap["BRAIN_COMPONENT"] = (int)ComponentTypes::BRAIN_COMPONENT;
	m_enumMap["CHECKPOINT_COMPONENT"] = (int)ComponentTypes::CHECKPOINT_COMPONENT;
	m_enumMap["CHILDREN_COMPONENT"] = (int)ComponentTypes::CHILDREN_COMPONENT;
	m_enumMap["COMPOSITE_COMPONENT"] = (int)ComponentTypes::COMPOSITE_COMPONENT;
	m_enumMap["CONTAINER_COMPONENT"] = (int)ComponentTypes::CONTAINER_COMPONENT;
	m_enumMap["HUD_COMPONENT"] = (int)ComponentTypes::HUD_COMPONENT;
	m_enumMap["GRID_DISPLAY_COMPONENT"] = (int)ComponentTypes::GRID_DISPLAY_COMPONENT;
	m_enumMap["INVENTORY_COMPONENT"] = (int)ComponentTypes::INVENTORY_COMPONENT;
	m_enumMap["INTERFACE_COMPONENT"] = (int)ComponentTypes::INTERFACE_COMPONENT;
	m_enumMap["IMGUI_COMPONENT"] = (int)ComponentTypes::IMGUI_COMPONENT;
	m_enumMap["PARTICLE_COMPONENT"] = (int)ComponentTypes::PARTICLE_COMPONENT;
	m_enumMap["NAVIGATION_COMPONENT"] = (int)ComponentTypes::NAVIGATION_COMPONENT;
	m_enumMap["PARTICLE_X_COMPONENT"] = (int)ComponentTypes::PARTICLE_X_COMPONENT;
	m_enumMap["PHYSICS_COMPONENT"] = (int)ComponentTypes::PHYSICS_COMPONENT;
	m_enumMap["PLAYER_CONTROL_COMPONENT"] = (int)ComponentTypes::PLAYER_CONTROL_COMPONENT;
	m_enumMap["POOL_COMPONENT"] = (int)ComponentTypes::POOL_COMPONENT;
	m_enumMap["PUZZLE_COMPONENT"] = (int)ComponentTypes::PUZZLE_COMPONENT;
	m_enumMap["RENDER_COMPONENT"] = (int)ComponentTypes::RENDER_COMPONENT;
	m_enumMap["MASKED_OVERLAY_COMPONENT"] = (int)ComponentTypes::MASKED_OVERLAY_COMPONENT;
	m_enumMap["SOUND_COMPONENT"] = (int)ComponentTypes::SOUND_COMPONENT;
	m_enumMap["STATE_COMPONENT"] = (int)ComponentTypes::STATE_COMPONENT;
	m_enumMap["TEXT_COMPONENT"] = (int)ComponentTypes::TEXT_COMPONENT;
	m_enumMap["TRANSFORM_COMPONENT"] = (int)ComponentTypes::TRANSFORM_COMPONENT;
	m_enumMap["UICONTROL_COMPONENT"] = (int)ComponentTypes::UICONTROL_COMPONENT;
	m_enumMap["VITALITY_COMPONENT"] = (int)ComponentTypes::VITALITY_COMPONENT;
	m_enumMap["WEAPON_COMPONENT"] = (int)ComponentTypes::WEAPON_COMPONENT;
	m_enumMap["LIGHT_COMPONENT"] = (int)ComponentTypes::LIGHT_COMPONENT;
	m_enumMap["LIGHTED_TREATMENT_COMPONENT"] = (int)ComponentTypes::LIGHTED_TREATMENT_COMPONENT;
	m_enumMap["ENVIRONMENT_COMPONENT"] = (int)ComponentTypes::ENVIRONMENT_COMPONENT;


	//Renderer Types
	m_enumMap["RenderType::OPENGL"] = (int)RendererType::OPENGL;
	m_enumMap["RenderType::SDL"] = (int)RendererType::SDL;

	//Render Modes
	m_enumMap["RenderBlendMode::BLEND"] = (int)RenderBlendMode::BLEND;
	m_enumMap["RenderBlendMode::ADD"] = (int)RenderBlendMode::ADD;
	m_enumMap["RenderBlendMode::MULTIPLY"] = (int)RenderBlendMode::MULTIPLY;
	m_enumMap["RenderBlendMode::MODULATE"] = (int)RenderBlendMode::MODULATE;
	m_enumMap["RenderBlendMode::NONE"] = (int)RenderBlendMode::NONE;

	//Child slot treatment
	m_enumMap["ChildSlotTreatment::HORIZONTAL"] = (int)ChildSlotTreatment::HORIZONTAL;
	m_enumMap["ChildSlotTreatment::VERTICAL"] = (int)ChildSlotTreatment::VERTICAL;
	m_enumMap["ChildSlotTreatment::STACKED"] = (int)ChildSlotTreatment::STACKED;

	//Mouse Modes
	m_enumMap["CONTROL_MODE_SELECT"] = CONTROL_MODE_SELECT;
	m_enumMap["CONTROL_MODE_PLAY"] = CONTROL_MODE_PLAY;

	//Renderer Types
	m_enumMap["RendererType::SDL"] = (int)RendererType::SDL;
	m_enumMap["RendererType::OPENGL"] = (int)RendererType::OPENGL;

	//GameObject CollisionTags
	m_enumMap["ContactTag::GENERAL_SOLID"] = ContactTag::GENERAL_SOLID;
	m_enumMap["ContactTag::GENERAL_FREE"] = ContactTag::GENERAL_FREE;
	m_enumMap["ContactTag::LEVEL_CAGE"] = ContactTag::LEVEL_CAGE;

	//Box2d Related
	m_enumMap["B2_STATIC"] = b2_staticBody;
	m_enumMap["B2_KINEMATIC"] = b2_kinematicBody;
	m_enumMap["B2_DYNAMIC"] = b2_dynamicBody;
	m_enumMap["B2_CIRCLE"] = b2Shape::e_circle;
	m_enumMap["B2_BOX"] = b2Shape::e_polygon;
	m_enumMap["B2_CHAIN"] = b2Shape::e_chain;
	m_enumMap["B2_EDGE"] = b2Shape::e_edge;
	m_enumMap["B2_WELD"] = b2JointType::e_weldJoint;
	m_enumMap["B2_REVOLUTE"] = b2JointType::e_revoluteJoint;

	//Physics chain winding order to reflec outwards or inwards
	m_enumMap["PhysicsChainType::CCW_REFLECT_IN"] = (int)PhysicsChainType::CCW_REFLECT_IN;
	m_enumMap["PhysicsChainType::CW_REFLECT_OUT"] = (int)PhysicsChainType::CW_REFLECT_OUT;

	//Animation Modes
	m_enumMap["AnimationMode::ANIMATE_ONE_TIME"] = (int)AnimationMode::ANIMATE_ONE_TIME;
	m_enumMap["AnimationMode::ANIMATE_CONTINUOUS"] = (int)AnimationMode::ANIMATE_CONTINUOUS;
	m_enumMap["AnimationMode::ANIMATE_STILL_FRAME"] = (int)AnimationMode::ANIMATE_STILL_FRAME;

	//Player Control
	m_enumMap["INPUT_CONTROL_MOVEMENT"] = INPUT_CONTROL_MOVEMENT;
	m_enumMap["INPUT_CONTROL_USE"] = INPUT_CONTROL_USE;
	m_enumMap["INPUT_CONTROL_HOVER"] = INPUT_CONTROL_HOVER;
	m_enumMap["INPUT_CONTROL_CLICK"] = INPUT_CONTROL_CLICK;

	//Game Object Actions
	m_enumMap["Actions::NONE"] = Actions::NONE;
	m_enumMap["Actions::MOVE"] = Actions::MOVE;
	m_enumMap["Actions::SPRINT"] = Actions::SPRINT;
	m_enumMap["Actions::ROTATE"] = Actions::ROTATE;
	m_enumMap["Actions::USE"] = Actions::USE;
	m_enumMap["Actions::USAGE"] = Actions::USAGE;
	m_enumMap["Actions::INSPECT"] = Actions::INSPECT;
	m_enumMap["Actions::TALK"] = Actions::TALK;
	m_enumMap["Actions::PUSH"] = Actions::PUSH;
	m_enumMap["Actions::TAKE"] = Actions::TAKE;
	m_enumMap["Actions::COMBINE"] = Actions::COMBINE;
	m_enumMap["Actions::OPEN"] = Actions::OPEN;
	m_enumMap["Actions::CLOSE"] = Actions::CLOSE;
	m_enumMap["Actions::WARP"] = Actions::WARP;
	m_enumMap["Actions::ENTER"] = Actions::ENTER;
	m_enumMap["Actions::SHOW_INTERFACE"] = Actions::SHOW_INTERFACE;
	m_enumMap["Actions::HIDE_INTERFACE"] = Actions::HIDE_INTERFACE;
	m_enumMap["Actions::APPLY_HIGHLIGHT"] = Actions::APPLY_HIGHLIGHT;
	m_enumMap["Actions::REMOVE_HIGHLIGHT"] = Actions::REMOVE_HIGHLIGHT;
	m_enumMap["Actions::DROP"] = Actions::DROP;

	//User Events
	m_enumMap["InterfaceEvents::ON_TOUCHING"] = (int)InterfaceEvents::ON_TOUCHING;
	m_enumMap["InterfaceEvents::ON_STOP_TOUCHING"] = (int)InterfaceEvents::ON_STOP_TOUCHING;
	m_enumMap["InterfaceEvents::ON_HOVER"] = (int)InterfaceEvents::ON_HOVER;
	m_enumMap["InterfaceEvents::ON_HOVER_OUT"] = (int)InterfaceEvents::ON_HOVER_OUT;
	m_enumMap["InterfaceEvents::ON_LCLICK"] = (int)InterfaceEvents::ON_LCLICK;
	m_enumMap["InterfaceEvents::ON_RCLICK"] = (int)InterfaceEvents::ON_RCLICK;
	m_enumMap["InterfaceEvents::ON_DRAG"] = (int)InterfaceEvents::ON_DRAG;
	m_enumMap["InterfaceEvents::ON_DROP"] = (int)InterfaceEvents::ON_DROP;


	//Keys
	m_enumMap["Key::SDLK_ESCAPE"] = SDLK_ESCAPE;
	m_enumMap["Key::SDL_SCANCODE_W"] = SDL_SCANCODE_W;
	m_enumMap["Key::SDL_SCANCODE_A"] = SDL_SCANCODE_A;
	m_enumMap["Key::SDL_SCANCODE_S"] = SDL_SCANCODE_S;
	m_enumMap["Key::SDL_SCANCODE_D"] = SDL_SCANCODE_D;
	m_enumMap["Key::SDL_SCANCODE_I"] = SDL_SCANCODE_I;
	m_enumMap["Key::SDL_SCANCODE_F"] = SDL_SCANCODE_F;

	m_enumMap["Key::SDLK_F1"] = SDLK_F1;
	m_enumMap["Key::SDLK_F2"] = SDLK_F2;
	m_enumMap["Key::SDLK_F3"] = SDLK_F3;
	m_enumMap["Key::SDLK_F4"] = SDLK_F4;
	m_enumMap["Key::SDLK_F5"] = SDLK_F5;
	m_enumMap["Key::SDLK_F8"] = SDLK_F8;
	m_enumMap["Key::SDLK_F9"] = SDLK_F9;
	m_enumMap["Key::SDLK_F12"] = SDLK_F12;

	//Scene Action Codes
	m_enumMap["SCENE_ACTION_QUIT"] = SCENE_ACTION_QUIT;
	m_enumMap["SCENE_ACTION_ADD"] = SCENE_ACTION_ADD;
	m_enumMap["SCENE_ACTION_ADD_AND_PAUSE"] = SCENE_ACTION_ADD_AND_PAUSE;
	m_enumMap["SCENE_ACTION_REPLACE"] = SCENE_ACTION_REPLACE;
	m_enumMap["SCENE_ACTION_LOAD_LEVEL"] = SCENE_ACTION_LOAD_LEVEL;
	m_enumMap["SCENE_ACTION_LOAD_NEXTLEVEL"] = SCENE_ACTION_LOAD_NEXTLEVEL;
	m_enumMap["SCENE_ACTION_EXIT"] = SCENE_ACTION_EXIT;
	m_enumMap["SCENE_ACTION_DIRECT"] = SCENE_ACTION_DIRECT;
	m_enumMap["SCENE_ACTION_RELEASE_DIRECT"] = SCENE_ACTION_RELEASE_DIRECT;
	m_enumMap["SCENE_ACTION_TOGGLE_SETTING"] = SCENE_ACTION_TOGGLE_SETTING;
	m_enumMap["SCENE_ACTION_QUICK_SAVE"] = SCENE_ACTION_QUICK_SAVE;
	m_enumMap["SCENE_ACTION_QUICK_LOAD"] = SCENE_ACTION_QUICK_LOAD;
	m_enumMap["SCENE_ACTION_START_NEW"] = SCENE_ACTION_START_NEW;

	//Scene Tags
	m_enumMap["SCENETAG_MENU"] = SCENETAG_MENU;

	//Object Alignments
	m_enumMap["PositionAlignment::TOP_LEFT"] = (int)PositionAlignment::TOP_LEFT;
	m_enumMap["PositionAlignment::TOP_CENTER"] = (int)PositionAlignment::TOP_CENTER;
	m_enumMap["PositionAlignment::TOP_RIGHT"] = (int)PositionAlignment::TOP_RIGHT;
	m_enumMap["PositionAlignment::CENTER_LEFT"] = (int)PositionAlignment::CENTER_LEFT;
	m_enumMap["PositionAlignment::CENTER"] = (int)PositionAlignment::CENTER;
	m_enumMap["PositionAlignment::CENTER_RIGHT"] = (int)PositionAlignment::CENTER_RIGHT;
	m_enumMap["PositionAlignment::BOTTOM_LEFT"] = (int)PositionAlignment::BOTTOM_LEFT;
	m_enumMap["PositionAlignment::BOTTOM_CENTER"] = (int)PositionAlignment::BOTTOM_CENTER;
	m_enumMap["PositionAlignment::BOTTOM_RIGHT"] = (int)PositionAlignment::BOTTOM_RIGHT;

	//Game Layers
	m_enumMap["GameLayer::BACKGROUND_FINAL"] = GameLayer::BACKGROUND_FINAL;
	m_enumMap["GameLayer::BACKGROUND_5"] = GameLayer::BACKGROUND_5;
	m_enumMap["GameLayer::BACKGROUND_4"] = GameLayer::BACKGROUND_4;
	m_enumMap["GameLayer::BACKGROUND_3"] = GameLayer::BACKGROUND_3;
	m_enumMap["GameLayer::BACKGROUND_2"] = GameLayer::BACKGROUND_2;
	m_enumMap["GameLayer::BACKGROUND_1"] = GameLayer::BACKGROUND_1;
	m_enumMap["GameLayer::MAIN"] = GameLayer::MAIN;
	m_enumMap["GameLayer::FOREGROUND_1"] = GameLayer::FOREGROUND_1;
	m_enumMap["GameLayer::FOREGROUND_2"] = GameLayer::FOREGROUND_2;
	m_enumMap["GameLayer::FOREGROUND_3"] = GameLayer::FOREGROUND_3;
	m_enumMap["GameLayer::FOREGROUND_4"] = GameLayer::FOREGROUND_4;
	m_enumMap["GameLayer::FOREGROUND_5"] = GameLayer::FOREGROUND_5;
	m_enumMap["GameLayer::GUI_1"] = GameLayer::GUI_1;
	m_enumMap["GameLayer::GUI_2"] = GameLayer::GUI_2;
	m_enumMap["GameLayer::GUI_3"] = GameLayer::GUI_3;
	m_enumMap["GameLayer::ABSTRACT"] = GameLayer::ABSTRACT;
	m_enumMap["GameLayer::GRID_DISPLAY"] = GameLayer::GRID_DISPLAY;


	//Game Object Display Modes
	m_enumMap["DISPLAY_UI_MODE_STANDARD"] = DISPLAY_UI_MODE_STANDARD;
	m_enumMap["DISPLAY_UI_MODE_ONHOVER"] = DISPLAY_UI_MODE_ONHOVER;
	m_enumMap["DISPLAY_UI_MODE_ONCLICK"] = DISPLAY_UI_MODE_ONCLICK;

	//Particle Emitter types
	m_enumMap["ParticleEmitterType::ONETIME"] = ParticleEmitterType::ONETIME;
	m_enumMap["ParticleEmitterType::CONTINUOUS"] = ParticleEmitterType::CONTINUOUS;

	//OpenGL Texture Index values
	m_enumMap["GL_TextureIndexType::MAIN_TEXTURE_ATLAS_0"] = (int)GL_TextureIndexType::MAIN_TEXTURE_ATLAS_0;
	m_enumMap["GL_TextureIndexType::MAIN_TEXTURE_ATLAS_1"] = (int)GL_TextureIndexType::MAIN_TEXTURE_ATLAS_1;
	m_enumMap["GL_TextureIndexType::MAIN_TEXTURE_ATLAS_2"] = (int)GL_TextureIndexType::MAIN_TEXTURE_ATLAS_2;
	m_enumMap["GL_TextureIndexType::MAIN_TEXTURE_ATLAS_3"] = (int)GL_TextureIndexType::MAIN_TEXTURE_ATLAS_3;
	m_enumMap["GL_TextureIndexType::MAIN_TEXTURE_ATLAS_4"] = (int)GL_TextureIndexType::MAIN_TEXTURE_ATLAS_4;
	m_enumMap["GL_TextureIndexType::IMGUI_TEXTURE_ATLAS"] = (int)GL_TextureIndexType::IMGUI_TEXTURE_ATLAS;
	
	//Texure Blend Modes
	//m_enumMap["SDL_BLENDMODE_BLEND"] = SDL_BLENDMODE_BLEND;
	//m_enumMap["SDL_BLENDMODE_ADD"] = SDL_BLENDMODE_ADD;
	//m_enumMap["SDL_BLENDMODE_NONE"] = SDL_BLENDMODE_NONE;

	//GameObject Types
	//m_enumMap["GameObjectType::ABSTRACT"] = GameObjectType::ABSTRACT;
	//m_enumMap["GameObjectType::SPRITE"] = GameObjectType::SPRITE;
	//m_enumMap["GameObjectType::LINE"] = GameObjectType::LINE;
	//m_enumMap["GameObjectType::POINT"] = GameObjectType::POINT;
	//m_enumMap["GameObjectType::RECTANGLE"] = GameObjectType::RECTANGLE;

	//GameObject Trait Tags
	m_enumMap["player"] = TraitTag::player;
	m_enumMap["barrier"] = TraitTag::barrier;
	m_enumMap["collectable"] = TraitTag::collectable;
	m_enumMap["weapon"] = TraitTag::weapon;
	m_enumMap["gui"] = TraitTag::gui;
	m_enumMap["waypoint"] = TraitTag::waypoint;
	m_enumMap["abstract"] = TraitTag::abstract;
	m_enumMap["always_in_line_of_sight"] = TraitTag::always_in_line_of_sight;
	m_enumMap["mouse_interface"] = TraitTag::mouse_interface;
	m_enumMap["debug"] = TraitTag::debug;
	m_enumMap["pooled"] = TraitTag::pooled;
	m_enumMap["fragment"] = TraitTag::fragment;
	m_enumMap["objective"] = TraitTag::objective;
	m_enumMap["impasse"] = TraitTag::impasse;
	m_enumMap["conditional_impasse"] = TraitTag::conditional_impasse;
	m_enumMap["complex_impasse"] = TraitTag::complex_impasse;
	m_enumMap["mobile"] = TraitTag::mobile;
	m_enumMap["puzzle"] = TraitTag::puzzle;
	m_enumMap["vertical_movement_zone"] = TraitTag::vertical_movement_zone;
	m_enumMap["inventory"] = TraitTag::inventory;
	m_enumMap["puzzle_item"] = TraitTag::puzzle_item;
	m_enumMap["door"] = TraitTag::door;
	m_enumMap["inventory_player"] = TraitTag::inventory_player;
	m_enumMap["inventory_open"] = TraitTag::inventory_open;
	m_enumMap["inventory_closed"] = TraitTag::inventory_closed;
	m_enumMap["inventory_display"] = TraitTag::inventory_display;
	m_enumMap["door_side"] = TraitTag::door_side;
	m_enumMap["door_front"] = TraitTag::door_front;
	m_enumMap["receptacle"] = TraitTag::receptacle;
	m_enumMap["save_me"] = TraitTag::save_me;
	m_enumMap["toggle_switch"] = TraitTag::toggle_switch;
	m_enumMap["light_emission"] = TraitTag::light_emission;


	//Hud Item Types
	m_enumMap["HudItemTypes::STATUS_SINGLE"] = (int)HudItemTypes::STATUS_SINGLE;
	m_enumMap["HudItemTypes::STATUS_SERIES"] = (int)HudItemTypes::STATUS_SERIES;

	m_enumMap["LightType::TEXTURE_LIGHT"] = (int)LightType::TEXTURE_LIGHT;
	m_enumMap["LightType::SHADER_LIGHT"] = (int)LightType::SHADER_LIGHT;
	m_enumMap["LightType::RAY_LIGHT"] = (int)LightType::RAY_LIGHT;

	//Scene Settings
	m_enumMap["DebugSceneSettings::SHOW_PHYSICS_DEBUG"] = DebugSceneSettings::SHOW_PHYSICS_DEBUG;
	m_enumMap["DebugSceneSettings::SHOW_NAVIGATION_DEBUG_MAP"] = DebugSceneSettings::SHOW_NAVIGATION_DEBUG_MAP;

	//Scene Settings
	m_enumMap["NavigationSizeCategory::SMALL"] = NavigationSizeCategory::SMALL;
	m_enumMap["NavigationSizeCategory::MEDIUM"] = NavigationSizeCategory::MEDIUM;
	m_enumMap["NavigationSizeCategory::LARGE"] = NavigationSizeCategory::LARGE;

	//Condition Operators
	m_enumMap["ConditionOperator::AND"] = (int)ConditionOperator::AND;
	m_enumMap["ConditionOperator::OR"] = (int)ConditionOperator::OR;

	//GameObject States
	m_enumMap["GameObjectState::ON"] = (int)GameObjectState::ON;
	m_enumMap["GameObjectState::OFF"] = (int)GameObjectState::OFF;
	m_enumMap["GameObjectState::OPENED"] = (int)GameObjectState::OPENED;
	m_enumMap["GameObjectState::CLOSED"] = (int)GameObjectState::CLOSED;
	m_enumMap["GameObjectState::IDLE"] = (int)GameObjectState::IDLE;
	m_enumMap["GameObjectState::WALK"] = (int)GameObjectState::WALK;
	m_enumMap["GameObjectState::RUN"] = (int)GameObjectState::RUN;
	m_enumMap["GameObjectState::SPRINT"] = (int)GameObjectState::SPRINT;
	m_enumMap["GameObjectState::JUMP"] = (int)GameObjectState::JUMP;
	m_enumMap["GameObjectState::CLIMB"] = (int)GameObjectState::CLIMB;
	m_enumMap["GameObjectState::DEAD"] = (int)GameObjectState::DEAD;
	m_enumMap["GameObjectState::DISABLED_UPDATE"] = (int)GameObjectState::DISABLED_UPDATE;
	m_enumMap["GameObjectState::DISABLED_PHYSICS"] = (int)GameObjectState::DISABLED_PHYSICS;
	m_enumMap["GameObjectState::DISABLED_RENDER"] = (int)GameObjectState::DISABLED_RENDER;
	m_enumMap["GameObjectState::DISABLED_COLLISION"] = (int)GameObjectState::DISABLED_COLLISION;
	m_enumMap["GameObjectState::DEPLOYED"] = (int)GameObjectState::DEPLOYED;
	m_enumMap["GameObjectState::CONCEALED"] = (int)GameObjectState::CONCEALED;
	m_enumMap["GameObjectState::IMPASSABLE"] = (int)GameObjectState::IMPASSABLE;
	m_enumMap["GameObjectState::EQUIPPED"] = (int)GameObjectState::EQUIPPED;
	m_enumMap["GameObjectState::ITEM_OBTAINABLE"] = (int)GameObjectState::ITEM_OBTAINABLE;
	m_enumMap["GameObjectState::ITEM_LOOSE"] = (int)GameObjectState::ITEM_LOOSE;
	m_enumMap["GameObjectState::ITEM_STORED_ENCLOSED"] = (int)GameObjectState::ITEM_STORED_ENCLOSED;
	m_enumMap["GameObjectState::ITEM_STORED_OPEN"] = (int)GameObjectState::ITEM_STORED_OPEN;
	m_enumMap["GameObjectState::ITEM_STORED_PLAYER"] = (int)GameObjectState::ITEM_STORED_PLAYER;
	m_enumMap["GameObjectState::IDLE_RIGHT"] = (int)GameObjectState::IDLE_RIGHT;
	m_enumMap["GameObjectState::IDLE_LEFT"] = (int)GameObjectState::IDLE_LEFT;
	m_enumMap["GameObjectState::IDLE_UP"] = (int)GameObjectState::IDLE_UP;
	m_enumMap["GameObjectState::IDLE_DOWN"] = (int)GameObjectState::IDLE_DOWN;
	m_enumMap["GameObjectState::WALK_RIGHT"] = (int)GameObjectState::WALK_RIGHT;
	m_enumMap["GameObjectState::WALK_LEFT"] = (int)GameObjectState::WALK_LEFT;
	m_enumMap["GameObjectState::WALK_UP"] = (int)GameObjectState::WALK_UP;
	m_enumMap["GameObjectState::WALK_DOWN"] = (int)GameObjectState::WALK_DOWN;
	m_enumMap["GameObjectState::IDLE_RIGHT_EQUIPPED"] = (int)GameObjectState::IDLE_RIGHT_EQUIPPED;
	m_enumMap["GameObjectState::IDLE_LEFT_EQUIPPED"] = (int)GameObjectState::IDLE_LEFT_EQUIPPED;
	m_enumMap["GameObjectState::IDLE_UP_EQUIPPED"] = (int)GameObjectState::IDLE_UP_EQUIPPED;
	m_enumMap["GameObjectState::IDLE_DOWN_EQUIPPED"] = (int)GameObjectState::IDLE_DOWN_EQUIPPED;
	m_enumMap["GameObjectState::WALK_RIGHT_EQUIPPED"] = (int)GameObjectState::WALK_RIGHT_EQUIPPED;
	m_enumMap["GameObjectState::WALK_LEFT_EQUIPPED"] = (int)GameObjectState::WALK_LEFT_EQUIPPED;
	m_enumMap["GameObjectState::WALK_UP_EQUIPPED"] = (int)GameObjectState::WALK_UP_EQUIPPED;
	m_enumMap["GameObjectState::WALK_DOWN_EQUIPPED"] = (int)GameObjectState::WALK_DOWN_EQUIPPED;

	m_enumMap["GameObjectState::RUN_RIGHT"] = (int)GameObjectState::RUN_RIGHT;
	m_enumMap["GameObjectState::RUN_LEFT"] = (int)GameObjectState::RUN_LEFT;
	m_enumMap["GameObjectState::RUN_UP"] = (int)GameObjectState::RUN_UP;
	m_enumMap["GameObjectState::RUN_DOWN"] = (int)GameObjectState::RUN_DOWN;
	m_enumMap["GameObjectState::DRAGGABLE"] = (int)GameObjectState::DRAGGABLE;


}

EnumMap::~EnumMap()
{
	m_enumMap.clear();
}

const int EnumMap::toEnum(std::string name)
{
	assert(m_enumMap.find(name) != m_enumMap.end() && "Constant Name wasnt found in EnumMap");

	return m_enumMap[name];
}

std::string EnumMap::findKeyWithValueHint(int value, std::string nameHint)
{

	for (const auto& pair : m_enumMap) {
		if (pair.first.find(nameHint) == 0 && value == pair.second) {

			return pair.first;

		}
	}

	return "NOT_FOUND";
}


