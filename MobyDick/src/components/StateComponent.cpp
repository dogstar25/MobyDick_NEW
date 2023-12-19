#include "StateComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

StateComponent::StateComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::STATE_COMPONENT;

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

	if (componentJSON.isMember("renderStates")) {

		for (Json::Value itrItem : componentJSON["renderStates"])
		{

			RenderState renderState;
			renderState.state = game->enumMap()->toEnum(itrItem["state"].asString());
			renderState.animationId = itrItem["animationId"].asString();

			m_renderStates[renderState.state] = renderState;

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

void StateComponent::_addState(int state)
{

	m_states.set(state, true);

	//See if there is a render state defined for this state
	if (m_renderStates.find(state) != m_renderStates.end() && parent()->hasComponent(ComponentTypes::ANIMATION_COMPONENT)) {

		const auto& renderState = m_renderStates[state];

		const auto& animationComponent = parent()->getComponent<AnimationComponent>(ComponentTypes::ANIMATION_COMPONENT);
		animationComponent->animate(renderState.animationId);

	}

}

void StateComponent::removeState(int newState)
{

	m_states.set(newState, false);


}

void StateComponent::addState(int newState)
{

	//Check to see if we have a transition defined for this state combination
	//Also, if the gameObject already has the state, dont try to set it again
	if (_checkTransition(newState) == false and testState(newState) == false) {

		_addState(newState);

		switch (newState) {

		case GameObjectState::IDLE:

			m_states.set(GameObjectState::WALK, false);
			m_states.set(GameObjectState::RUN, false);
			m_states.set(GameObjectState::SPRINT, false);
			m_states.set(GameObjectState::JUMP, false);
			m_states.set(GameObjectState::CLIMB, false);
			break;

		case GameObjectState::WALK:

			m_states.set(GameObjectState::IDLE, false);
			m_states.set(GameObjectState::RUN, false);
			m_states.set(GameObjectState::SPRINT, false);
			m_states.set(GameObjectState::JUMP, false);
			m_states.set(GameObjectState::CLIMB, false);
			break;

		case GameObjectState::RUN:

			m_states.set(GameObjectState::WALK, false);
			m_states.set(GameObjectState::IDLE, false);
			m_states.set(GameObjectState::SPRINT, false);
			m_states.set(GameObjectState::JUMP, false);
			m_states.set(GameObjectState::CLIMB, false);
			break;

		case GameObjectState::SPRINT:

			m_states.set(GameObjectState::WALK, false);
			m_states.set(GameObjectState::IDLE, false);
			m_states.set(GameObjectState::RUN, false);
			m_states.set(GameObjectState::JUMP, false);
			m_states.set(GameObjectState::CLIMB, false);
			break;

		case GameObjectState::JUMP:

			m_states.set(GameObjectState::WALK, false);
			m_states.set(GameObjectState::IDLE, false);
			m_states.set(GameObjectState::RUN, false);
			m_states.set(GameObjectState::SPRINT, false);
			m_states.set(GameObjectState::CLIMB, false);
			break;

		case GameObjectState::CLIMB:

			m_states.set(GameObjectState::WALK, false);
			m_states.set(GameObjectState::IDLE, false);
			m_states.set(GameObjectState::RUN, false);
			m_states.set(GameObjectState::SPRINT, false);
			m_states.set(GameObjectState::JUMP, false);
			break;

		default:

			SDL_assert(true && "No match for State!");

		}

	}

}


void StateComponent::update()
{

	//Loop through all potential transitions that are "in progress" and set the state if the transition timer is complete
	for (auto transition : m_transitions) {

		//Have we already started a transition timer for it and if so, has it expired
		if (transition.transitionTimer.has_value() == true &&
			transition.transitionTimer.value().hasMetTargetDuration() == true) {

			_addState(transition.toState);
			transition.transitionTimer = std::nullopt;

		}

	}


}

void StateComponent::setParent(GameObject* gameObject)
{
	//Call base component setParent
	Component::setParent(gameObject);

}

