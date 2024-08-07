#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include <optional>

#include <box2d/box2d.h>

#include "Component.h"
#include "../Scene.h"

class TransformComponent;

struct ContactDefinition {
	int contactTag{};
	int saveOriginalContactTag{};
};

class PhysicsComponent : public Component
{

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
	
	void setFixedRotation(bool fixedRotation);
	void setBullet(bool isBullet);
	void setAngle(float angle);
	void setLinearDamping(float linearDamping);
	void setAngularDamping(float angularDamping);
	void setGravityScale(float gravityScale);
	void setIsSensor(bool isSensor);

	//This is when we want to chnage the position of the object from within a box2d callback
	void setChangePositionPosition(b2Vec2 position) { m_changePositionPosition = position; }
	
	void attachItem(GameObject* inventoryObject, b2JointType jointType, b2Vec2 attachLocation = {0,0});
	void deleteAllJoints();
	b2MouseJoint* createB2MouseJoint();

	//Accessor functions
	b2Vec2 objectAnchorPoint() { return m_objectAnchorPoint; }
	b2Vec2 position() { return m_physicsBody->GetPosition(); }
	float angle() { return m_physicsBody->GetAngle(); }
	b2Body* physicsBody() {	return m_physicsBody; }
	bool isTouchingObjectsCapturedRequired() { return m_touchingObjectsCapturedRequired; }

private:

	void setPhysicsBodyActive(bool  active);

	b2Body* _buildB2Body(Json::Value physicsComponentJSON, Json::Value transformComponentJSON, b2World* physicsWorld, b2Vec2 sizeOverride);
	uint16 _setCollisionMask(Json::Value physicsComponentJSON);

	b2Body* m_physicsBody{ nullptr };
	uint16 m_physicsType{ 0 };
	b2Vec2 m_objectAnchorPoint{ 0 , 0 };
	std::optional<b2Vec2> m_changePositionPosition{};
	bool m_touchingObjectsCapturedRequired{ true };

};

#endif