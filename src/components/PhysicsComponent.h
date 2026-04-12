#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include <optional>

#include <box2d/box2d.h>

#include "Component.h"
#include "../Scene.h"

class TransformComponent;

struct ContactDefinition {
	int contactTag{};
	int originalContactTag{};
};

class PhysicsComponent : public Component
{
	//This is used to force a refilter on contacts for when we disable or enable conatacts
	static constexpr uint32_t kRefilterBit = 0x80000000u;

	friend class StateComponent;

public:
	PhysicsComponent() = default;;
	PhysicsComponent(Json::Value definitionJSON, GameObject* parent, Scene* parentScene, float xMapPos, float yMapPos, float angleAdjust, b2Vec2 sizeOverride);
	~PhysicsComponent();

	void update() override;
	void postInit() override;
	//void setParent(GameObject* gameObject) override;

	void applyMovement(float velocity, int direction, int strafeDirection);
	void applyMovement(float velocity, b2Vec2 trajectory);
	void applyMovement(b2Vec2 trajectory);
	void applyImpulse(float force, b2Vec2 trajectory);
	void applyImpulse(float speed, int direction, int strafeDirection);
	void applyAngleImpulse(float force);
	void applyRotation(float angularVelocity);
	void applyTorque(float angularVelocity);
	void setTransform(b2Vec2 positionVector, float angle);
	void setTransform(b2Vec2 positionVector);
	void setLinearVelocity(b2Vec2 velocityVector);
	void enableAllContacts();
	void disableAllContacts();

	void destroyJoint(b2JointId jointId);
	
	void setFixedRotation(bool fixedRotation);
	void setBullet(bool isBullet);
	void setAngle(float angle);
	void setLinearDamping(float linearDamping);
	void setAngularDamping(float angularDamping);
	void setGravityScale(float gravityScale);
	void setIsSensor(bool isSensor);
	void setOriginalPhysicsType(b2BodyType bodyType) { m_originalPhysicsType = bodyType; }

	void attachItem(GameObject* inventoryObject, b2JointType jointType, b2Vec2 attachLocation = {0,0});
	void deleteAllJoints();
	b2JointId createB2MouseJoint();
	void setCollisionTag(int contactTag);

	//Accessor functions
	b2Vec2 objectAnchorPoint() { return m_objectAnchorPoint; }
	b2Vec2 position();
	float angle();
	b2BodyId physicsBodyId() {	return m_physicsBodyId; }
	bool isTouchingObjectsCapturedRequired() { return m_touchingObjectsCapturedRequired; }
	b2BodyType originalPhysicsType() { return m_originalPhysicsType; }
	b2Vec2 linearVelocity();

	void changePhysicsBodyType(b2BodyType m_physicsType, bool changeOriginal=false);

private:

	void setPhysicsBodyActive(bool  active);

	b2BodyId _buildB2Body(Json::Value physicsComponentJSON, Json::Value transformComponentJSON, b2WorldId physicsWorldId, b2Vec2 sizeOverride);
	uint16_t _setCollisionMask(Json::Value physicsComponentJSON);
	void _touchShapeForRefilter(b2ShapeId shape);

	b2BodyId m_physicsBodyId{ 0 };
	b2BodyType m_physicsType{};
	b2BodyType m_originalPhysicsType{};
	b2Vec2 m_objectAnchorPoint{ 0 , 0 };
	bool m_touchingObjectsCapturedRequired{ true };
	static const int m_maxBodyShapes{ 16 };
	static const int m_maxBodyJoints{ 16 };

	std::vector<std::unique_ptr<ContactDefinition>> m_contactDefs;

};

#endif