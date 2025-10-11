#include "DefaultRotateAction.h"

#include "../GameObject.h"


void DefaultRotateAction::perform(float angularVelocity)
{
	const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	physicsComponent->applyRotation(angularVelocity);

}
