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

	m_actions.resize(32);
	for (Json::Value itrAction: componentJSON["actions"])
	{
		//Get the name of the class to be used as the action as a string
		std::string actionClass = itrAction["actionClass"].asString();

		//Get the Enum that represents the Game Objects action as an int
		int actionId = game->enumMap()->toEnum(itrAction["actionId"].asString());

		m_actions[actionId] = game->actionFactory()->create(actionClass);

		//Label
		if (itrAction.isMember("label")) {

			m_actions[actionId]->setLabel(itrAction["label"].asString());

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


// Serialization and Deserialization
namespace Json {

	template<>
	void serialize<ActionComponent>(Json::Value& value, ActionComponent& o) {

		Json::Value baseComponentValue;
		Json::serialize(baseComponentValue, static_cast<const Component&>(o));
		value["component"] = baseComponentValue;

		for (const auto& action : o.m_actions) {

			Json::Value actionValue;

			Json::serialize(actionValue, *action);
			value["actions"].append(actionValue);

		}
	}

	template<>
	void deserialize<ActionComponent>(Json::Value& value, ActionComponent& o) {

		Json::Value& actionsArray = value["actions"];

		for (auto& actionValue : actionsArray) {

			std::shared_ptr<Action> action = std::make_shared<Action>(); // Assuming Action is a class

			Json::deserialize(actionValue, *action);
			o.m_actions.push_back(action);
		}
	}
}


