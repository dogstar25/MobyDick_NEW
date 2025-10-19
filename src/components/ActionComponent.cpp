#include "ActionComponent.h"

#include <memory.h>

#include "../EnumMap.h"
#include "../game.h"
#include "../Util.h"

#include "../actions/DefaultMoveAction.h"
#include "../actions/DefaultRotateAction.h"
#include "../actions/DefaultOnHoverAction.h"
#include "../actions/DefaultOnHoverOutAction.h"
#include "../IMGui/IMGuiItem.h"

extern std::unique_ptr<Game> game;

ActionComponent::ActionComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::ACTION_COMPONENT, parent)
{

	for (Json::Value itrAction: componentJSON["actions"])
	{

		//Get the name of the class to be used as the action as a string
		std::string actionClass = itrAction["actionClass"].asString();

		//Get the Enum that represents the Game Objects action as an int
		int actionId = game->enumMap()->toEnum(itrAction["actionId"].asString());

		ActionType actionType{};
		if (itrAction.isMember("actionType")) {
			ActionType actionType = (ActionType)game->enumMap()->toEnum(itrAction["actionType"].asString());
		}
		else {
			ActionType actionType = ActionType::DIRECT;
		}

		Json::Value properties{};
		if (itrAction.isMember("properties")) {

			properties = itrAction["properties"];

		}
		m_actions[actionId] = game->actionFactory()->create(actionClass, properties, parent);

		//Label
		if (itrAction.isMember("label")) {

			m_actions[actionId]->setLabel(itrAction["label"].asString());

		}

	}

}

void ActionComponent::update()
{

	for (auto actionItr = m_actions.begin(); actionItr != m_actions.end(); ++actionItr) {

		if (actionItr->second->status() == ProgressionStatus::IN_PROGRESS) {
			actionItr->second->update();
		}

	}


}

std::shared_ptr<Action> ActionComponent::getAction(int actionId)
{

	return m_actions[actionId];
}

bool ActionComponent::hasAction(int actionId)
{

	if (m_actions.at(actionId)) {
		return true;
	}

	return false;
}


