#include "SurvivorBrainComponent.h"

#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdlrenderer.h"
#include "game.h"

#include "../GameConstants.h"

extern std::unique_ptr<Game> game;


SurvivorBrainComponent::SurvivorBrainComponent(Json::Value definitionJSON)
	: BrainComponent(definitionJSON)
{

	m_currentState = BrainState::IDLE;

}

void SurvivorBrainComponent::update()
{

	BrainComponent::update();

	//Determine state
	m_currentState = _determineState();

	switch (m_currentState) {

	case BrainState::FOLLOW:
		_doFollow();
		break;
	case BrainState::LOST:
		_doLost();
		break;
	default:
		_doIdle();
		break;
	}

}

void SurvivorBrainComponent::followMe(GameObject* gameObjectToFollow) {

	m_gameObjectToFollow = gameObjectToFollow;
	m_currentState = BrainState::FOLLOW;

}

void SurvivorBrainComponent::stay() {

	m_gameObjectToFollow = nullptr;
	m_currentState = BrainState::IDLE;

}


int SurvivorBrainComponent::_determineState()
{

	int state{ m_currentState };

	if (m_currentState == BrainState::FOLLOW) {
		//If we have lost site of the object we're following then change to lost state
		if (_detectFollowedObject() == false) {

			if (m_lostTimer.firstTime) {
				m_lostTimer = Timer(3);
			}
			else if (m_lostTimer.hasMetTargetDuration()) {
				state = BrainState::LOST;
			}
		}
	}
	if (m_currentState == BrainState::LOST) {

		if (_detectFollowedObject() == true) {
			m_interimDestination.reset();
			state = BrainState::FOLLOW;
			m_lostTimer = {};
				
		}
	}

	return state;

}

void SurvivorBrainComponent::_doLost()
{
	//_determineTargetLocation(); - add this once all nav points are in place. Set targetDestination to the nearest nav point that 
	//can see the followed object
	setTargetDestination(m_gameObjectToFollow->getCenterPosition());
	navigate();
	m_tempVisitedNavPoints.clear();

}

void SurvivorBrainComponent::_stayBehindFollowedObject()
{

	const auto& gameObjectToFollowPhysicsComponent = m_gameObjectToFollow->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	const auto& gameObjectToFollowTransformComponent = m_gameObjectToFollow->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);


	float gameObjectAngleDegrees = util::radiansToDegrees(gameObjectToFollowPhysicsComponent->angle());
	float orientationAngle = atan2(
		parent()->getCenterPosition().y - m_gameObjectToFollow->getCenterPosition().y,
		parent()->getCenterPosition().x - m_gameObjectToFollow->getCenterPosition().x
	);


	float orientationAngleDegrees = util::radiansToDegrees(orientationAngle);

	float difference = gameObjectAngleDegrees - orientationAngleDegrees;

	if (!((difference > 90 && difference < 270) ||
		(difference > -180 && difference < -90))) {

		//Rotate to the left or the right based on the position
		int sinDirection{};
		int cosDirection{};
		if (difference < 90) {
			sinDirection = 1;
			cosDirection = -1;
		}
		else {
			sinDirection = -1;
			cosDirection = 1;
		}

		//Rotate myself around the object im following until im behind
		b2Vec2 trajectory{};
		trajectory.x = (sin(orientationAngle) * 200 * sinDirection) + (m_gameObjectToFollow->getCenterPosition().x - parent()->getCenterPosition().x);
		trajectory.y = (cos(orientationAngle) * 200 * cosDirection) + (m_gameObjectToFollow->getCenterPosition().y - parent()->getCenterPosition().y);
		trajectory.Normalize();

		const auto& actionComponent = parent()->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);
		const auto& moveAction = actionComponent->getAction(ACTION_MOVE);
		moveAction->perform(parent(), trajectory);

	}

}


void SurvivorBrainComponent::_doFollow()
{

	b2Vec2 trajectory{};

	//If we are not closeenough to the object we're following then move to within tolerance
	if (util::calculateDistance(parent()->getCenterPosition(), m_gameObjectToFollow->getCenterPosition())
		> SURVIVOR_FOLLOW_TOLERANCE) {

		trajectory.x = m_gameObjectToFollow->getCenterPosition().x - parent()->getCenterPosition().x;
		trajectory.y = m_gameObjectToFollow->getCenterPosition().y - parent()->getCenterPosition().y;

		trajectory.Normalize();

		const auto& actionComponent = parent()->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);
		const auto& moveAction = actionComponent->getAction(ACTION_MOVE);
		moveAction->perform(parent(), trajectory);

	}

	//Stay behind the object you are following
	//If we are touching a wall, then stay put. This shoudl help player tell me to stay instead of always
	//moving out of his way
	if (!_isTouchingBarrier()) {
		_stayBehindFollowedObject();
	}


	//
	//DeBug stuff - draw some helper lines
	//
	//const auto& gameObjectToFollowPhysicsComponent = m_gameObjectToFollow->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	//const auto& gameObjectToFollowTransformComponent = m_gameObjectToFollow->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);

	//SDL_FPoint destination{};
	//destination.x = m_gameObjectToFollow->getCenterPosition().x + (64 * cos(gameObjectToFollowPhysicsComponent->angle() + util::degreesToRadians(135)));
	//destination.y = m_gameObjectToFollow->getCenterPosition().y + (64 * sin(gameObjectToFollowPhysicsComponent->angle() + util::degreesToRadians(135)));

	//SDL_FPoint destination2{};
	//destination2.x = m_gameObjectToFollow->getCenterPosition().x + (64 * cos(gameObjectToFollowPhysicsComponent->angle() + util::degreesToRadians(180)));
	//destination2.y = m_gameObjectToFollow->getCenterPosition().y + (64 * sin(gameObjectToFollowPhysicsComponent->angle() + util::degreesToRadians(180)));

	//SDL_FPoint destination3{};
	//destination3.x = m_gameObjectToFollow->getCenterPosition().x + (64 * cos(gameObjectToFollowPhysicsComponent->angle() + util::degreesToRadians(225)));
	//destination3.y = m_gameObjectToFollow->getCenterPosition().y + (64 * sin(gameObjectToFollowPhysicsComponent->angle() + util::degreesToRadians(225)));

	//glm::vec2 start{ m_gameObjectToFollow->getCenterPosition().x , m_gameObjectToFollow->getCenterPosition().y };
	//glm::vec2 stop{ destination.x , destination.y };
	//game->renderer()->addLine(start, stop, { 255,255,255,255 });
	//start = { m_gameObjectToFollow->getCenterPosition().x , m_gameObjectToFollow->getCenterPosition().y };
	//stop = { destination2.x , destination2.y };
	//game->renderer()->addLine(start, stop, { 255,255,255,255 });
	//start = { m_gameObjectToFollow->getCenterPosition().x , m_gameObjectToFollow->getCenterPosition().y };
	//stop = { destination3.x , destination3.y };
	//game->renderer()->addLine(start, stop, { 255,255,255,255 });


}

bool SurvivorBrainComponent::_detectFollowedObject()
{

	std::optional<SDL_FPoint> playerPosition{};

	for (auto& seenObject : m_seenObjects) {

		if (seenObject.gameObject == m_gameObjectToFollow) {

			return true;
		}
	}

	return false;

}

bool SurvivorBrainComponent::_isTouchingBarrier()
{

	for (const auto& touchingObject : parent()->getTouchingObjects()) {

		if (touchingObject.second->hasTrait(TraitTag::barrier)) {
			return true;
		}
	}

	return false;
}


void SurvivorBrainComponent::_doIdle()
{
	//const auto& animationComponent = parent()->getComponent<AnimationComponent>(ComponentTypes::ANIMATION_COMPONENT);
	//animationComponent->setCurrentAnimationState(ANIMATION_IDLE);

}



bool SurvivorBrainComponent::_rotateTowards(b2Vec2 targetPoint)
{
	bool onTarget{ false };

	auto physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
	auto transformComponent = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	auto currentAngle = physicsComponent->angle();
	auto rotationCenter = transformComponent->getCenterPosition();

	//calculate the angle
	auto desiredAngle = atan2f(
		(targetPoint.y - rotationCenter.y),
		(targetPoint.x - rotationCenter.x));
	desiredAngle = util::normalizeRadians(desiredAngle);

	auto desiredAngleDegrees = util::radiansToDegrees(desiredAngle);
	auto objectAngleDegrees = util::radiansToDegrees(currentAngle);

	float rotationVelocity{ 0 };

	//Get the objects action and vitality components
	auto actionComponent = parent()->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);
	auto vitality = parent()->getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);

	if ((desiredAngle - currentAngle) < 0.0) {
		rotationVelocity = vitality->rotateSpeed() * -1;
	}
	else {
		rotationVelocity = vitality->rotateSpeed();
	}

	auto difference = abs(desiredAngle - currentAngle);

	//Once the angle is very close then set the angle directly
	if (difference < 0.19) {

		const auto& action = actionComponent->getAction(ACTION_ROTATE);
		action->perform(parent(), 0);
		onTarget = true;

		//This next code makes the aim dead on balls accurate - may be too hard
		const auto& physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		physicsComponent->setAngle(desiredAngle);
		physicsComponent->update();

	}
	else {
		const auto& action = actionComponent->getAction(ACTION_ROTATE);
		action->perform(parent(), rotationVelocity);
	}

	return onTarget;
}

std::optional<SDL_FPoint> SurvivorBrainComponent::_detectPlayer()
{

	std::optional<SDL_FPoint> playerPosition{};

	for (auto& seenObject : m_seenObjects) {

		if (seenObject.gameObject->hasTrait(TraitTag::player)) {

			playerPosition = seenObject.gameObject->getCenterPosition();
			break;
		}
	}

	return playerPosition;

}