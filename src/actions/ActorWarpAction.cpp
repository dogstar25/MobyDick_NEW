#include "ActorWarpAction.h"

#include "../GameObject.h"

void ActorWarpAction::perform(b2Vec2 destination)
{
	const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	auto b2Destination = util::toBox2dPoint(destination);

	physicsComponent->setTransform(b2Destination);

}
