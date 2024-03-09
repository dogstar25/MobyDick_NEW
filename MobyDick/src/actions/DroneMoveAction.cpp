#include "DroneMoveAction.h"
#include "../components/StateComponent.h"


#include "../GameObject.h"


void DroneMoveAction::perform(GameObject* gameObject, b2Vec2 trajectory)
{
	const auto& physicsComponent = gameObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	const auto& animationComponent = gameObject->getComponent<AnimationComponent>(ComponentTypes::ANIMATION_COMPONENT);
	const auto& stateComponent = gameObject->getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
	const auto& vitalityComponent = gameObject->getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);

	physicsComponent->applyMovement(vitalityComponent->speed(), trajectory);


	if (animationComponent)
	{

		if (trajectory.Length() != 0)
		{
			
			stateComponent->addState(GameObjectState::RUN);
			
		}

	}
}
