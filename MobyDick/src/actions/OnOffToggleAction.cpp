#include "OnOffToggleAction.h"
#include "../components/StateComponent.h"
#include "../GameObject.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

void OnOffToggleAction::perform(GameObject* gameObject)
{

	GameObjectState stateToPropogate{};

	//If we dont want to toggle, but set the on/OFF to what is passed in as a property, get the property
	if (hasActionProperty("forcedState")) {

		auto property = getActionProperty("forcedState");
		stateToPropogate = (GameObjectState)game->enumMap()->toEnum(property.asString());

	}
	else {

		//Flip the switch on/off state
		if (gameObject->hasState(GameObjectState::ON)) {

			gameObject->addState(GameObjectState::OFF);
			stateToPropogate = GameObjectState::OFF;
		}
		else {

			gameObject->addState(GameObjectState::ON);
			stateToPropogate = GameObjectState::ON;
		}

	}

	if (gameObject->hasTrait(TraitTag::toggle_switch)) {


		std::string buttonTargetObjectName = gameObject->name() + "_TARGET";
		auto targetObjects = gameObject->parentScene()->getGameObjectsByName(buttonTargetObjectName);
		if (targetObjects.empty() == false) {


			for (auto& targetObject : targetObjects) {


				if (targetObject->hasTrait(TraitTag::toggleable)) {
					targetObject->addState(stateToPropogate);
				}
				//turn off state of all children as well
				StateComponent::propogateStateToAllChildren(targetObject.get(), stateToPropogate, TraitTag::toggleable);

			}

		}
		else if (gameObject->parent().has_value() &&
			gameObject->parent().value()->hasComponent(ComponentTypes::CHILDREN_COMPONENT)) {


			if (gameObject->parent().value()->hasTrait(TraitTag::toggleable)) {
				gameObject->parent().value()->addState(stateToPropogate);
			}
			StateComponent::propogateStateToAllChildren(gameObject->parent().value(), stateToPropogate, TraitTag::toggleable);

		}
	}
	//If this is a toggleable, then we will propogate the state to the children only
	else if (gameObject->hasTrait(TraitTag::toggleable)) {

		StateComponent::propogateStateToAllChildren(gameObject, stateToPropogate, TraitTag::toggleable);
	}

}



