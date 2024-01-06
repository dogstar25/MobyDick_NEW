#include "StateComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

StateComponent::StateComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::STATE_COMPONENT;

	m_beginState = game->enumMap()->toEnum(componentJSON["beginState"].asString());
	m_states.set(m_beginState, true);

	if (componentJSON.isMember("transitions")) {

		for (Json::Value itrItem : componentJSON["transitions"])
		{

			StateTransition stateTransition;
			stateTransition.fromState = game->enumMap()->toEnum(itrItem["from"].asString());
			stateTransition.toState = game->enumMap()->toEnum(itrItem["to"].asString());
			stateTransition.transitionDuration = itrItem["duration"].asFloat();
			stateTransition.animationId = itrItem["animationId"].asString();

			m_transitions.push_back(stateTransition);
		}
	}

	if (componentJSON.isMember("animationStates")) {

		for (Json::Value itrItem : componentJSON["animationStates"])
		{

			RenderState renderState;
			renderState.state = game->enumMap()->toEnum(itrItem["state"].asString());
			renderState.animationId = itrItem["animationId"].asString();

			m_animationStates[renderState.state] = renderState;

		}
	}



}

void StateComponent::postInit()
{




}

bool StateComponent::testState(int state)
{

	return m_states.test(state);

}

std::optional<StateTransition> StateComponent::getCurrentTransition()
{

	for (auto transition : m_transitions) {

		//Do we 
		if (transition.transitionTimer.has_value() == true &&
			transition.transitionTimer.value().hasMetTargetDuration() == false) {

			return transition;

		}
	}

	return std::nullopt;

}

std::optional<std::string> StateComponent::getCurrentAnimatedState()
{
	

	auto transition = getCurrentTransition();

	//If we have an active transition from one state to another and it is tied to an animation
	//then return it
	if (transition.has_value() && transition.value().animationId.has_value()) {

		return transition.value().animationId;

	}
	//Otherwise see if we have are in a state that is tied to an animation and returned the first one found
	else {

		auto itr = m_animationStates.begin();
		while (itr != m_animationStates.end()) {

			if (testState(itr->second.state) == true) {

				return itr->second.animationId;
			}

			itr++;
		}

	}

	return std::nullopt;

}

void StateComponent::removeState(int newState)
{

	m_states.set(newState, false);


}

void StateComponent::addState(int newState)
{

	//Check to see if we have a transition defined for this state combination
	//Also, if the gameObject already has the state, dont try to set it again
	if (_hasTransitionDuration(newState) == false and testState(newState) == false) {
		
		_setAndReconcileState(newState);

	}

}

void StateComponent::_setAndReconcileState(int newState)
{

	switch (newState) {

	case GameObjectState::IDLE:

		m_states.set(newState, true);

		m_states.set(GameObjectState::WALK, false);
		m_states.set(GameObjectState::RUN, false);
		m_states.set(GameObjectState::SPRINT, false);
		m_states.set(GameObjectState::JUMP, false);
		m_states.set(GameObjectState::CLIMB, false);
		break;

	case GameObjectState::WALK:

		m_states.set(newState, true);

		m_states.set(GameObjectState::IDLE, false);
		m_states.set(GameObjectState::RUN, false);
		m_states.set(GameObjectState::SPRINT, false);
		m_states.set(GameObjectState::JUMP, false);
		m_states.set(GameObjectState::CLIMB, false);
		break;

	case GameObjectState::RUN:

		m_states.set(newState, true);

		m_states.set(GameObjectState::WALK, false);
		m_states.set(GameObjectState::IDLE, false);
		m_states.set(GameObjectState::SPRINT, false);
		m_states.set(GameObjectState::JUMP, false);
		m_states.set(GameObjectState::CLIMB, false);
		break;

	case GameObjectState::SPRINT:

		m_states.set(newState, true);

		m_states.set(GameObjectState::WALK, false);
		m_states.set(GameObjectState::IDLE, false);
		m_states.set(GameObjectState::RUN, false);
		m_states.set(GameObjectState::JUMP, false);
		m_states.set(GameObjectState::CLIMB, false);
		break;

	case GameObjectState::JUMP:

		m_states.set(newState, true);

		m_states.set(GameObjectState::WALK, false);
		m_states.set(GameObjectState::IDLE, false);
		m_states.set(GameObjectState::RUN, false);
		m_states.set(GameObjectState::SPRINT, false);
		m_states.set(GameObjectState::CLIMB, false);
		break;

	case GameObjectState::CLIMB:

		m_states.set(newState, true);

		m_states.set(GameObjectState::WALK, false);
		m_states.set(GameObjectState::IDLE, false);
		m_states.set(GameObjectState::RUN, false);
		m_states.set(GameObjectState::SPRINT, false);
		m_states.set(GameObjectState::JUMP, false);
		break;

	case GameObjectState::OPENED:

		m_states.set(newState, true);

		m_states.set(GameObjectState::CLOSED, false);
		break;

	case GameObjectState::CLOSED:

		m_states.set(newState, true);

		m_states.set(GameObjectState::OPENED, false);
		break;

	default:

		SDL_assert(true && "No match for State!");

	}
}

void StateComponent::update()
{

	//Loop through all potential transitions that are "in progress" and set the state if the transition timer is complete
	for (auto& transition : m_transitions) {

		//Have we already started a transition timer for it and if so, has it expired
		if (transition.transitionTimer.has_value() == true &&
			transition.transitionTimer.value().hasMetTargetDuration() == true) {

			_setAndReconcileState(transition.toState);
			transition.transitionTimer = std::nullopt;

		}

	}


}

void StateComponent::setParent(GameObject* gameObject)
{
	//Call base component setParent
	Component::setParent(gameObject);

}

