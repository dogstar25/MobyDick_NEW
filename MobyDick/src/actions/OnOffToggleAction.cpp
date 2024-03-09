#include "OnOffToggleAction.h"

#include "../GameObject.h"

void OnOffToggleAction::perform(GameObject* gameObject)
{

	std::string buttonTargetObjectName = gameObject->name() + "_TARGET";
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

	//Flip the target object or objects on/off state
	auto targetObjects = gameObject->parentScene()->getGameObjectsByName(buttonTargetObjectName);
	assert(!targetObjects.empty() && "GameObject wasnt found!");

	for (auto& targetObject : targetObjects) {

		if (targetObject->hasState(GameObjectState::ON)) {

			targetObject->addState(GameObjectState::OFF);
		}
		else if (targetObject->hasState(GameObjectState::OFF)) {

			targetObject->addState(GameObjectState::ON);
		}

		//turn off state of all children as well
		util::propogateStateToAllChildren(targetObject.get(), stateToPropogate);

	}

}



