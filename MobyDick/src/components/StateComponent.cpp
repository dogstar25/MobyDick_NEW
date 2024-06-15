#include "StateComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

StateComponent::StateComponent(Json::Value componentJSON) :
	Component(ComponentTypes::STATE_COMPONENT)
{

	if (componentJSON.isMember("beginStates")) {
		Json::Value beginStates = componentJSON["beginStates"];
		for (auto& stateItr : beginStates) {

			auto state = (GameObjectState)game->enumMap()->toEnum(stateItr.asString());
			addState(state);
		}
	}

	if (componentJSON.isMember("transitions")) {

		for (Json::Value itrItem : componentJSON["transitions"])
		{

			StateTransition stateTransition;
			stateTransition.fromState = (GameObjectState)game->enumMap()->toEnum(itrItem["from"].asString());
			stateTransition.toState = (GameObjectState)game->enumMap()->toEnum(itrItem["to"].asString());
			stateTransition.transitionDuration = itrItem["duration"].asFloat();
			stateTransition.animationId = itrItem["animationId"].asString();

			m_transitions.push_back(stateTransition);
		}
	}

	if (componentJSON.isMember("animationStates")) {

		for (Json::Value itrItem : componentJSON["animationStates"])
		{

			RenderState renderState;
			renderState.state = (GameObjectState)game->enumMap()->toEnum(itrItem["state"].asString());
			renderState.animationId = itrItem["animationId"].asString();

			m_animationStates[renderState.state] = renderState;

		}
	}



}

void StateComponent::postInit()
{




}

bool StateComponent::testState(GameObjectState state)
{

	return m_states.test((int)state);

}

std::optional<StateTransition> StateComponent::getCurrentTransition()
{

	for (auto transition : m_transitions) {

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

void StateComponent::removeState(GameObjectState newState)
{

	if (testState(newState)) {

		//Set the state to false
		m_states.set((int)newState, false);

		//Depending on the state being removed, there may be further state specific code to
		//reverse from the state being onpreviously
		switch (newState)
		{
		case GameObjectState::DISABLED_COLLISION:

			//enable collisions
			if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {
				const auto& physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
				for (auto fixture = physicsComponent->physicsBody()->GetFixtureList(); fixture != 0; fixture = fixture->GetNext())
				{
					ContactDefinition* contactDefinition = reinterpret_cast<ContactDefinition*>(fixture->GetUserData().pointer);
					contactDefinition->contactTag = contactDefinition->saveOriginalContactTag;
					fixture->Refilter();
				}
			}

			break;
		case GameObjectState::DISABLED_PHYSICS:

			if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {

				const auto& physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
				if (physicsComponent) {
					physicsComponent->setPhysicsBodyActive(true);
				}

			}

			break;

		case GameObjectState::DISABLED_RENDER:
			break;

		case GameObjectState::DISABLED_UPDATE:
			break;

		}
	}
}

void StateComponent::addState(GameObjectState newState)
{


	//Check to see if we have a transition defined for this state combination
	//Also, if the gameObject already has the state, dont try to set it again
	if (_hasTransitionDuration(newState) == false and testState(newState) == false) {
		
		_setAndReconcileState(newState);

	}

}

void StateComponent::_setAndReconcileState(GameObjectState newState)
{
	//Set the state to true
	m_states.set((int)newState, true);

	//Depending on the state being added, other states may have to be turned off
	//because of mutual exclusivity.
	switch (newState) {

		case GameObjectState::ON:

			m_states.set((int)GameObjectState::OFF, false);
			break;

		case GameObjectState::OFF:

			m_states.set((int)GameObjectState::ON, false);
			break;

		case GameObjectState::IDLE:

			m_states.set((int)GameObjectState::WALK, false);
			m_states.set((int)GameObjectState::RUN, false);
			m_states.set((int)GameObjectState::SPRINT, false);
			m_states.set((int)GameObjectState::JUMP, false);
			m_states.set((int)GameObjectState::CLIMB, false);
			break;

		case GameObjectState::WALK:

			m_states.set((int)GameObjectState::IDLE, false);
			m_states.set((int)GameObjectState::RUN, false);
			m_states.set((int)GameObjectState::SPRINT, false);
			m_states.set((int)GameObjectState::JUMP, false);
			m_states.set((int)GameObjectState::CLIMB, false);
			break;

		case GameObjectState::RUN:

			m_states.set((int)GameObjectState::WALK, false);
			m_states.set((int)GameObjectState::IDLE, false);
			m_states.set((int)GameObjectState::SPRINT, false);
			m_states.set((int)GameObjectState::JUMP, false);
			m_states.set((int)GameObjectState::CLIMB, false);
			break;

		case GameObjectState::SPRINT:

			m_states.set((int)GameObjectState::WALK, false);
			m_states.set((int)GameObjectState::IDLE, false);
			m_states.set((int)GameObjectState::RUN, false);
			m_states.set((int)GameObjectState::JUMP, false);
			m_states.set((int)GameObjectState::CLIMB, false);
			break;

		case GameObjectState::JUMP:

			m_states.set((int)GameObjectState::WALK, false);
			m_states.set((int)GameObjectState::IDLE, false);
			m_states.set((int)GameObjectState::RUN, false);
			m_states.set((int)GameObjectState::SPRINT, false);
			m_states.set((int)GameObjectState::CLIMB, false);
			break;

		case GameObjectState::CLIMB:

			m_states.set((int)GameObjectState::WALK, false);
			m_states.set((int)GameObjectState::IDLE, false);
			m_states.set((int)GameObjectState::RUN, false);
			m_states.set((int)GameObjectState::SPRINT, false);
			m_states.set((int)GameObjectState::JUMP, false);
			break;

		case GameObjectState::OPENED:

			m_states.set((int)GameObjectState::CLOSED, false);
			break;

		case GameObjectState::CLOSED:

			m_states.set((int)GameObjectState::OPENED, false);
			break;

		case GameObjectState::ITEM_OBTAINABLE:

			m_states.set((int)GameObjectState::ITEM_STORED_PLAYER, false);
			break;

		case GameObjectState::ITEM_STORED_ENCLOSED:

			m_states.set((int)GameObjectState::ITEM_STORED_PLAYER, false);
			break;

		case GameObjectState::ITEM_STORED_OPEN:

			m_states.set((int)GameObjectState::ITEM_STORED_ENCLOSED, false);
			m_states.set((int)GameObjectState::ITEM_STORED_PLAYER, false);
			break;

		case GameObjectState::ITEM_LOOSE:

			m_states.set((int)GameObjectState::ITEM_STORED_PLAYER, false);
			m_states.set((int)GameObjectState::ITEM_STORED_ENCLOSED, false);
			break;

		case GameObjectState::WALK_LEFT:

			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::WALK_RIGHT:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::WALK_UP:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::WALK_DOWN:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_RIGHT:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_LEFT:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_UP:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_DOWN:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::WALK_LEFT_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::WALK_RIGHT_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::WALK_UP_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::WALK_DOWN_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_RIGHT_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_LEFT_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_UP_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_DOWN_EQUIPPED, false);

			break;

		case GameObjectState::IDLE_DOWN_EQUIPPED:

			m_states.set((int)GameObjectState::WALK_LEFT, false);
			m_states.set((int)GameObjectState::WALK_RIGHT, false);
			m_states.set((int)GameObjectState::WALK_UP, false);
			m_states.set((int)GameObjectState::WALK_DOWN, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT, false);
			m_states.set((int)GameObjectState::IDLE_LEFT, false);
			m_states.set((int)GameObjectState::IDLE_UP, false);
			m_states.set((int)GameObjectState::IDLE_DOWN, false);
			m_states.set((int)GameObjectState::WALK_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_UP_EQUIPPED, false);
			m_states.set((int)GameObjectState::WALK_DOWN_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_RIGHT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_LEFT_EQUIPPED, false);
			m_states.set((int)GameObjectState::IDLE_UP_EQUIPPED, false);

			break;
		case GameObjectState::DISABLED_COLLISION:

			if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {
				const auto& physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
				for (auto fixture = physicsComponent->physicsBody()->GetFixtureList(); fixture != 0; fixture = fixture->GetNext())
				{
					if (fixture->IsSensor() == false) {
						ContactDefinition* contactDefinition = reinterpret_cast<ContactDefinition*>(fixture->GetUserData().pointer);
						contactDefinition->contactTag = ContactTag::GENERAL_FREE;
						fixture->Refilter();
					}
				}
			}

			break;
		case GameObjectState::DISABLED_PHYSICS:

			if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {

				const auto& physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
				if (physicsComponent) {
					physicsComponent->setPhysicsBodyActive(false);
				}
			}

			m_states.set((int)newState, true);
			break;

		case GameObjectState::DISABLED_RENDER:
			m_states.set((int)newState, true);
			break;

		case GameObjectState::DISABLED_UPDATE:
			m_states.set((int)newState, true);
			break;

		case GameObjectState::ON_VERTICAL_MOVEMENT:
			m_states.set((int)newState, true);
			break;
		case GameObjectState::EQUIPPED:
			//nothing
			break;
		case GameObjectState::IMPASSABLE:
			//nothing
			break;

		default:

			SDL_assert(false && "No match for State!");

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

bool StateComponent::_hasTransitionDuration(GameObjectState newState)
{

	for (auto& transition : m_transitions) {

		//If we have a transition defined that matches the states that we are trying to go to and from
		if (m_states.test((int)transition.fromState) == true && transition.toState == newState) {

			//If we have NOT started a transition timer then start one now
			if (transition.transitionTimer.has_value() == false) {

				transition.transitionTimer = Timer(transition.transitionDuration, false);

			}

			return true;
		}

	}


	return false;
}

