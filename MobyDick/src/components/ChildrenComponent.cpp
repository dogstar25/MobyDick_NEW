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
	m_matchParentRotation = componentJSON["matchParentRotation"].asBool();
	if (componentJSON.isMember("sameSlotTreatment")) {
		m_childSameSlotTreatment = (ChildSlotTreatment)game->enumMap()->toEnum(componentJSON["sameSlotTreatment"].asString());
	}

	std::optional<int> locationSlot{};
	bool centeredOnLocation{true};

	m_isDependentObjectOwner = true;
	std::optional<b2Vec2> sizeOverride{};

	//Build all possible Standard Slot - they will be empty
	//_buildStandardSlots();

	//Build all possible Grid Slots - they will be empty
	//_buildGridSlots();

	int childCount{};
	for (Json::Value itrChild : componentJSON["childObjects"])
	{
		childCount++;

		std::string childObjectType = itrChild["gameObjectType"].asString();
		std::string name = _buildChildName(parentName, childCount);

		std::shared_ptr<GameObject> childObject = parentScene->createGameObject(childObjectType, -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN, false, name);


		//Child Size Override
		if (itrChild.isMember("size")) {

			b2Vec2 size = { itrChild["size"]["width"].asFloat(), itrChild["size"]["height"].asFloat() };
			childObject->setSize(size);

		}

		//Child Color Override
		if (itrChild.isMember("color")) {

			SDL_Color color = game->colorMap()->toSDLColor(itrChild["color"].asString());
			childObject->setColor(color);

		}

		//Standard Slot
		if (itrChild.isMember("standardSlot")) {

			PositionAlignment positionAlignment = (PositionAlignment)game->enumMap()->toEnum( itrChild["standardSlot"].asString());
			_addChild(childObject, positionAlignment);
			
		}

		//Absolute Position Slot
		if (itrChild.isMember("absolutePositionSlot")) {

			SDL_FPoint position = { itrChild["absolutePositionSlot"]["x"].asFloat(), itrChild["absolutePositionSlot"]["y"].asFloat()};
			_addChild(childObject, position);
		}

	}
}


ChildrenComponent::~ChildrenComponent()
{

}

void ChildrenComponent::postInit()
{

	//Set the layer for these pieces using the parents layer
	for (auto& slotItr : m_childSlots) {

		//Each child slot can have multiple gameObjects that live in a vector
		//Only Standard slots support multipl
		for (auto& child : slotItr.second) {

			if (child.gameObject.has_value()) {

				child.gameObject.value()->setLayer(parent()->layer());
				child.gameObject.value()->postInit();
				child.gameObject.value()->setParent(parent());

			}

		}
	}

	//Calculate GridSLot locationOffsets



}

void ChildrenComponent::update()
{

	short locationSlot = 0;
	b2Vec2 newChildPosition{};
	
	
	//This is how many children we have in this slot, stored by slot type
	std::map<ChildSlotType, std::vector<SlotMeasure>> slotMeasurements = _calculateSlotMeasurements();

	for (auto& childSlot : m_childSlots)
	{

		//Calculate slot positions for standard slots and absolute slots
		//Grid slot positions were already calculated and will not change and new ones cannot be added during runtime 
		// like standard and absolute
		//Standard slots could have a child added during runtime to an existing slot which could change the positions inside of its slot
		//New child objects could be added as absolute slots during runtime
		ChildSlotType slotType = _getSlotType(childSlot.first);

		if (slotType == ChildSlotType::STANDARD_SLOT) {

			PositionAlignment positionAlignment = _translateStandardSlotPositionAlignment(childSlot.first);
			_calculateStandardSlotPositions(childSlot.second, positionAlignment, slotMeasurements[ChildSlotType::STANDARD_SLOT]);

		}

		if (slotType == ChildSlotType::ABSOLUTE_POSITION) {

			int slot = _getSlotIndex(childSlot.first) - 1;
			_calculateAbsoluteSlotPositions(childSlot.second, slot, slotMeasurements[ChildSlotType::ABSOLUTE_POSITION]);

		}

	
		//Loop through all the children in this slot and calculate their positions based on the offsets we've already calculated
		//and the parents position
		for (const auto& childObject : childSlot.second) {

			if (childObject.gameObject.has_value()) {
				b2Vec2 newPosition =
				{ parent()->getCenterPosition().x + childObject.calculatedOffset.x,
				parent()->getCenterPosition().y + childObject.calculatedOffset.y };


				if (m_matchParentRotation == true) {

					childObject.gameObject->get()->setPosition(newPosition, parent()->getAngle());
				}
				else {

					childObject.gameObject->get()->setPosition(newPosition, -1);
				}


				childObject.gameObject->get()->update();

			}

		}

	}

	_removeFromWorldPass();
	
}

ChildSlotType ChildrenComponent::_getSlotType(std::string slotKey)
{

	if (slotKey[0] == 'A') {
		return ChildSlotType::ABSOLUTE_POSITION;
	}

	if (slotKey[0] == 'S') {
		return ChildSlotType::STANDARD_SLOT;
	}

	//default
	return ChildSlotType::STANDARD_SLOT;
}

int ChildrenComponent::_getSlotIndex(std::string slotKey)
{
	int num = std::stoi(slotKey.substr(1, 1));

	return num;
}


void ChildrenComponent::_removeFromWorldPass()
{

	for (auto& slotItr : m_childSlots) {

		//Each child slot can have multiple gameObjects that live in a vector
		auto childItr = slotItr.second.begin();
		while (childItr!= slotItr.second.end() ) {

			//Some slots may be empty so check
			if (childItr->gameObject.has_value() && childItr->gameObject.value()->removeFromWorld() == true) {

				//Remove object from gloabl index collection
				parent()->parentScene()->deleteIndex(childItr->gameObject.value()->id());

				//Erase the object from the map
				slotItr.second.erase(childItr);

			}
			else {

				++childItr;
			}

		}

		slotItr.second.shrink_to_fit();

	}

}



//A stepChild is an object that is added as a child later and does not get the same naming
//and removal treatment. If a stepChild has other children, they are not removed
void ChildrenComponent::addStepChild(std::shared_ptr<GameObject> gameObject, PositionAlignment positionAlignment)
{

	std::string slotKey = _determineSlotKey(positionAlignment);

	Child child;
	child.gameObject = gameObject;

	std::vector<Child> children = { child };

	//Insert will not insert if the key is already there
	m_childSlots.insert(std::pair<std::string, std::vector<Child>>(slotKey, children));

}

//A stepChild lives outside of the parent so we just remove him as a child and do not delete its 
//index from the main lookup tanle
void ChildrenComponent::removeChild(std::string id)
{

	for (auto& slotItr : m_childSlots) {

		//Each child slot can have multiple gameObjects that live in a vector
		auto childItr = slotItr.second.begin();
		while (childItr != slotItr.second.end()) {

			if (childItr->gameObject.has_value() && childItr->gameObject.value()->id() == id) {

				if (childItr->isStepChild == false) {

					childItr->gameObject.value()->setRemoveFromWorld(true);
				}

				//Erase the object from the map unless its the last item. there should always be one item
				//that has the position. This is only a problem with standard slots that can hold more than 1
				if (slotItr.second.size() > 1) {

					slotItr.second.erase(childItr);
				}
				else {

					slotItr.second[0].gameObject = std::nullopt;

				}

				_removeAllChildren(childItr->gameObject.value().get());

				break;

			}
			else {

				++childItr;
			}

		}

		slotItr.second.shrink_to_fit();

	}
}

void ChildrenComponent::_removeAllChildren(GameObject* gameObject)
{
	//Does this gameObject have a children component
	if (gameObject->hasComponent(ComponentTypes::CHILDREN_COMPONENT)) {

		const auto& childsChildrenComponent = gameObject->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);

		for (auto& slotItr : childsChildrenComponent->childSlots()) {

			auto childItr = slotItr.second.begin();
			while (childItr != slotItr.second.end()) {

				if (childItr->gameObject.has_value()) {

					if (childItr->isStepChild == false) {
						childItr->gameObject.value()->setRemoveFromWorld(true);
					}

					//Erase the object from the map unless its the last item. there should always be one item
					//that has the position. This is only a problem with standard slots that can hold more than 1
					if (slotItr.second.size() > 1) {

						slotItr.second.erase(childItr);
					}
					else {

						slotItr.second[0].gameObject = std::nullopt;

					}

					_removeAllChildren(childItr->gameObject.value().get());

					++childItr;
				}

			}
		}

	}

}

//
//This is for adding childObjects to the standard slots
//
std::string ChildrenComponent::_addChild(std::shared_ptr<GameObject> childObject, PositionAlignment positionAlignment)
{

	std::string slotKey = _determineSlotKey(positionAlignment);

	int childSlotCount{0};


	Child child;
	child.gameObject = childObject;

	if (m_childSlots.contains(slotKey)) {

		m_childSlots.at(slotKey).push_back(child);
	}
	else {

		std::vector<Child> children = { child };
		m_childSlots.emplace(std::pair<std::string, std::vector<Child>>(slotKey, children));

	}

	return std::string();
}

//
//This is for adding childObjects to Absolute Position Slots
//
std::string ChildrenComponent::_addChild(std::shared_ptr<GameObject> childObject, SDL_FPoint position)
{

	std::string slotKey = _determineSlotKey(position);

	Child child;
	child.gameObject = childObject;
	child.absolutePositionOffset = position;

	if (m_childSlots.contains(slotKey)) {

		m_childSlots.at(slotKey).push_back(child);
	}
	else {

		std::vector<Child> children = { child };
		m_childSlots.emplace(std::pair<std::string, std::vector<Child>>(slotKey, children));

	}

	return std::string();
}

std::string ChildrenComponent::_determineSlotKey(SDL_FPoint absolutePosition)
{
	int count{};

	for (const auto& pair : m_childSlots) {

		//If the position x and y are matchs a position of a slot object already there then this goes into the same slot
		const std::string& key = pair.first;
		if (!key.empty() && key[0] == 'A') {
			if (pair.second[0].absolutePositionOffset.x == absolutePosition.x && pair.second[0].absolutePositionOffset.y == absolutePosition.y) {
				return key;
			}
			count++;
		}
	}

	std::string slotKey = std::format("A{:01}", count+1);
	return slotKey;

}

std::string ChildrenComponent::_determineSlotKey(PositionAlignment positionAlignment)
{
	std::string slotKey{};

	switch (positionAlignment) {

		case PositionAlignment::TOP_LEFT:
			slotKey = "S1";
			break;
		case PositionAlignment::TOP_CENTER:
			slotKey = "S2";
			break;
		case PositionAlignment::TOP_RIGHT:
			slotKey = "S3";
			break;
		case PositionAlignment::CENTER_LEFT:
			slotKey = "S4";
			break;
		case PositionAlignment::CENTER:
			slotKey = "S5";
			break;
		case PositionAlignment::CENTER_RIGHT:
			slotKey = "S6";
			break;
		case PositionAlignment::BOTTOM_LEFT:
			slotKey = "S7";
			break;
		case PositionAlignment::BOTTOM_CENTER:
			slotKey = "S8";
			break;
		case PositionAlignment::BOTTOM_RIGHT:
			slotKey = "S9";
			break;

	}

	return slotKey;


}

void ChildrenComponent::render()
{

	for (auto& slotItr : m_childSlots) {

		for (auto& child : slotItr.second) {

			if (child.gameObject.has_value()) {
				child.gameObject.value()->render();
			}

		}
	}


}


std::string ChildrenComponent::_buildChildName(std::string parentName, int childCount)
{

	auto name = std::format("{}_CH{:03}", parentName, childCount);

	return name;

}

void ChildrenComponent::_buildGridSlots()
{

	GameObjectDefinition gameObjectDefinition = parent()->gameObjectDefinition();
	Json::Value componentDefinition = util::getComponentConfig(gameObjectDefinition.definitionJSON(), ComponentTypes::CHILDREN_COMPONENT);

	//Child Size Override
	if (componentDefinition.isMember("gridDefinition")) {

		int rows = componentDefinition["gridDefinition"]["rows"].asInt();
		int column = componentDefinition["gridDefinition"]["columns"].asInt();
		float gridPadding = componentDefinition["gridDefinition"]["childPadding"].asFloat();
		float marginPadding = componentDefinition["gridDefinition"]["marginPadding"].asFloat();



	}






	Child child;
	std::vector<Child> children = { child };

	m_childSlots.emplace(std::pair<std::string, std::vector<Child>>("S1", children));



}

PositionAlignment ChildrenComponent::_translateStandardSlotPositionAlignment(std::string slotKey)
{

	if (slotKey == "S1") {
		return PositionAlignment::TOP_LEFT;
	}
	if (slotKey == "S2") {
		return PositionAlignment::TOP_CENTER;
	}
	if (slotKey == "S3") {
		return PositionAlignment::TOP_RIGHT;
	}
	if (slotKey == "S4") {
		return PositionAlignment::CENTER_LEFT;
	}
	if (slotKey == "S5") {
		return PositionAlignment::CENTER;
	}
	if (slotKey == "S6") {
		return PositionAlignment::CENTER_RIGHT;
	}
	if (slotKey == "S7") {
		return PositionAlignment::BOTTOM_LEFT;
	}
	if (slotKey == "S8") {
		return PositionAlignment::BOTTOM_CENTER;
	}
	if (slotKey == "S9") {
		return PositionAlignment::BOTTOM_RIGHT;
	}

	SDL_assert(true && "No match for slotType!");

}

std::map<ChildSlotType, std::vector<SlotMeasure>> ChildrenComponent::_calculateSlotMeasurements()
{
	std::map<ChildSlotType, std::vector<SlotMeasure>> slotMeasurements;
	slotMeasurements[ChildSlotType::ABSOLUTE_POSITION] = std::vector<SlotMeasure>(9);
	slotMeasurements[ChildSlotType::STANDARD_SLOT] = std::vector<SlotMeasure>(9);


	// Loop through all of the childobjects in each slot so that we can calulate the combined WIDTH and HEIGHT MEASUREMENTS 
	// of all objects together including the child object padding, in order to later calculate an accurate 
	// center point for the slot and the minimum and maximum width and height of the slot.
	for (auto& slotItr : m_childSlots) {

		auto childSlotType = _getSlotType(slotItr.first);

		//slot index for S2 is 2, A5 is 5, etc.
		int slotIndex = _getSlotIndex(slotItr.first) - 1;

		//Start with adding in the chail object padding value
		float totalPaddingValue = fmin(0, (slotItr.second.size() - 1) * m_childPadding);
		slotMeasurements[childSlotType][slotIndex].sizeTotal.x += totalPaddingValue;
		slotMeasurements[childSlotType][slotIndex].sizeTotal.y += totalPaddingValue;

		//Add up every objects width and height and store the maximum width and height for each slot
		for (auto& child : slotItr.second) {

			if (child.gameObject.has_value()) {

				slotMeasurements[childSlotType][slotIndex].sizeTotal.x += child.gameObject.value()->getSize().x;
				slotMeasurements[childSlotType][slotIndex].sizeTotal.y += child.gameObject.value()->getSize().y;

				slotMeasurements[childSlotType][slotIndex].maxDimension.x =
					fmax(slotMeasurements[childSlotType][slotIndex].maxDimension.x, child.gameObject.value()->getSize().x);
				slotMeasurements[childSlotType][slotIndex].maxDimension.y =
					fmax(slotMeasurements[childSlotType][slotIndex].maxDimension.y, child.gameObject.value()->getSize().y);

			}

		}
	}

	return slotMeasurements;

}

void ChildrenComponent::_calculateAbsoluteSlotPositions(std::vector<Child>& childObjects, int slot, std::vector<SlotMeasure>& slotMeasurements)
{

	int slotGameObjectNumber{};

	//Loop through the vector that lives in this slot position
	for (auto& child : childObjects) {

		slotGameObjectNumber++;

		if (child.gameObject.has_value()) {

			//If there is only ONE gameObject in this slot, then the center position calculated above is our value
			if (childObjects.size() == 1 || m_childSameSlotTreatment == ChildSlotTreatment::STACKED) {

				child.calculatedOffset = child.absolutePositionOffset;
			}
			else {

				//Calcualte the center point for this slot which takes into account is there are multiple gameObjects in the slot
				SDL_FPoint slotCenterPositionOffset = _calculateAbsoluteSlotCenterPosition(child.absolutePositionOffset, slot, slotMeasurements);

				//Calculate position offsets for each gameObject in the slot using our calculated centerpoint as a starting point
				child.calculatedOffset = _calculateSlotMultiChild(slotCenterPositionOffset, slotGameObjectNumber, childObjects.size(),
					child.gameObject.value().get());

			}
		}
	}

}


void ChildrenComponent::_calculateStandardSlotPositions(std::vector<Child>& childObjects, PositionAlignment positionAlignment, 
	std::vector<SlotMeasure>& slotMeasurements)
{

	int slotGameObjectNumber{};

	//Loop through the vector that lives in this slot position
	for (auto& child : childObjects) {

		slotGameObjectNumber++;

		if (child.gameObject.has_value()) {

			//Calcualte the center point for this slot which takes into account is there are multiple gameObjects in the slot
			SDL_FPoint slotCenterPositionOffset = _calculateStandardSlotCenterPosition(positionAlignment, slotMeasurements);

			//If there is only ONE gameObject in this slot, then the center position calculated above is our value
			if (childObjects.size() == 1 || m_childSameSlotTreatment == ChildSlotTreatment::STACKED) {

				child.calculatedOffset = slotCenterPositionOffset;
			}
			else {

				//Calculate position offsets for each gameObject in the slot using our calculated centerpoint as a starting point
				child.calculatedOffset = _calculateSlotMultiChild(slotCenterPositionOffset, slotGameObjectNumber, childObjects.size(),
					child.gameObject.value().get());

			}
		}
	}

}

SDL_FPoint ChildrenComponent::_calculateSlotMultiChild(SDL_FPoint slotCenterPositionOffset, int slotGameObjectNumber, int totalSlotObjectCount, 
	GameObject* childGameObject)
{

	float oddEvenadjustValue = 0;
	int stepCount = 0;
	b2Vec2 firstChildPosition = {};
	SDL_FPoint newPositionOffset{};
	

	//calculate vertical step adjustment depending on even or odd
	if (totalSlotObjectCount % 2 == 0)
	{
		//isEvenNumber
		if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {
			oddEvenadjustValue = (childGameObject->getSize().y + m_childPadding) / 2;
		}
		else {
			oddEvenadjustValue = (childGameObject->getSize().x + m_childPadding) / 2;
		}
	}
	else
	{
		if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {
			oddEvenadjustValue = childGameObject->getSize().y + m_childPadding;
		}
		else {
			oddEvenadjustValue = childGameObject->getSize().x + m_childPadding;
		}
	}

	//calculate number of steps to take to place 1st child object
	stepCount = totalSlotObjectCount / 2;

	if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {
		//Calculate 1st child object position based on the previous childPosition calculated
		//values based on location slot
		firstChildPosition.x = slotCenterPositionOffset.x;
		firstChildPosition.y =
			slotCenterPositionOffset.y -
			oddEvenadjustValue -
			((childGameObject->getSize().y + m_childPadding) * stepCount);

		//Calculate our current child object position using the stepSize and the
		//position of the first child position
		newPositionOffset.x = firstChildPosition.x;
		newPositionOffset.y =
			firstChildPosition.y + ((childGameObject->getSize().y + m_childPadding) * slotGameObjectNumber);
	}
	else {
		//Calculate 1st child object position based on the previous childPosition calculated
		//values based on location slot
		firstChildPosition.x =
			slotCenterPositionOffset.x -
			oddEvenadjustValue -
			((childGameObject->getSize().x + m_childPadding) * stepCount);
		firstChildPosition.y = slotCenterPositionOffset.y;

		//Calculate our current child object position using the stepSize and the
		//position of the first child position
		newPositionOffset.x = firstChildPosition.x + ((childGameObject->getSize().x + m_childPadding) * slotGameObjectNumber);
		newPositionOffset.y = firstChildPosition.y;

	}

	////////////TEST///////////////////////////
	if (totalSlotObjectCount > 1 && m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

		newPositionOffset.y / totalSlotObjectCount;
	}
	else {

		newPositionOffset.x / totalSlotObjectCount;
	}

	return newPositionOffset;

}

SDL_FPoint ChildrenComponent::_calculateStandardSlotCenterPosition(PositionAlignment positionAlignment, std::vector<SlotMeasure>& slotSizes)
{
	SDL_FPoint slotCenterPositionOffset{};

	int slot = (int)positionAlignment;

	switch (positionAlignment) {

		case PositionAlignment::TOP_LEFT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].maxDimension.x );
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].sizeTotal.y );
			}
			else {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].sizeTotal.x );
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::TOP_CENTER:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].sizeTotal.y);
			}
			else {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::TOP_RIGHT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].sizeTotal.y);
			}
			else {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].sizeTotal.x);
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::CENTER_LEFT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0;
			}
			else {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].sizeTotal.x);
				slotCenterPositionOffset.y = 0;
			}

			break;

		case PositionAlignment::CENTER:

			slotCenterPositionOffset.x = 0;
			slotCenterPositionOffset.y = 0;
			break;

		case PositionAlignment::CENTER_RIGHT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0;
			}
			else {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].sizeTotal.x);
				slotCenterPositionOffset.y = 0;
			}


			break;
		case PositionAlignment::BOTTOM_LEFT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].sizeTotal.y);
			}
			else {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].sizeTotal.x);
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::BOTTOM_CENTER:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].sizeTotal.y);
			}
			else {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::BOTTOM_RIGHT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].sizeTotal.y);
			}
			else {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].sizeTotal.x);
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].maxDimension.y);
			}

			break;

	}

	return slotCenterPositionOffset;

}

SDL_FPoint ChildrenComponent::_calculateAbsoluteSlotCenterPosition(SDL_FPoint position, int slot, std::vector<SlotMeasure>& slotSizes)
{
	SDL_FPoint slotCenterPositionOffset{};

	if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

		slotCenterPositionOffset.x = position.x - (slotSizes[slot].maxDimension.x / 2);
		slotCenterPositionOffset.y = position.y - (slotSizes[slot].sizeTotal.y / 2);
	}
	else {

		slotCenterPositionOffset.x = position.x - (slotSizes[slot].sizeTotal.x / 2);
		slotCenterPositionOffset.y = position.y - (slotSizes[slot].maxDimension.y / 2);
	}

	return slotCenterPositionOffset;

}