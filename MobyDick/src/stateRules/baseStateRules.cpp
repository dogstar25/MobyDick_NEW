#include "baseStateRules.h"


namespace GameObjectStates {

	TransitionRules::TransitionRules()
	{

		transitionRules = std::map< GameObjectState, TransitionToStates>();

		// Initialize the rules for GameObjectState::RUN
		//TransitionToStates runToStates;
		transitionRules[GameObjectState::RUN] = TransitionToStates();
		transitionRules[GameObjectState::RUN].transitionToStates[GameObjectState::IDLE] = 2.5f;
		transitionRules[GameObjectState::RUN].transitionToStates[GameObjectState::SPRINT];

		transitionRules[GameObjectState::IDLE] = TransitionToStates();
		transitionRules[GameObjectState::IDLE].transitionToStates[GameObjectState::RUN];


		// Add more rules for other states if needed

	}
}