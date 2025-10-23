#include "PlayerControlComponent.h"


#include "../IMGui/IMGuiUtil.h"
#include "../SceneManager.h"
#include "../EnumMap.h"
#include "../game.h"
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game> game;

//PlayerControlComponent::PlayerControlComponent() :
//	Component(ComponentTypes::PLAYER_CONTROL_COMPONENT)
//{
//
//}

PlayerControlComponent::PlayerControlComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::PLAYER_CONTROL_COMPONENT, parent)
{
	m_componentType = ComponentTypes::PLAYER_CONTROL_COMPONENT;

}

PlayerControlComponent::~PlayerControlComponent()
{

}






