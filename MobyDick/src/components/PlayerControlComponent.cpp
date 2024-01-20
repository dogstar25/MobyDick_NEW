#include "PlayerControlComponent.h"


#include "../IMGui/IMGuiUtil.h"
#include "../SceneManager.h"
#include "../EnumMap.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

PlayerControlComponent::PlayerControlComponent() :
	Component(ComponentTypes::PLAYER_CONTROL_COMPONENT)
{

}

PlayerControlComponent::PlayerControlComponent(Json::Value componentJSON) :
	Component(ComponentTypes::PLAYER_CONTROL_COMPONENT)
{
	m_componentType = ComponentTypes::PLAYER_CONTROL_COMPONENT;

}

PlayerControlComponent::~PlayerControlComponent()
{

}






