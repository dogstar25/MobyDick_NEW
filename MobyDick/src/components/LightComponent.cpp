#include "LightComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

LightComponent::LightComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::LIGHT_COMPONENT;

	m_lightType = static_cast<LightType>(game->enumMap()->toEnum(componentJSON["type"].asString()));
	m_intensity = componentJSON["intensity"].asFloat();
	m_flicker = componentJSON["flicker"].asFloat();

}


void LightComponent::update()
{

}