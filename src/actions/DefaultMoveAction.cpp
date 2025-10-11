#include "DefaultMoveAction.h"

#include "../GameObject.h"

void DefaultMoveAction::perform(int direction, int strafe)
{
	float speed{10};
	const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	//Vitality component could be optional for this default move action
	if (m_parent->hasComponent(ComponentTypes::VITALITY_COMPONENT)) {
		const auto& vitalityComponent = m_parent->getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);
		speed = vitalityComponent->speed();
	}

	physicsComponent->applyMovement(speed, direction, strafe);

}
