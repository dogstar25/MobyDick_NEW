#include "HudComponent.h"

#include "../hud/HudItemFactory.h"
#include "../EnumMap.h"
#include "../hud/HudItem.h"
#include "../game.h"
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game> game;

HudComponent::HudComponent(Json::Value componentJSON, GameObject* parent, Scene* parentScene) :
	Component(ComponentTypes::HUD_COMPONENT, parent)
{

	std::string labelObjectId = componentJSON["labelObjectId"].asString();
	std::string statusObjectId = componentJSON["statusObjectId"].asString();
	std::string statusValueId = componentJSON["statusValueId"].asString();
	float labelPadding = componentJSON["labelPadding"].asFloat();
	HudItemTypes type = static_cast<HudItemTypes>(game->enumMap()->toEnum(componentJSON["hudType"].asString()));

	m_isDependentObjectOwner = true;

	m_hudItem = HudItemFactory::instance().create(type, parent, labelObjectId, statusObjectId, statusValueId, labelPadding, parentScene);
}


void HudComponent::update()
{

	//Execute specific HudClassType updates
	m_hudItem->update(parent());


}

void HudComponent::render()
{

	m_hudItem->render(parent());

}