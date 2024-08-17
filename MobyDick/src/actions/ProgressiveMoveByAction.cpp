#include "ProgressiveMoveByAction.h"
#include "../components/StateComponent.h"


#include "../GameObject.h"


void ProgressiveMoveByAction::perform(GameObject* gameObject, b2Vec2 pixels, float speed)
{
	m_status = ProgressionStatus::IN_PROGRESS;

	const auto& physicsComponent = gameObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);


	//set destination and calculate trajectory





	//
	// physicsComponent->applyMovement(speed, direction);

}

void ProgressiveMoveByAction::perform(GameObject* gameObject)
{

	m_status = ProgressionStatus::IN_PROGRESS;

	const auto& physicsComponent = gameObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	if (gameObject->hasComponent(ComponentTypes::ACTION_COMPONENT)) {

		const auto& actionComponent = gameObject->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);

		float directionX = getActionProperty("xPixels").asFloat();
		float directionY = getActionProperty("yPixels").asFloat();
		float speed = getActionProperty("speed").asFloat();

		//set destination and calculate trajectory
		b2Vec2 moveTrajectory = { directionX, directionY };

		m_destination = {
			gameObject->getCenterPosition().x + moveTrajectory.x,
			gameObject->getCenterPosition().y + moveTrajectory.y
		};

		moveTrajectory.Normalize();

		physicsComponent->applyMovement(speed, { moveTrajectory.x, moveTrajectory.y });

	}

}

void ProgressiveMoveByAction::update()
{

	auto distance = util::calculateDistance(parent()->getCenterPosition(), { m_destination.x, m_destination.y });
	if (distance < 1) {

		const auto& physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		physicsComponent->setLinearVelocity({ 0,0 });

	}

}
