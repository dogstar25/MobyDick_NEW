#pragma once
#include <map>
#include "../BaseConstants.h"

namespace GameObjectStates
{


	struct Transition {

		float transitionTime{0.};

	};


	

	struct TransitionToStates {

		std::map< GameObjectState, float> transitionToStates;

	};

	struct TransitionRules {
		TransitionRules();
		std::map< GameObjectState, TransitionToStates> transitionRules;

	};


}
