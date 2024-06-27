#include "OnOffToggleAction.h"

#include "../GameObject.h"

void OnOffToggleAction::perform(GameObject* gameObject)
{

	GameObjectState stateToPropogate{};

	//Flip the switch on/off state
	if (gameObject->hasState(GameObjectState::ON)) {

		gameObject->addState(GameObjectState::OFF);
		stateToPropogate = GameObjectState::OFF;
	}
	else {

		gameObject->addState(GameObjectState::ON);
		stateToPropogate = GameObjectState::ON;
	}


	//The default is to find all objects that match the _TARGET name and flip thie ON/OFF
	//If none are found then we will fallback to toggle any sibling light fixtures
	std::string buttonTargetObjectName = gameObject->name() + "_TARGET";
	auto targetObjects = gameObject->parentScene()->getGameObjectsByName(buttonTargetObjectName);
	if (targetObjects.empty() == false) {

		for (auto& targetObject : targetObjects) {

			targetObject->addState(stateToPropogate);

		}

	}
	else {

		if (gameObject->parent().has_value()){

			//gameObject->parent().value()->addState(stateToPropogate);
			gameObject->addState(stateToPropogate);

		}

	}

}



