#include "OnOffToggleAction.h"
#include "../components/StateComponent.h"
#include "../GameObject.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

void OnOffToggleAction::perform()
{

	GameObjectState stateToPropogate{};

	//If we dont want to toggle, but set the on/OFF to what is passed in as a property, get the property
	if (hasActionProperty("forcedState")) {

		auto property = getActionProperty("forcedState");
		stateToPropogate = (GameObjectState)game->enumMap()->toEnum(property.asString());

	}
	else {

		//Flip the switch on/off state
		if (m_parent->hasState(GameObjectState::ON)) {

			m_parent->addState(GameObjectState::OFF);
			stateToPropogate = GameObjectState::OFF;
		}
		else {

			m_parent->addState(GameObjectState::ON);
			stateToPropogate = GameObjectState::ON;
		}

	}

	if (m_parent->hasTrait(TraitTag::toggle_switch)) {


		std::string buttonTargetObjectName = m_parent->name() + "_TARGET";
		auto targetObjects = m_parent->parentScene()->getGameObjectsByName(buttonTargetObjectName);
		if (targetObjects.empty() == false) {


			for (auto& targetObject : targetObjects) {


				if (targetObject->hasTrait(TraitTag::toggleable)) {
					targetObject->addState(stateToPropogate);
				}
				//turn off state of all children as well
				StateComponent::propogateStateToAllChildren(targetObject.get(), stateToPropogate, TraitTag::toggleable);

			}

		}
		else if (m_parent->parent().has_value() &&
			m_parent->parent().value()->hasComponent(ComponentTypes::CHILDREN_COMPONENT)) {


			if (m_parent->parent().value()->hasTrait(TraitTag::toggleable)) {
				m_parent->parent().value()->addState(stateToPropogate);
			}
			StateComponent::propogateStateToAllChildren(m_parent->parent().value(), stateToPropogate, TraitTag::toggleable);

		}
	}
	//If this is a toggleable, then we will propogate the state to the children only
	else if (m_parent->hasTrait(TraitTag::toggleable)) {

		StateComponent::propogateStateToAllChildren(m_parent, stateToPropogate, TraitTag::toggleable);
	}

}



