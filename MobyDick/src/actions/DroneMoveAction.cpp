#include "DroneMoveAction.h"
#include "../components/StateComponent.h"


#include "../GameObject.h"


void DroneMoveAction::perform(b2Vec2 trajectory)
{
	const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	const auto& animationComponent = m_parent->getComponent<AnimationComponent>(ComponentTypes::ANIMATION_COMPONENT);
	const auto& stateComponent = m_parent->getComponent<StateComponent>(ComponentTypes::STATE_COMPONENT);
	const auto& vitalityComponent = m_parent->getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);

	physicsComponent->applyMovement(vitalityComponent->speed(), trajectory);


	if (animationComponent)
	{

		if (trajectory.Length() != 0)
		{
			
			stateComponent->addState(GameObjectState::RUN);
			
		}

	}
}
