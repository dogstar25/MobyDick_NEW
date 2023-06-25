#include "ChildrenComponent.h"
#include "../EnumMap.h"
#include "../GameObject.h"
#include "../game.h"
#include "../EnumMap.h"

extern std::unique_ptr<Game> game;

ChildrenComponent::ChildrenComponent()
{

}

ChildrenComponent::ChildrenComponent(Json::Value componentJSON, std::string parentName, Scene* parentScene)
{


	m_componentType = ComponentTypes::CHILDREN_COMPONENT;

	m_childPadding = componentJSON["childPadding"].asFloat();
	m_childPositionRelative = componentJSON["childPositionRelative"].asBool();
	std::optional<int> locationSlot{};
	bool centeredOnLocation{true};

	m_isDependentObjectOwner = true;
	std::optional<b2Vec2> sizeOverride{};

	int childCount{};
	for (Json::Value itrChild : componentJSON["childObjects"])
	{
		childCount++;

		std::string childObjectType = itrChild["gameObjectType"].asString();

		//Slot
		std::optional<int> locationSlot{};
		if (itrChild.isMember("locationSlot")) {
			locationSlot = itrChild["locationSlot"].asInt();
		}

		//Slot alignment
		PositionAlignment positionAlignment{ PositionAlignment::CENTER };
		if (itrChild.isMember("alignment")) {
			positionAlignment = static_cast<PositionAlignment>(game->enumMap()->toEnum(itrChild["alignment"].asString()));
		}

		//Absolute position
		std::optional<b2Vec2> absolutePosition{};
		if (itrChild.isMember("absolutePosition")) {
			absolutePosition = { itrChild["absolutePosition"]["x"].asFloat() , itrChild["absolutePosition"]["y"].asFloat() };
		}

		if (itrChild.isMember("centeredOnLocation")) {
			centeredOnLocation = itrChild["centeredOnLocation"].asBool();
		}

		//Size override
		if (itrChild.isMember("size")) {
			sizeOverride = { itrChild["size"]["width"].asFloat(), itrChild["size"]["height"].asFloat() };
		}


		Child child{};
		ChildLocation location{};

		//Existence of a slot value overrides the absolute positioning
		if (locationSlot.has_value()) {
			location.locationType = ChildLocationType::SLOT;
			location.slot = locationSlot.value();
			location.positionAlignment = positionAlignment;

			m_childSlotCount[location.slot - (int)1] += 1;
		}
		else {
			location.locationType = ChildLocationType::ABSOLUTE_POSITION;
			if (absolutePosition.has_value()) {
				location.absolutePosition = absolutePosition.value();
			}
		}

		location.centeredOnLocation = centeredOnLocation;
		child.location = location;
		std::string name = _buildChildName(parentName, childCount);
		auto gameObject = std::make_shared<GameObject>(childObjectType, -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN, false, name );

		//Was there a sizeOverride
		if (sizeOverride.has_value()) {
			gameObject.get()->setSize(sizeOverride.value());
		}

		child.gameObject = gameObject;
		m_childObjects.push_back(child);

		//Add index 
		parentScene->addGameObjectIndex(gameObject);

	}
}


ChildrenComponent::~ChildrenComponent()
{

}


void ChildrenComponent::update()
{

	short locationSlot = 0;
	b2Vec2 newChildPosition{};
	int childCount{};
	
	for (auto& childObject : m_childObjects)
	{
		childCount++;

		const auto& transformComponent = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
		const auto& childTransformComponent = childObject.gameObject->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);

		//Calculate child position based on what type of location the child uses
		SDL_FPoint parentCenterPosition = transformComponent->getCenterPosition();
		float parentAngle = transformComponent->angle();

		if (childObject.location.locationType == ChildLocationType::SLOT) {
			newChildPosition = _calcChildPosition(childTransformComponent->size(), childCount, childObject.location, parentCenterPosition, parentAngle);
		}
		else if (childObject.location.locationType == ChildLocationType::ABSOLUTE_POSITION) {

			newChildPosition = _calcChildPosition(childTransformComponent->size(), childObject.location, parentCenterPosition, parentAngle);

		}
	
		// Should this child match the angle of the parent
		if (m_childPositionRelative == true)
		{
			childObject.gameObject->setPosition(newChildPosition, transformComponent->angle());
	
		}
		else
		{
			childObject.gameObject->setPosition(newChildPosition, -1);
		}
	
		//Since the child is a game object itself, call the update function for it
		//This acts as a recursive call when you have children objects 
		//within children objects
		childObject.gameObject->update();
	}

	_removeFromWorldPass();
	
}

void ChildrenComponent::_removeFromWorldPass()
{
	//First remove any pieces that were mared to be removed
	auto it = m_childObjects.begin();
	while (it != m_childObjects.end()) {

		if (it->gameObject->removeFromWorld() == true) {

			//Remove object from gloabl index collection
			parent()->parentScene()->deleteIndex(it->gameObject->id());

			//it->pieceObject->reset();
			//std::cout << "Erased from Children collection " << it->gameObject->id() << std::endl;
			it = m_childObjects.erase(it);
		}
		else {
			++it;
		}
	}

	m_childObjects.shrink_to_fit();

}
void ChildrenComponent::postInit()
{

	//Set the layer for these pieces using the parents layer
	for (auto& child : m_childObjects) {

		child.gameObject->setLayer(parent()->layer());
		child.gameObject->postInit();
		child.gameObject->setParent(parent());

		//Child Size Override
		GameObjectDefinition gameObjectDefinition = parent()->gameObjectDefinition();
		Json::Value componentDefinition = util::getComponentConfig(gameObjectDefinition.definitionJSON(), ComponentTypes::CHILDREN_COMPONENT);
		if (componentDefinition.isMember("size")) {

			assert(!child.gameObject->hasComponent(ComponentTypes::PHYSICS_COMPONENT) && "Cannot override the GameObject size if it is a physics object");

			b2Vec2 size = { componentDefinition ["size"]["width"].asFloat(), componentDefinition["size"]["height"].asFloat() };
			child.gameObject->setSize(size);

		}

	}


}

void ChildrenComponent::render()
{
	//Loop through any possible child objects, in all 9 positions, and render them too
	for (auto& childObject : m_childObjects)
	{
		childObject.gameObject->render();
	}

}


b2Vec2 ChildrenComponent::_calcChildPosition(b2Vec2 childSize, int childCount, ChildLocation location, SDL_FPoint parentPosition, float parentAngle)
{
	float x=0, y=0, xAdj = 0, yAdj = 0;

	//Different calculations for the different 9 possible positions
	//Calculate top left corner of child
	switch (location.slot) {
	case 1:
		x = parentPosition.x - childSize.x;
		y = parentPosition.y - childSize.y;
		break;
	case 2:
		x = parentPosition.x;
		y = parentPosition.y - (childSize.y);
		break;
	case 3:
		x = parentPosition.x + childSize.x;
		y = parentPosition.y - (childSize.y);
		break;
	case 4:
		x = parentPosition.x - (childSize.x);
		y = parentPosition.y;
		break;
	case 5:
		x = parentPosition.x;
		y = parentPosition.y;
		break;
	case 6:
		x = parentPosition.x + childSize.x;
		y = parentPosition.y;
		break;
	case 7:
		x = parentPosition.x - (childSize.x);
		y = parentPosition.y + childSize.y;
		break;
	case 8:
		x = parentPosition.x;
		y = parentPosition.y + childSize.y;
		break;
	case 9:
		x = parentPosition.x + childSize.x;
		y = parentPosition.y + childSize.y;
		break;

	}

	SDL_FPoint childCenterPosition{ x, y };

	//Adjust the position if there are multiple children in the same position
	if (m_childSlotCount[location.slot-1] > 1)
	//if (false)
	{
		float oddEvenadjustValue = 0;
		int stepCount = 0;
		b2Vec2 firstChildPosition = {};

		//calculate vertical step adjustment depending on even or odd
		if (m_childSlotCount[location.slot-1] % 2 == 0)
		{
			//isEvenNumber
			oddEvenadjustValue = (childSize.y + m_childPadding) / 2;
		}
		else
		{
			oddEvenadjustValue = childSize.y + m_childPadding;
		}

		//calculate number of steps to take to place 1st child object
		stepCount = m_childSlotCount[location.slot-1] / 2;

		//Calculate 1st child object position based on the previous childPosition calculated
		//values based on location slot
		firstChildPosition.x = childCenterPosition.x;
		firstChildPosition.y = 
			childCenterPosition.y -
			oddEvenadjustValue -
			((childSize.y + m_childPadding) * stepCount);

		//Calculate our current child object position using the stepSize and the
		//position of the first child position
		childCenterPosition.x = firstChildPosition.x;
		childCenterPosition.y =
			firstChildPosition.y + ((childSize.y + m_childPadding) * childCount);

	}

	if (m_childPositionRelative == true)
	{
		b2Vec2 adjustment{};

		adjustment = util::matchParentRotation(childCenterPosition, parentPosition, parentAngle);

		childCenterPosition.x += adjustment.x;
		childCenterPosition.y += adjustment.y;

	}

	b2Vec2 b2Vec2ChildPosition{};
	if (location.centeredOnLocation == true) {
		b2Vec2ChildPosition = { childCenterPosition.x, childCenterPosition.y };
	}
	else {
		b2Vec2ChildPosition = { childCenterPosition.x + (childSize.x/2), childCenterPosition.y + (childSize.y / 2) };
	}
	return b2Vec2ChildPosition;

}


//b2Vec2 ChildrenComponent::_applyAlignment(b2Vec2 childSize, b2Vec2 childPosition, PositionAlignment positionAlignment)
//{
//	float x{};
//	float y{};
//
//	if (location.positionAlignment == PositionAlignment::TOP_LEFT) {
//
//		x = 0;
//		y = 0;
//
//	}
//	else if (location.positionAlignment == PositionAlignment::TOP_CENTER) {
//
//		x = parentPosition.x;
//		y = 0;
//
//	}
//
//}

b2Vec2 ChildrenComponent::_calcChildPosition(b2Vec2 childSize, ChildLocation location, SDL_FPoint parentPosition, float parentAngle)
{
	b2Vec2 position{};

	if (location.centeredOnLocation == true) {
		position.x = parentPosition.x + location.absolutePosition.x;
		position.y = parentPosition.y + location.absolutePosition.y;
	}
	else {
		position.x = parentPosition.x + location.absolutePosition.x + (childSize.x / 2);
		position.y = parentPosition.y + location.absolutePosition.y + (childSize.y / 2);

	}

	return position;


}

std::string ChildrenComponent::_buildChildName(std::string parentName, int childCount)
{

	auto name = std::format("{}_CH{:03}", parentName, childCount);

	return name;

}

//b2Vec2 ChildrenComponent::_calcChildPosition2(
//	b2Vec2 childSize,
//	int locationSlot,
//	int locationAlignment,
//	int childNumber,
//	int childCount,
//	SDL_FPoint parentCenterPosition,
//	float parentAngle)
//{
//	//SDL_FRect childSize = { child->size().x, child->size().y };
//	//SDL_FPoint childPosition{};
//	float x = 0, y = 0, xAdj = 0, yAdj = 0;
//
//	//Different calculations for the different 9 possible positions
//	//Calculate top left corner of child
//	switch (locationSlot) {
//	case 1:
//		x = parentCenterPosition.x - (childSize.x);
//		y = parentCenterPosition.y - (childSize.y);
//		break;
//	case 2:
//		x = parentCenterPosition.x;
//		y = parentCenterPosition.y - (childSize.y);
//		break;
//	case 3:
//		x = parentCenterPosition.x + childSize.x;
//		y = parentCenterPosition.y - (childSize.y);
//		break;
//	case 4:
//		x = parentCenterPosition.x - (childSize.x);
//		y = parentCenterPosition.y;
//		break;
//	case 5:
//		x = parentCenterPosition.x;
//		y = parentCenterPosition.y;
//		break;
//	case 6:
//		x = parentCenterPosition.x + childSize.x;
//		y = parentCenterPosition.y;
//		break;
//	case 7:
//		x = parentCenterPosition.x - (childSize.x);
//		y = parentCenterPosition.y + childSize.y;
//		break;
//	case 8:
//		x = parentCenterPosition.x;
//		y = parentCenterPosition.y + childSize.y;
//		break;
//	case 9:
//		x = parentCenterPosition.x + childSize.x;
//		y = parentCenterPosition.y + childSize.y;
//		break;
//
//	}
//
//	SDL_FPoint childCenterPosition{ x, y };
//
//	//Adjust the position if there are multiple children in the same position
//	if (childCount > 1)
//	{
//		float oddEvenadjustValue = 0;
//		int stepCount = 0;
//		b2Vec2 firstChildPosition = {};
//
//		//calculate vertical step adjustment depending on even or odd
//		if (childCount % 2 == 0)
//		{
//			//isEvenNumber
//			oddEvenadjustValue = (childSize.y + m_childPadding) / 2;
//		}
//		else
//		{
//			oddEvenadjustValue = childSize.y + m_childPadding;
//		}
//
//		//calculate number of steps to take to place 1st child object
//		stepCount = childCount / 2;
//
//		//Calculate 1st child object position based on the previous childPosition calculated
//		//values based on location slot
//		firstChildPosition.x = childCenterPosition.x;
//		firstChildPosition.y =
//			childCenterPosition.y -
//			oddEvenadjustValue -
//			((childSize.y + m_childPadding) * stepCount);
//
//		//Calculate our current child object position using the stepSize and the
//		//position of the first child position
//		childCenterPosition.x = firstChildPosition.x;
//		childCenterPosition.y =
//			firstChildPosition.y + ((childSize.y + m_childPadding) * childNumber);
//
//	}
//
//	if (m_childPositionRelative == true)
//	{
//		b2Vec2 adjustment{};
//
//		adjustment = util::matchParentRotation(childCenterPosition, parentCenterPosition, parentAngle);
//
//		childCenterPosition.x += adjustment.x;
//		childCenterPosition.y += adjustment.y;
//
//	}
//
//	b2Vec2 b2Vec2ChildPosition = { childCenterPosition.x, childCenterPosition.y };
//	return b2Vec2ChildPosition;
//
//}

//void ChildrenComponent::update2()
//{
//
//	short locationSlot = 0;
//
//	for (auto& childLocation : m_childObjects)
//	{
//		locationSlot++;
//		int childNumber = 0;
//		int childCount = (int)childLocation.size();
//
//		for (auto& childObject : childLocation)
//		{
//
//			const auto& transformComponent = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
//			const auto& childTransformComponent = childObject.gameObject->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
//
//			childNumber++;
//
//			//Calculate child position
//			SDL_FPoint parentCenterPosition = transformComponent->getCenterPosition();
//			float parentAngle = transformComponent->angle();
//			b2Vec2 newChildPosition =
//				_calcChildPosition(childTransformComponent->size(), locationSlot, childObject.locationAlignment, childNumber,
//					childCount, parentCenterPosition, parentAngle);
//
//			// Should this child match the angle of the parent
//			if (m_childPositionRelative == true)
//			{
//				childObject.gameObject->setPosition(newChildPosition, transformComponent->angle());
//
//			}
//			else
//			{
//				childObject.gameObject->setPosition(newChildPosition, -1);
//			}
//
//			//Since the child is a game object itself, call the update function for it
//			//This acts as a recursive call when you have children objects 
//			//within children objects
//			childObject.gameObject->update();
//		}
//	}
//
//}
