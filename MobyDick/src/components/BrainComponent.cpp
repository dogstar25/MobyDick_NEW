#include "BrainComponent.h"

#include "../game.h"
#include "../QueryAABBHits.h"
#include "../RayCastHits.h"
#include "../EnumMap.h"

#include <algorithm>


extern std::unique_ptr<Game> game;

//BrainComponent::BrainComponent() :
//	Component(ComponentTypes::BRAIN_COMPONENT)
//{
//}

BrainComponent::BrainComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::BRAIN_COMPONENT, parent)
{

	m_sightSensorSize = componentJSON["sightSensorSize"].asInt();

	for (Json::Value traits : componentJSON["detectObjectTraits"])
	{
		uint32_t trait = game->enumMap()->toEnum(traits.asString());
		m_detectObjectTraits.set(trait);
	}


}

BrainComponent::~BrainComponent()
{

	m_seenObjects.clear();
	m_detectedObjects.clear();

}

void BrainComponent::update()
{

	if (m_updateSensorInputTimer.isFirstTime() == false && m_updateSensorInputTimer.hasMetTargetDuration()) {
		_updateSensorInput();
	}

}

void BrainComponent::postInit()
{
}


void BrainComponent::_updateSensorInput()
{

	auto transform = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	auto centerPosition = parent()->getCenterPosition();

	m_detectedObjects.clear();
	m_seenObjects.clear();

	b2Vec2 centerB2 = { centerPosition.x, centerPosition.y};
	util::toBox2dPoint(centerB2);

	b2AABB aabb;
	aabb.lowerBound = b2Vec2(centerB2.x - m_sightSensorSize, centerB2.y - m_sightSensorSize);
	aabb.upperBound = b2Vec2(centerB2.x + m_sightSensorSize, centerB2.y + m_sightSensorSize);
	
	//Make the AABB query
	std::vector<b2ShapeId> queryAABBHits = QueryAABBHits(parent()->parentScene()->physicsWorld(), aabb);

	//Loop through all of the found objects and store the ones that we care about that we have a direct line
	//of sight to
	for (b2ShapeId shapeId : queryAABBHits) {

		b2BodyId bodyId = b2Shape_GetBody(shapeId);
		GameObject* detectedGameObject = static_cast<GameObject*>(b2Body_GetUserData(bodyId));

		for (auto i = 0; i < m_detectObjectTraits.size(); i++) {

			//Is this one we care about and is it NOT our own body being detected
			if (detectedGameObject->traits()[i] && m_detectObjectTraits[i]
				&& (detectedGameObject != parent())) {

				//Get this objects shared pointer from the scene and store it instead of the raw pointer
				std::optional<std::weak_ptr<GameObject>> gameObject = parent()->parentScene()->getGameObject(detectedGameObject->id());

				m_detectedObjects.push_back(gameObject.value());

				auto seenObjectDetails = _hasLineOfSight(detectedGameObject);
				if (seenObjectDetails.has_value() == true) {

					seenObjectDetails.value().gameObject = gameObject.value();
					m_seenObjects.push_back(seenObjectDetails.value());

				}

				break;
			}
		}
	}

}

/*
* This is a ray from a detected object to the Brain Owner
* If no barrier object was found along this ray, then there
* is a clear line of sight
*/
std::optional<SeenObjectDetails> BrainComponent::_hasLineOfSight(GameObject* detectedObject)
{
	bool clearPath{true};

	std::optional<SeenObjectDetails> seenObjectDetails{};

	seenObjectDetails = util::hasLineOfSight(parent(), detectedObject, parent()->parentScene()->physicsWorld());

	if (clearPath) {
		return seenObjectDetails;
	}
	else {
		return std::nullopt;
	}
}
