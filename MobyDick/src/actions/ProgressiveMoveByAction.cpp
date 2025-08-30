#include "ProgressiveMoveByAction.h"
#include "../components/StateComponent.h"


#include "../GameObject.h"


void ProgressiveMoveByAction::perform()
{

	m_status = ProgressionStatus::IN_PROGRESS;

	const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	if (m_parent->hasComponent(ComponentTypes::ACTION_COMPONENT)) {

		const auto& actionComponent = m_parent->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);

		float directionX = getActionProperty("xPixels").asFloat();
		float directionY = getActionProperty("yPixels").asFloat();
		float speed = getActionProperty("speed").asFloat();

		//set destination and calculate trajectory
		b2Vec2 moveTrajectory = { directionX, directionY };

		m_destination = {
			m_parent->getCenterPosition().x + moveTrajectory.x,
			m_parent->getCenterPosition().y + moveTrajectory.y
		};

		b2Normalize(moveTrajectory);

		//Adjust speed property for box2d
		speed *= .01;

		physicsComponent->applyMovement(speed, { moveTrajectory.x, moveTrajectory.y });

		m_lastSavedDistance.reset();

	}

}

void ProgressiveMoveByAction::update()
{

	auto distance = util::calculateDistance(m_parent->getCenterPosition(), { m_destination.x, m_destination.y });
	if (m_lastSavedDistance.has_value() && distance >= m_lastSavedDistance) {

		const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		physicsComponent->setLinearVelocity({ 0,0 });
		physicsComponent->setTransform(util::toBox2dPoint( m_destination));

		m_status = ProgressionStatus::COMPLETE;
	}
	else {
		m_lastSavedDistance = distance;
	}

}
