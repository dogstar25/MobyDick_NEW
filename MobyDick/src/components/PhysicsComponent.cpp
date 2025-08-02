#include "PhysicsComponent.h"


#include "../EnumMap.h"
#include "../Game.h"


extern std::unique_ptr<Game> game;

PhysicsComponent::PhysicsComponent(Json::Value definitionJSON, GameObject* parent, Scene* parentScene, float xMapPos, float yMapPos, 
	float angleAdjust,	b2Vec2 sizeOverride) :
	Component(ComponentTypes::PHYSICS_COMPONENT, parent)
{

	//Get reference to the animationComponent JSON config and transformComponent JSON config
	Json::Value physicsComponentJSON = util::getComponentConfig(definitionJSON, ComponentTypes::PHYSICS_COMPONENT);
	Json::Value transformComponentJSON = util::getComponentConfig(definitionJSON, ComponentTypes::TRANSFORM_COMPONENT);

	m_physicsType = game->enumMap()->toEnum(physicsComponentJSON["type"].asString());

	m_objectAnchorPoint.Set(physicsComponentJSON["anchorPoint"]["x"].asFloat(),
		physicsComponentJSON["anchorPoint"]["y"].asFloat());

	//Build the physics body
	m_physicsBodyId = _buildB2Body(physicsComponentJSON, transformComponentJSON, parentScene->physicsWorld(), sizeOverride);

	//Calculate the spawn position
	b2Vec2 position{};

	//Get size of the object
	b2Vec2 size = b2Vec2_zero;
	if (sizeOverride != b2Vec2_zero) {
		size = sizeOverride;
	}
	else {
		size = { transformComponentJSON["size"]["width"].asFloat(),
			transformComponentJSON["size"]["height"].asFloat() };
	}

	//We want to mostly always capture a list of other objects that this one is touching, but we can turn it off
	// for some objects to save processing
	if (physicsComponentJSON.isMember("touchingObjectsCapturedRequired")) {
		m_touchingObjectsCapturedRequired = physicsComponentJSON["touchingObjectsCapturedRequired"].asBool();
	}
	
	//Angle adjustment if any in radians
	
	float newAngle = util::degreesToRadians(angleAdjust);
	b2Rot rotation = {cos(newAngle), sin(newAngle)};

	//The width and height can change when a rectangle shape is rotated
	float objectWidthAfterAngle = abs(rotation.s * size.y + rotation.c * size.x);
	float objectHeightAfterAngle = abs(rotation.s * size.x + rotation.c * size.y);

	//Get the pixel position of where we are placing the object. Divide by 2 to get the center
	position.x = (xMapPos * game->worldTileSize().x + (objectWidthAfterAngle / 2));
	position.y = (yMapPos * game->worldTileSize().y + (objectHeightAfterAngle / 2));

	//Scale them to the box2d size
	position = util::toBox2dPoint(position);

	//Initial spawn position
	b2Body_SetTransform(m_physicsBodyId, position, rotation);

}

PhysicsComponent::~PhysicsComponent()
{

	//We need to free the memory associated with our special object that we store in each fixture's userdata
	//NOTE:this should be the only spot where we do not depend on smart pointers
	//std::cout << this->parent()->id() << " PhysicsComponent Destructor called" << std::endl;
	b2ShapeId shapeArray[m_maxBodyShapes];

	int shapeCount = b2Body_GetShapes(m_physicsBodyId, shapeArray, m_maxBodyShapes);
	for (int i; i < shapeCount; i++)
	{
		ContactDefinition* contactDefinition = reinterpret_cast<ContactDefinition*>(b2Shape_GetUserData(shapeArray[i]));
		if (contactDefinition) {
			delete contactDefinition;
		}
	}

	b2DestroyBody(m_physicsBodyId);

}

void PhysicsComponent::postInit()
{


}

void PhysicsComponent::setTransform(b2Vec2 positionVector, float angleInRadians)
{

	b2Rot rotation = { cos(angleInRadians), sin(angleInRadians) };

	b2Body_SetTransform(m_physicsBodyId, positionVector, rotation);

}

void PhysicsComponent::setTransform(b2Vec2 positionVector)
{

	b2Body_SetTransform(m_physicsBodyId, positionVector, b2Body_GetRotation(m_physicsBodyId));

}

void PhysicsComponent::setPhysicsBodyActive(bool  active)
{

	b2Body_Enable(m_physicsBodyId);

}

void PhysicsComponent::setLinearVelocity(b2Vec2 velocityVector)
{

	b2Body_SetLinearVelocity(m_physicsBodyId, velocityVector);

}

void PhysicsComponent::setLinearDamping(float linearDamping)
{

	b2Body_SetLinearDamping(m_physicsBodyId, linearDamping);

}

void PhysicsComponent::setAngularDamping(float angularDamping)
{
	b2Body_SetAngularDamping(m_physicsBodyId, angularDamping);
}

void PhysicsComponent::setGravityScale(float gravityScale)
{
	b2Body_SetGravityScale(m_physicsBodyId, gravityScale);
}

void PhysicsComponent::setAngle(float angle)
{
	b2Rot rotation = b2MakeRot(angle);
	b2Vec2 pos = b2Body_GetPosition(m_physicsBodyId);
	b2Body_SetTransform(m_physicsBodyId, pos, rotation);

}

b2Vec2 PhysicsComponent::position() 
{ 
	return b2Body_GetPosition(m_physicsBodyId);
}

float PhysicsComponent::angle() 
{ 
	float angle = b2Rot_GetAngle(b2Body_GetRotation(m_physicsBodyId));

	return angle; 
}

void PhysicsComponent::update()
{

	
	//If we were given a position change from our contactListener, since we cannnot change position within
	//contactListener, we set it here. An example would be warping the player to a new location after making contact
	//with a portal object
	//TODO:This can be put into the contactListener now in box2d v3 so do it , like for warping an object
	b2Vec2 currentPosition = b2Body_GetPosition(m_physicsBodyId);
	if (m_changePositionPosition.has_value()) {
		currentPosition = { m_changePositionPosition->x , m_changePositionPosition->y };
		m_changePositionPosition.reset();
	}

	////////////////////////////////////////////////////////////////////////////////////////////////

	if (parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->absolutePositioning() == true) {

		SDL_FPoint cameraPosition = { Camera::instance().frame().x, Camera::instance().frame().y };
		SDL_FPoint convertedCameraPosition = util::toBox2dPoint(cameraPosition);

		currentPosition = { currentPosition.x + convertedCameraPosition.x, currentPosition.y + convertedCameraPosition.y};

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////

	b2Body_SetTransform(m_physicsBodyId, currentPosition, b2Body_GetRotation(m_physicsBodyId));

	//Transfer the physicsComponent coordinates to the transformComponent
	//converting the angle to degrees
	b2Vec2 convertedPosition{};
	float convertedAngle = util::radiansToDegrees(angle());

	convertedPosition = b2MulSV(GameConfig::instance().scaleFactor(), b2Body_GetPosition(m_physicsBodyId));

	parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->setPosition(convertedPosition, convertedAngle);
}


b2BodyId PhysicsComponent::_buildB2Body(Json::Value physicsComponentJSON, Json::Value transformComponentJSON, b2WorldId physicsWorldId,
	b2Vec2 sizeOverride)
{

	b2BodyDef bodyDef = b2DefaultBodyDef();
	bodyDef.type = static_cast<b2BodyType>(m_physicsType);
	bodyDef.position = b2Vec2( 0.0f, 0.0f );
	b2BodyId bodyId = b2CreateBody(physicsWorldId, &bodyDef);

	//Store a reference to the parent object in the userData pointer field
	b2Body_SetUserData(bodyId, parent());

	if (physicsComponentJSON.isMember("linearDamping")) {
		body->SetLinearDamping(physicsComponentJSON["linearDamping"].asFloat());
	}
	if (physicsComponentJSON.isMember("angularDamping")) {
		body->SetAngularDamping(physicsComponentJSON["angularDamping"].asFloat());
	}
	if (physicsComponentJSON.isMember("gravityScale")) {
		body->SetGravityScale(physicsComponentJSON["gravityScale"].asFloat());
	}
	if (physicsComponentJSON.isMember("isBullet")) {
		body->SetBullet(physicsComponentJSON["isBullet"].asBool());
	}
	//Size Override will only apply to an object that has one fixure and it is a box/polygon shape
	bool isSizeOverrideAllowed{};

	if (physicsComponentJSON["fixtures"].size() == 1) {
		
		auto checkCollisionShape = game->enumMap()->toEnum(physicsComponentJSON["fixtures"][0]["collisionShape"].asString());

		if (checkCollisionShape == b2Shape::e_polygon && sizeOverride != b2Vec2_zero) {

			isSizeOverrideAllowed = true;

		}
	}

	//Build fixtures
	for (const auto& fixtureJSON : physicsComponentJSON["fixtures"]) {

		float xOffset{};
		float yOffset{};

		b2Shape* shape;
		b2PolygonShape box;
		b2CircleShape circle;
		b2ChainShape chain;
		b2EdgeShape edge;

		auto collisionShape = game->enumMap()->toEnum(fixtureJSON["collisionShape"].asString());
		//default
		shape = &box;

		//See if we have an offset value
		if (fixtureJSON.isMember("offset")) {

			auto offsetJSON = fixtureJSON["offset"];
			if (offsetJSON.isMember("x")) {
				xOffset = fixtureJSON["offset"]["x"].asFloat();
				util::toBox2dPoint(xOffset);
			}
			if (offsetJSON.isMember("y")) {
				yOffset = fixtureJSON["offset"]["y"].asFloat();
				util::toBox2dPoint(yOffset);
			}

		}

		if (collisionShape == b2Shape::e_circle)
		{
			circle.m_radius = fixtureJSON["collisionRadius"].asFloat();
			circle.m_p.Set(xOffset, yOffset);
			shape = &circle;
		}
		else if (collisionShape == b2Shape::e_polygon) {

			float sizeX{};
			float sizeY{};
			//If a size is not specified for the fixture then default to the transform size of the object
			//Also, apply the override if there is one
			if (isSizeOverrideAllowed) {
				sizeX = sizeOverride.x;
				sizeY = sizeOverride.y;
			}
			else if (fixtureJSON.isMember("size")) {
				sizeX = fixtureJSON["size"]["width"].asFloat();
				sizeY = fixtureJSON["size"]["height"].asFloat();
			}
			else {
				sizeX = transformComponentJSON["size"]["width"].asFloat();
				sizeY = transformComponentJSON["size"]["height"].asFloat();
			}

			//Convert to what box2d needs
			sizeX = util::toBox2dPoint(sizeX) / 2;
			sizeY = util::toBox2dPoint(sizeY) / 2;

			box.SetAsBox(sizeX, sizeY);
			b2Vec2 offsetLocation = { xOffset, yOffset };
			box.SetAsBox(sizeX, sizeY, offsetLocation, 0);
			shape = &box;
		}
		else if (collisionShape == b2Shape::e_chain) {

			PhysicsChainType physicsChainType = (PhysicsChainType)game->enumMap()->toEnum(fixtureJSON["physicsChainType"].asString());

			float sizeX{};
			float sizeY{};
			//If a size is not specified for the fixture then default to the transform size of the object
			if (fixtureJSON.isMember("size")) {
				sizeX = fixtureJSON["size"]["width"].asFloat() / GameConfig::instance().scaleFactor();
				sizeY = fixtureJSON["size"]["height"].asFloat() / GameConfig::instance().scaleFactor();
			}
			else {
				sizeX = transformComponentJSON["size"]["width"].asFloat() / GameConfig::instance().scaleFactor();
				sizeY = transformComponentJSON["size"]["height"].asFloat() / GameConfig::instance().scaleFactor();
			}

			b2Vec2 chainVs[4];
			if (physicsChainType == PhysicsChainType::CW_REFLECT_OUT) {
				chainVs[0].Set(-sizeX/2, -sizeY/2);
				chainVs[1].Set(sizeX/2 , -sizeY/2);
				chainVs[2].Set(sizeX/2, sizeY/2);
				chainVs[3].Set(-sizeX/2, sizeY/2);
			}
			else if (physicsChainType == PhysicsChainType::CCW_REFLECT_IN) {
				chainVs[0].Set(-sizeX / 2, sizeY / 2);
				chainVs[1].Set(sizeX / 2, sizeY / 2);
				chainVs[2].Set(sizeX / 2, -sizeY / 2);
				chainVs[3].Set(-sizeX / 2, -sizeY / 2);
			}

			chain.CreateLoop(chainVs, 4);
			shape = &chain;
		}

		b2FixtureDef fixtureDef;
		fixtureDef.shape = shape;
		fixtureDef.friction = fixtureJSON["friction"].asFloat();
		fixtureDef.density = fixtureJSON["density"].asFloat();
		fixtureDef.restitution = fixtureJSON["restitution"].asFloat();
		fixtureDef.isSensor = fixtureJSON["isSensor"].asFloat();

		ContactDefinition* contactDefinition = new ContactDefinition();
		contactDefinition->contactTag = game->enumMap()->toEnum(fixtureJSON["contactTag"].asString());
		contactDefinition->saveOriginalContactTag = game->enumMap()->toEnum(fixtureJSON["contactTag"].asString());
		fixtureDef.userData.pointer = reinterpret_cast<uintptr_t>(contactDefinition);

		body->CreateFixture(&fixtureDef);

	}

	return body;

}

uint16 PhysicsComponent::_setCollisionMask(Json::Value physicsComponentJSON)
{
	uint16 mask = 0;

	return mask;

}


void PhysicsComponent::applyImpulse(float force, b2Vec2 trajectory)
{

	trajectory.x *= force;
	trajectory.y *= force;

	//m_physicsBody->ApplyLinearImpulseToCenter(trajectory, true);
	m_physicsBody->ApplyLinearImpulseToCenter(trajectory, true);

}

void PhysicsComponent::applyImpulse(float speed, int direction, int strafeDirection)
{
	b2Vec2 trajectory = { (float)strafeDirection, (float)direction };
	trajectory.Normalize();

	trajectory *= speed;

	//apply point
	b2Vec2 applyPoint{2, -2};

	//m_physicsBody->ApplyLinearImpulseToCenter(trajectory, true);
	m_physicsBody->ApplyForce(trajectory, applyPoint, true);

}

void PhysicsComponent::applyMovement(float velocity, b2Vec2 trajectory)
{

	trajectory.x *= velocity;
	trajectory.y *= velocity;

	//m_physicsBody->ApplyForceToCenter(trajectory, true);
	m_physicsBody->SetLinearVelocity(trajectory);

}

void PhysicsComponent::applyAngleImpulse(float force)
{

	m_physicsBody->ApplyAngularImpulse(force,false);
	//m_physicsBody->ApplyForceToCenter(trajectory, true);

}

b2MouseJoint* PhysicsComponent::createB2MouseJoint()
{
	b2MouseJointDef* mouseJointDef;

	//Find the cage object that should exist for 
	const auto& levelCageObject = parent()->parentScene()->getFirstGameObjectByType("LEVEL_CAGE");
	const auto& cagePhysicsComponent = levelCageObject.value().get()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	mouseJointDef = new b2MouseJointDef();
	mouseJointDef->bodyA = cagePhysicsComponent->physicsBody();
	mouseJointDef->bodyB = m_physicsBody;
	mouseJointDef->target = m_physicsBody->GetPosition();
	mouseJointDef->maxForce = 1000.0f;
	mouseJointDef->stiffness = 5000;

	b2MouseJoint* mouseJoint = static_cast<b2MouseJoint*>(parent()->parentScene()->physicsWorld()->CreateJoint(mouseJointDef));

	return mouseJoint;

}

//void PhysicsComponent::applyMovement(float speed, int direction, int strafeDirection)
//{
//
//
//	//Calc direction XY
//	//float dx = cos(this->physicsBody->GetAngle()) * velocity * this->direction; // X-component.
//	//float dy = sin(this->physicsBody->GetAngle()) * velocity * this->direction; // Y-component.
//	float dx = (float)cos(1.5708) * speed * direction; // X-component.
//	float dy = (float)sin(1.5708) * speed * direction; // Y-component.
//
//	//calc strafe xy and add direction and strafe vectors
//	//1.5708 is 90 degrees
//	//float sx = cos(this->physicsBody->GetAngle() + 1.5708) * velocity * this->strafe; // X-component.
//	//float sy = sin(this->physicsBody->GetAngle() + 1.5708) * velocity * this->strafe; // Y-component.
//	float sx = (float)cos((1.5708) + 1.5708) * speed * strafeDirection; // X-component.
//	float sy = (float)sin((1.5708) + 1.5708) * speed * strafeDirection; // Y-component.
//
//	//Create the vector for forward/backward  direction
//	b2Vec2 directionVector = b2Vec2(dx, dy);
//
//	//Create the vector for strafe direction
//	b2Vec2 strafeVector = b2Vec2(sx, sy);
//
//	//Initialize new final movement vector
//	b2Vec2 vec2;
//	vec2.SetZero();
//
//	vec2 = (directionVector + strafeVector);
//
//	//this->physicsBody->SetTransform(vec3, this->physicsBody->GetAngle());
//	std::cout << "applyMovement " << vec2.x << " " << vec2.y << "\n";
//	m_physicsBody->SetLinearVelocity(vec2);
//
//	//m_physicsBody->ApplyLinearImpulseToCenter(vec2, true);
//
//
//}

void PhysicsComponent::applyMovement(float speed, int direction, int strafeDirection)
{

	b2Vec2 trajectory = { (float)strafeDirection, (float)direction  };
	trajectory.Normalize();

	trajectory *= speed;

	m_physicsBody->SetLinearVelocity(trajectory);


}

void PhysicsComponent::applyMovement(b2Vec2 velocity)
{

	m_physicsBody->ApplyLinearImpulseToCenter(velocity, true);


}

void PhysicsComponent::applyRotation(float angularVelocity)
{
	m_physicsBody->SetAngularVelocity(angularVelocity);
}

void PhysicsComponent::applyTorque(float angularVelocity)
{
	m_physicsBody->ApplyTorque(angularVelocity, true);
}

//void PhysicsComponent::stash()
//{
//	b2Vec2 velocityVector = b2Vec2(0, 0);
//	b2Vec2 positionVector = b2Vec2(-50, -50);
//
//	m_physicsBody->SetTransform(positionVector, 0);
//	m_physicsBody->SetLinearVelocity(velocityVector);
//	m_physicsBody->SetEnabled(false);
//}

void PhysicsComponent::deleteAllJoints()
{
	std::vector< b2JointEdge*> joints{};

	//We need to store the joints in an array first , then delete them
	//deleting them as you go will mess up the ->next
	for (b2JointEdge* joint = physicsBody()->GetJointList(); joint; joint = joint->next)
	{
		joints.push_back(joint);
	}

	for (auto joint : joints) {

		parent()->parentScene()->physicsWorld()->DestroyJoint(joint->joint);

	}


}

void PhysicsComponent::attachItem(GameObject* attachObject, b2JointType jointType, b2Vec2 attachLocation)
{
	b2JointDef* jointDef=nullptr;
	b2WeldJointDef* weldJointDef;
	b2RevoluteJointDef* revoluteJointDef;

	//Get physics component of the attachment object
	const auto& attachObjectPhysicsComponent = attachObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	//Get attachment anchor point
	b2Vec2 attachObjectAnchorPoint = {
	attachObjectPhysicsComponent->m_objectAnchorPoint.x,
	attachObjectPhysicsComponent->m_objectAnchorPoint.y
	};

	//Build specific joint
	if (jointType == b2JointType::e_weldJoint) {
		weldJointDef = new b2WeldJointDef();
		weldJointDef->bodyA = m_physicsBody;
		weldJointDef->bodyB = attachObjectPhysicsComponent->m_physicsBody;
		weldJointDef->collideConnected = false;
		weldJointDef->localAnchorA = attachLocation;
		weldJointDef->localAnchorB = attachObjectAnchorPoint;
		jointDef = weldJointDef;
	}
	else if (jointType == b2JointType::e_revoluteJoint) {
		revoluteJointDef = new b2RevoluteJointDef();
		revoluteJointDef->bodyA = m_physicsBody;
		revoluteJointDef->bodyB = attachObjectPhysicsComponent->m_physicsBody;
		revoluteJointDef->collideConnected = false;
		revoluteJointDef->localAnchorA = attachLocation;
		revoluteJointDef->localAnchorB = attachObjectAnchorPoint;

		jointDef = revoluteJointDef;
	}

	parent()->parentScene()->physicsWorld()->CreateJoint(jointDef);

}

void PhysicsComponent::setFixedRotation(bool fixedRotation)
{
	m_physicsBody->SetFixedRotation(fixedRotation);
}

void PhysicsComponent::setBullet(bool isBullet)
{
	m_physicsBody->SetBullet(isBullet);
}


