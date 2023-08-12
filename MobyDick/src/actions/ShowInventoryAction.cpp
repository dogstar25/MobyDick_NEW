#include "ShowInventoryAction.h"

#include "../GameObject.h"


void ShowInventoryAction::perform(GameObject* gameObject, float angularVelocity)
{
	const auto& physicsComponent = gameObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	physicsComponent->applyRotation(angularVelocity);

}
