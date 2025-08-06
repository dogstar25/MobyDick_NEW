#include "ContactFilter.h"

#include "GameObject.h"
#include "components/PhysicsComponent.h"

std::vector<std::bitset<ContactTag::MAX_OBJECT_CATEGORIES>> ContactFilter::m_contactMasks;

ContactFilter& ContactFilter::instance()
{

	static ContactFilter singletonInstance;
	return singletonInstance;

}

ContactFilter::ContactFilter()
{

	using namespace ContactTag;

	m_contactMasks.resize(ContactTag::MAX_OBJECT_CATEGORIES);

	//General_solid - set all to collide
	m_contactMasks[GENERAL_SOLID].reset();
	m_contactMasks[GENERAL_SOLID].flip();

	//General_free - set all to NOT collide
	m_contactMasks[GENERAL_FREE].reset();

	//Initialize the level frame one
	m_contactMasks[LEVEL_CAGE].reset();

	
}

bool ContactFilter::ShouldCollide(b2ShapeId shapeAId, b2ShapeId shapeBId, void* context)
{

	auto userDataA = b2Shape_GetUserData(shapeAId);
	auto userDataB = b2Shape_GetUserData(shapeBId);

	auto bodyAId = b2Shape_GetBody(shapeAId);
	auto bodyBId = b2Shape_GetBody(shapeBId);

	GameObject* gaemObjectA = static_cast<GameObject*>(b2Body_GetUserData(bodyAId));
	GameObject* gaemObjectB = static_cast<GameObject*>(b2Body_GetUserData(bodyBId));

	ContactDefinition* defA = static_cast<ContactDefinition*>(userDataA);
	ContactDefinition* defB = static_cast<ContactDefinition*>(userDataB);

	int contactTagA = defA->contactTag;
	int contactTagB = defB->contactTag;

	//If one of these objects is GENERAL_SOLID and the other one is NOT GENERAL_FREE then collide
	if ((contactTagA == ContactTag::GENERAL_SOLID && contactTagB != ContactTag::GENERAL_FREE ) || 
		(contactTagB == ContactTag::GENERAL_SOLID && contactTagA != ContactTag::GENERAL_FREE) ){
		return true;
	}

	auto& contactAMask = m_contactMasks[contactTagA];
	auto& contactBMask = m_contactMasks[contactTagB];

	if (contactAMask.test(contactTagB) &&
		contactBMask.test(contactTagA)) {
		return true;
	}
	else {
		return false;
	}

}
