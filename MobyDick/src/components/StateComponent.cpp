#include "StateComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

StateComponent::StateComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::STATE_COMPONENT;


}

void StateComponent::postInit()
{




}

bool StateComponent::testState(int state)
{

	return m_states.test(state);

}

void StateComponent::removeState(int newState)
{

	m_states.set(newState, false);


}

void StateComponent::addState(int newState)
{

	m_states.set(newState, true);

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


void StateComponent::update()
{

	//Loop through all potential transitions that are "in progress" and set the state if the transition timer is complete
	for (auto transition : m_transitions) {

		//Have we already started a transition timer for it and if so, has it expired
		if (transition.transitionTimer.has_value() == true &&
			transition.transitionTimer.value().hasMetTargetDuration() == true) {

			m_states.set(transition.toState, true);
			transition.transitionTimer = std::nullopt;

		}

	}


}

void StateComponent::setParent(GameObject* gameObject)
{
	//Call base component setParent
	Component::setParent(gameObject);

}

