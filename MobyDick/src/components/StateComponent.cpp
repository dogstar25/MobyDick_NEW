#include "StateComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

StateComponent::StateComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::STATE_COMPONENT;

	initializeMutuallyExclusiveStates();

}

void StateComponent::postInit()
{




}

void StateComponent::setState(GameObjectState newState)
{

	//set the state
	State state;
	m_states.insert(state);

	switch (state.state) {

		case GameObjectState::IDLE:

			m_states.erase({ GameObjectState::WALK });
			m_states.erase({ GameObjectState::RUN });
			m_states.erase({ GameObjectState::SPRINT });
			m_states.erase({ GameObjectState::JUMP });
			m_states.erase({ GameObjectState::CLIMB });
			break;

		case GameObjectState::WALK:

			m_states.erase({ GameObjectState::RUN });
			m_states.erase({ GameObjectState::IDLE });
			m_states.erase({ GameObjectState::SPRINT });
			m_states.erase({ GameObjectState::JUMP });
			m_states.erase({ GameObjectState::CLIMB });
			break;

		case GameObjectState::RUN:

			m_states.erase({ GameObjectState::WALK });
			m_states.erase({ GameObjectState::IDLE });
			m_states.erase({ GameObjectState::SPRINT });
			m_states.erase({ GameObjectState::JUMP });
			m_states.erase({ GameObjectState::CLIMB });
			break;

		case GameObjectState::SPRINT:

			m_states.erase({ GameObjectState::WALK });
			m_states.erase({ GameObjectState::IDLE });
			m_states.erase({ GameObjectState::RUN });
			m_states.erase({ GameObjectState::JUMP });
			m_states.erase({ GameObjectState::CLIMB });
			break;

		case GameObjectState::JUMP:

			m_states.erase({ GameObjectState::WALK });
			m_states.erase({ GameObjectState::IDLE });
			m_states.erase({ GameObjectState::RUN });
			m_states.erase({ GameObjectState::SPRINT });
			m_states.erase({ GameObjectState::CLIMB });
			break;

		case GameObjectState::CLIMB:

			m_states.erase({ GameObjectState::WALK });
			m_states.erase({ GameObjectState::IDLE });
			m_states.erase({ GameObjectState::RUN });
			m_states.erase({ GameObjectState::JUMP });
			m_states.erase({ GameObjectState::SPRINT });
			break;

		default:

			SDL_assert(true && "No match for State!");

	}


}


void StateComponent::initializeMutuallyExclusiveStates()
{

	//m_mutuallyExclusiveStates = std::map<GameObjectState, std::set<GameObjectState>>();



}

void StateComponent::update()
{




}

void StateComponent::setParent(GameObject* gameObject)
{
	//Call base component setParent
	Component::setParent(gameObject);

}

void StateComponent::transitionTo(GameObjectState gameObjectState)
{
}
