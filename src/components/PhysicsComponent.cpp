#include "PhysicsComponent.h"


#include "../EnumMap.h"
#include "../Game.h"
#include "../include/GameGlobals.h"

//extern std::unique_ptr<Game> game;

PhysicsComponent::PhysicsComponent(Json::Value definitionJSON, GameObject* parent, Scene* parentScene, float xMapPos, float yMapPos, 
	float angleAdjust,	b2Vec2 sizeOverride) :
	Component(ComponentTypes::PHYSICS_COMPONENT, parent)
{

	//Get reference to the animationComponent JSON config and transformComponent JSON config
	Json::Value physicsComponentJSON = util::getComponentConfig(definitionJSON, ComponentTypes::PHYSICS_COMPONENT);
	Json::Value transformComponentJSON = util::getComponentConfig(definitionJSON, ComponentTypes::TRANSFORM_COMPONENT);

	m_physicsType = m_originalPhysicsType = (b2BodyType)game->enumMap()->toEnum(physicsComponentJSON["type"].asString());

	m_objectAnchorPoint = { physicsComponentJSON["anchorPoint"]["x"].asFloat(),	physicsComponentJSON["anchorPoint"]["y"].asFloat() };

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

	m_contactDefs.clear();

	if (b2Body_IsValid(m_physicsBodyId)) {

		b2ShapeId shapeArray[m_maxBodyShapes];

		int shapeCount = b2Body_GetShapes(m_physicsBodyId, shapeArray, m_maxBodyShapes);
		for (int i = 0; i < shapeCount; i++)
		{
			b2Shape_SetUserData(shapeArray[i], nullptr);
		}

		b2Body_SetUserData(m_physicsBodyId, nullptr);

		b2DestroyBody(m_physicsBodyId);
	}

}

void PhysicsComponent::setCollisionTag(int contactTag)
{

	b2ShapeId shapeArray[m_maxBodyShapes];

	int shapeCount = b2Body_GetShapes(m_physicsBodyId, shapeArray, m_maxBodyShapes);
	for (int i = 0; i < shapeCount; i++)
	{
		ContactDefinition* contactDefinition = reinterpret_cast<ContactDefinition*>(b2Shape_GetUserData(shapeArray[i]));
		if (contactDefinition) {
			contactDefinition->contactTag = contactTag;
		}
	}

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

void PhysicsComponent::changePhysicsBodyType(b2BodyType physicsType, bool changeOriginal)
{
	m_physicsType = physicsType;
	b2Body_SetType(m_physicsBodyId, physicsType);

	//Changing the original means that we are permanantly changing this body type
	if (changeOriginal) {
		m_originalPhysicsType = m_physicsType;
	}
}

void PhysicsComponent::setPhysicsBodyActive(bool  active)
{

	b2Body_Enable(m_physicsBodyId);

}

void PhysicsComponent::setLinearVelocity(b2Vec2 velocityVector)
{

	b2Body_SetLinearVelocity(m_physicsBodyId, velocityVector);

}

void PhysicsComponent::enableAllContacts()
{

	b2ShapeId shapeArray[m_maxBodyShapes];

	int shapeCount = b2Body_GetShapes(m_physicsBodyId, shapeArray, m_maxBodyShapes);
	for (int i = 0; i < shapeCount; i++)
	{

		auto userData = b2Shape_GetUserData(shapeArray[i]);
		ContactDefinition* contactDefinition = static_cast<ContactDefinition*>(userData);
		contactDefinition->contactTag = contactDefinition->originalContactTag;
		_touchShapeForRefilter(shapeArray[i]);

	}

	b2Body_SetAwake(m_physicsBodyId, true);
}

void PhysicsComponent::_touchShapeForRefilter(b2ShapeId shape)
{
	b2Filter f = b2Shape_GetFilter(shape);
	// Flip a no-op category bit to force a change
	f.categoryBits ^= kRefilterBit;
	b2Shape_SetFilter(shape, f); // forces refilter, destroys stale contacts
}

void PhysicsComponent::disableAllContacts()
{

	b2ShapeId shapeArray[m_maxBodyShapes];

	int shapeCount = b2Body_GetShapes(m_physicsBodyId, shapeArray, m_maxBodyShapes);
	for (int i = 0; i < shapeCount; i++)
	{

		auto userData = b2Shape_GetUserData(shapeArray[i]);
		ContactDefinition* contactDefinition = static_cast<ContactDefinition*>(userData);
		contactDefinition->contactTag = ContactTag::GENERAL_FREE;
		_touchShapeForRefilter(shapeArray[i]);

	}

	b2Body_SetAwake(m_physicsBodyId, true);

}

void PhysicsComponent::destroyJoint(b2JointId jointId)
{
	b2DestroyJoint(jointId);
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

void PhysicsComponent::setIsSensor(bool isSensor)
{

	//need to find a way to live without this
	//Game items for example, could have to shapes on the body. one for collision and one for sensor.
	//we could then change the collision filter of the collision shape and or sensor shape

}


void PhysicsComponent::update()
{
	
	b2Vec2 currentPosition = b2Body_GetPosition(m_physicsBodyId);

	//Handle Absolute positioned objects
	if (parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT)->absolutePositioning() == true) {

		SDL_FPoint cameraPosition = { Camera::instance().frame().x, Camera::instance().frame().y };
		SDL_FPoint convertedCameraPosition = util::toBox2dPoint(cameraPosition);

		currentPosition = { currentPosition.x + convertedCameraPosition.x, currentPosition.y + convertedCameraPosition.y};

		b2Body_SetTransform(m_physicsBodyId, currentPosition, b2Body_GetRotation(m_physicsBodyId));
	}

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
	
	if (physicsComponentJSON.isMember("linearDamping")) {
		bodyDef.linearDamping = physicsComponentJSON["linearDamping"].asFloat();
	}
	if (physicsComponentJSON.isMember("angularDamping")) {
		bodyDef.angularDamping = physicsComponentJSON["angularDamping"].asFloat();
	}
	if (physicsComponentJSON.isMember("gravityScale")) {
		bodyDef.gravityScale = physicsComponentJSON["gravityScale"].asFloat();
	}
	if (physicsComponentJSON.isMember("isBullet")) {
		bodyDef.isBullet = physicsComponentJSON["isBullet"].asBool();
	}
	if (physicsComponentJSON.isMember("isFixedRotation")) {
		bodyDef.fixedRotation = physicsComponentJSON["isFixedRotation"].asBool();
	}

	b2BodyId bodyId = b2CreateBody(physicsWorldId, &bodyDef);

	//Withbox2d V3 bodies that are still will go to sleep causing them to not get picked up as sesorEvents
	b2Body_EnableSleep(bodyId, false);

	//Store a reference to the parent object in the userData pointer field
	b2Body_SetUserData(bodyId, parent());

	//Size Override will only apply to an object that has one fixure and it is a box/polygon shape
	bool isSizeOverrideAllowed{};

	if (physicsComponentJSON["shapes"].size() == 1) {
		
		auto checkCollisionShape = game->enumMap()->toEnum(physicsComponentJSON["shapes"][0]["collisionShape"].asString());

		if (checkCollisionShape == b2ShapeType::b2_polygonShape && sizeOverride != b2Vec2_zero) {

			isSizeOverrideAllowed = true;

		}
	}

	//Build fixtures
	for (const auto& shapesJSON : physicsComponentJSON["shapes"]) {

		float xOffset{};
		float yOffset{};

		b2ShapeId shapeId;
		b2ChainId chainId;

		//See if we have an offset value
		if (shapesJSON.isMember("offset")) {

			auto offsetJSON = shapesJSON["offset"];
			if (offsetJSON.isMember("x")) {
				xOffset = shapesJSON["offset"]["x"].asFloat();
				util::toBox2dPoint(xOffset);
			}
			if (offsetJSON.isMember("y")) {
				yOffset = shapesJSON["offset"]["y"].asFloat();
				util::toBox2dPoint(yOffset);
			}

		}

		if (parent()->type() == "KEY1") {

			int todd = 1;
		}

		auto collisionShape = game->enumMap()->toEnum(shapesJSON["collisionShape"].asString());

		//The Surface Material structure is shared by all shape types
		b2SurfaceMaterial surfaceMaterial = b2DefaultSurfaceMaterial();
		surfaceMaterial.friction = shapesJSON["friction"].asFloat();
		surfaceMaterial.restitution = shapesJSON["restitution"].asFloat();

		auto contactDefinition = std::make_unique<ContactDefinition>();
		contactDefinition->contactTag = game->enumMap()->toEnum(shapesJSON["contactTag"].asString());
		contactDefinition->originalContactTag = game->enumMap()->toEnum(shapesJSON["contactTag"].asString());
		
		//All shapes except the chain share common code
		if (collisionShape == b2ShapeType::b2_circleShape ||
			collisionShape == b2ShapeType::b2_polygonShape  )
		{
			b2Circle circle;
			b2Polygon box;

			b2ShapeDef shapeDef = b2DefaultShapeDef();
			shapeDef.material = surfaceMaterial;
			shapeDef.density = shapesJSON["density"].asFloat();
			shapeDef.isSensor = shapesJSON["isSensor"].asBool();
			shapeDef.userData = contactDefinition.get();
			if (shapesJSON["disableSensorInvolvement"].asBool() == true) {
				shapeDef.enableSensorEvents = false;
			}
			else {
				shapeDef.enableSensorEvents = true;
			}

			m_contactDefs.push_back(std::move(contactDefinition));

			if (collisionShape == b2ShapeType::b2_circleShape)
			{
				circle.radius = shapesJSON["collisionRadius"].asFloat();
				circle.center = { xOffset, yOffset };

				b2ShapeId shapeId = b2CreateCircleShape(bodyId, &shapeDef, &circle);

			}
			else if (collisionShape == b2ShapeType::b2_polygonShape) 
			{

				float sizeX{};
				float sizeY{};
				//If a size is not specified for the fixture then default to the transform size of the object
				//Also, apply the override if there is one
				if (isSizeOverrideAllowed) {
					sizeX = sizeOverride.x;
					sizeY = sizeOverride.y;
				}
				else if (shapesJSON.isMember("size")) {
					sizeX = shapesJSON["size"]["width"].asFloat();
					sizeY = shapesJSON["size"]["height"].asFloat();
				}
				else {
					sizeX = transformComponentJSON["size"]["width"].asFloat();
					sizeY = transformComponentJSON["size"]["height"].asFloat();
				}

				//Convert to what box2d needs
				sizeX = util::toBox2dPoint(sizeX) / 2;
				sizeY = util::toBox2dPoint(sizeY) / 2;

				b2Vec2 offsetLocation = { xOffset, yOffset };
				box = b2MakeOffsetBox(sizeX, sizeY, offsetLocation, b2Rot_identity);
				b2ShapeId shapeId = b2CreatePolygonShape(bodyId, &shapeDef, &box);

			}

		}
		else if (collisionShape == b2ShapeType::b2_chainSegmentShape) {

			PhysicsChainType physicsChainType = (PhysicsChainType)game->enumMap()->toEnum(shapesJSON["physicsChainType"].asString());
			b2ChainDef chainDef = b2DefaultChainDef();
			chainDef.enableSensorEvents = true;

			float sizeX{};
			float sizeY{};
			//If a size is not specified for the fixture then default to the transform size of the object
			if (shapesJSON.isMember("size")) {
				sizeX = shapesJSON["size"]["width"].asFloat() / GameConfig::instance().scaleFactor();
				sizeY = shapesJSON["size"]["height"].asFloat() / GameConfig::instance().scaleFactor();
			}
			else {
				sizeX = transformComponentJSON["size"]["width"].asFloat() / GameConfig::instance().scaleFactor();
				sizeY = transformComponentJSON["size"]["height"].asFloat() / GameConfig::instance().scaleFactor();
			}

			b2Vec2 chainVs[4];

			if (physicsChainType == PhysicsChainType::CW_REFLECT_OUT) {

				chainVs[0] = b2Vec2(-sizeX / 2, -sizeY / 2);
				chainVs[1] = b2Vec2(sizeX / 2, -sizeY / 2);
				chainVs[2] = b2Vec2(sizeX / 2,  sizeY / 2);
				chainVs[3] = b2Vec2(-sizeX / 2,  sizeY / 2);
			}
			else if (physicsChainType == PhysicsChainType::CCW_REFLECT_IN) {
				
				chainVs[0] = b2Vec2(-sizeX / 2, sizeY / 2);
				chainVs[1] = b2Vec2(sizeX / 2, sizeY / 2);
				chainVs[2] = b2Vec2(sizeX / 2, -sizeY / 2);
				chainVs[3] = b2Vec2(-sizeX / 2, -sizeY / 2);

			}

			chainDef.points = chainVs;
			chainDef.count = 4;
			chainDef.isLoop = true;

			chainDef.userData = contactDefinition.get();
			m_contactDefs.push_back(std::move(contactDefinition));

			b2ChainId chainId = b2CreateChain(bodyId, &chainDef);
		}

	}

	return bodyId;

}

uint16_t PhysicsComponent::_setCollisionMask(Json::Value physicsComponentJSON)
{
	uint16_t mask = 0;

	return mask;

}


void PhysicsComponent::applyImpulse(float force, b2Vec2 trajectory)
{

	trajectory.x *= force;
	trajectory.y *= force;

	b2Body_ApplyLinearImpulseToCenter(m_physicsBodyId, trajectory, true);

}

void PhysicsComponent::applyImpulse(float speed, int direction, int strafeDirection)
{
	b2Vec2 trajectory = { (float)strafeDirection, (float)direction };
	trajectory = b2Normalize(trajectory);

	trajectory *= speed;

	//apply point
	b2Vec2 applyPoint{2, -2};

	b2Body_ApplyForce(m_physicsBodyId, trajectory, applyPoint, true);

}

void PhysicsComponent::applyMovement(float speed, b2Vec2 trajectory)
{

	constexpr float kDirectionEpsilon = 1e-4f;

	const auto len = b2Length(trajectory);
	if (len < kDirectionEpsilon) {

		b2Body_SetLinearVelocity(m_physicsBodyId, {0.0f, 0.0f});
		return;

	}

	//normalize
	trajectory.x /= len;
	trajectory.y /= len;

	//Calculate basic velocity
	b2Vec2 velocity = {
		trajectory.x * speed,
		trajectory.y * speed
	};

	//Get objects current velocity
	const b2Vec2 currentLinearVelocity = b2Body_GetLinearVelocity(m_physicsBodyId);

	b2Body_SetLinearVelocity(m_physicsBodyId, velocity);

}

b2Vec2 PhysicsComponent::linearVelocity()
{

	return b2Body_GetLinearVelocity(m_physicsBodyId);

}

void PhysicsComponent::applyAngleImpulse(float force)
{

	b2Body_ApplyAngularImpulse(m_physicsBodyId, force,false);

}

b2JointId PhysicsComponent::createB2MouseJoint()
{
	b2JointId jointId{};

	//Find the cage object that should exist for the world
	const auto& levelCageObject = parent()->parentScene()->getFirstGameObjectByType("LEVEL_CAGE");
	const auto& cagePhysicsComponent = levelCageObject.value().get()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	b2MouseJointDef jointDef = b2DefaultMouseJointDef();
	jointDef.bodyIdA = cagePhysicsComponent->physicsBodyId();
	jointDef.bodyIdB = m_physicsBodyId;
	jointDef.target = b2Body_GetPosition(m_physicsBodyId);
	jointDef.maxForce = 10000.0f;
	jointDef.hertz = 50000;
	jointDef.dampingRatio = 0.7f;
	jointDef.collideConnected = false;

	//i added the IF during DEBUGGING BRO
	if (b2Body_IsValid(jointDef.bodyIdA) && b2Body_IsValid(jointDef.bodyIdB)) {

		jointId = b2CreateMouseJoint(parent()->parentScene()->physicsWorld(), &jointDef);

	}

	return jointId;

}

void PhysicsComponent::applyMovement(float speed, int direction, int strafeDirection)
{

	b2Vec2 trajectory = { (float)strafeDirection, (float)direction  };
	b2Normalize(trajectory);

	trajectory *= speed;

	b2Body_SetLinearVelocity(m_physicsBodyId, trajectory);


}

void PhysicsComponent::applyMovement(b2Vec2 velocity)
{

	b2Body_ApplyLinearImpulseToCenter(m_physicsBodyId, velocity, true);

}

void PhysicsComponent::applyRotation(float angularVelocity)
{
	b2Body_SetAngularVelocity(m_physicsBodyId, angularVelocity);
}

void PhysicsComponent::applyTorque(float angularVelocity)
{
	b2Body_ApplyTorque(m_physicsBodyId, angularVelocity, true);
}

void PhysicsComponent::deleteAllJoints()
{

	b2JointId jointArray[m_maxBodyJoints];

	int jointCount = b2Body_GetJoints(m_physicsBodyId, jointArray, m_maxBodyJoints);
	for (int i=0; i < jointCount; i++) {

		b2DestroyJoint(jointArray[i]);
	}

}

void PhysicsComponent::attachItem(GameObject* attachObject, b2JointType jointType, b2Vec2 attachLocation)
{

	b2WeldJointDef weldJointDef = b2DefaultWeldJointDef();
	b2RevoluteJointDef revoluteJointDef = b2DefaultRevoluteJointDef();

	//Get physics component of the attachment object
	const auto& attachObjectPhysicsComponent = attachObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);

	//Get attachment anchor point
	b2Vec2 attachObjectAnchorPoint = {
		attachObjectPhysicsComponent->m_objectAnchorPoint.x,
		attachObjectPhysicsComponent->m_objectAnchorPoint.y
	};

	//Build specific joint
	if (jointType == b2JointType::b2_weldJoint) {
		weldJointDef.bodyIdA = m_physicsBodyId;
		weldJointDef.bodyIdB = attachObjectPhysicsComponent->physicsBodyId();
		weldJointDef.collideConnected = false;
		weldJointDef.localAnchorA = attachLocation;
		weldJointDef.localAnchorB = attachObjectAnchorPoint;
		b2CreateWeldJoint(parent()->parentScene()->physicsWorld(), &weldJointDef);
	}
	else if (jointType == b2JointType::b2_revoluteJoint) {
		revoluteJointDef.bodyIdA = m_physicsBodyId;
		revoluteJointDef.bodyIdB = attachObjectPhysicsComponent->physicsBodyId();
		revoluteJointDef.collideConnected = false;
		revoluteJointDef.localAnchorA = attachLocation;
		revoluteJointDef.localAnchorB = attachObjectAnchorPoint;
		b2CreateRevoluteJoint(parent()->parentScene()->physicsWorld(), &revoluteJointDef);

	}


}

void PhysicsComponent::setFixedRotation(bool fixedRotation)
{
	b2Body_SetFixedRotation(m_physicsBodyId, fixedRotation);
}

void PhysicsComponent::setBullet(bool isBullet)
{
	b2Body_SetBullet(m_physicsBodyId, isBullet);
}


