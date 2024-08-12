#include "ChildrenComponent.h"
#include "../EnumMap.h"
#include "../GameObject.h"
#include "../game.h"
#include "../EnumMap.h"

extern std::unique_ptr<Game> game;

ChildrenComponent::ChildrenComponent(Json::Value componentJSON, GameObject* parent, std::string parentName, Scene* parentScene) :
	Component(ComponentTypes::CHILDREN_COMPONENT, parent)
{

	m_childPadding = componentJSON["childPadding"].asFloat();
	m_matchParentRotation = componentJSON["matchParentRotation"].asBool();
	if (componentJSON.isMember("sameSlotTreatment")) {
		m_childSameSlotTreatment = (ChildSlotTreatment)game->enumMap()->toEnum(componentJSON["sameSlotTreatment"].asString());
	}

	std::optional<int> locationSlot{};
	bool centeredOnLocation{true};

	m_isDependentObjectOwner = true;
	std::optional<b2Vec2> sizeOverride{};

	int childCount{};
	for (Json::Value itrChild : componentJSON["childObjects"])
	{
		childCount++;

		std::string childObjectType = itrChild["gameObjectType"].asString();
		std::string name{};
		if (itrChild.isMember("name")) {
			name = itrChild["name"].asString();
		}
		else {
			name = _buildChildName(parentName, childCount);
		}


		if (childObjectType == "NOPE_BUBBLE") {
			int todd = 1;

		}


		//Child Size Override
		b2Vec2 sizeOverride{ 0.f, 0.f };
		if (itrChild.isMember("size")) {

			sizeOverride = { itrChild["size"]["width"].asFloat(), itrChild["size"]["height"].asFloat() };
		}

		//StepChild?
		//StepChildren live in in main gameObjects collection for the scene and NOT under the parentObject
		bool isStepChild = itrChild["isStepChild"].asBool();

		//Child Layer Override
		//Layer can only be spercified for step children because regular children always display on same layer as parent
		GameLayer stepChildGameLayer{parent->layer()};
		if (itrChild.isMember("layer") && isStepChild) {

			stepChildGameLayer = static_cast<GameLayer>(game->enumMap()->toEnum(itrChild["layer"].asString()));

		}

		//Create the child
		std::shared_ptr<GameObject> childObject = 
			parentScene->createGameObject(childObjectType, parent, -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN, false, name, sizeOverride);
		childObject->isChild = true;

		//If this is a stepchild then we need to add it to the world where the scene will be responsible for it
		if (isStepChild == true) {

			parentScene->addGameObject(childObject, stepChildGameLayer);
		}

		//Begin States
		if (itrChild.isMember("beginStates")) {
			Json::Value beginStates = itrChild["beginStates"];
			for (auto& stateItr : beginStates) {

				auto state = (GameObjectState)game->enumMap()->toEnum(stateItr.asString());
				childObject->addState(state);
			}
		}

		//Child Color Override
		if (itrChild.isMember("color")) {

			SDL_Color color = game->colorMap()->toSDLColor(itrChild["color"].asString());
			childObject->setColor(color);

		}

		//Child description Override
		if (itrChild.isMember("description")) {
			std::string description = itrChild["description"].asString();
			childObject->setDescription(description);
		}


		//Standard Slot
		if (itrChild.isMember("standardSlot")) {

			PositionAlignment positionAlignment = (PositionAlignment)game->enumMap()->toEnum( itrChild["standardSlot"].asString());

			if (isStepChild) {
				addStepChild(childObject, positionAlignment);
			}
			else {
				_addChild(childObject, positionAlignment);
			}
			
		}

		//Absolute Position Slot
		if (itrChild.isMember("absolutePositionSlot")) {

			SDL_FPoint position = { itrChild["absolutePositionSlot"]["x"].asFloat(), itrChild["absolutePositionSlot"]["y"].asFloat()};

			if (isStepChild) {
				addStepChild(childObject, position);
			}
			else {
				_addChild(childObject, position);
			}
			
		}

	}
}


ChildrenComponent::~ChildrenComponent()
{

}

void ChildrenComponent::postInit()
{

	//Call postinit for a  children
	for (auto& slotItr : m_childSlots) {

		for (auto& child : slotItr.second) {

			if (child.gameObject.has_value()) {

				child.gameObject.value()->postInit();
			}

		}
	}

}

void ChildrenComponent::update()
{

	short locationSlot = 0;
	b2Vec2 newChildPosition{};
	
	for (auto& childSlot : m_childSlots)
	{

		//Calculate slot positions for standard slots and absolute slots
		//Standard slots could have a child added during runtime to an existing slot which could change the positions inside of its slot
		//New child objects could be added as absolute slots during runtime
		ChildSlotType slotType = _getSlotType(childSlot.first);

		std::vector<SlotMeasure> slotMeasurements = _calculateSlotMeasurements(childSlot.first);

		if (slotType == ChildSlotType::STANDARD_SLOT) {

			PositionAlignment positionAlignment = _translateStandardSlotPositionAlignment(childSlot.first);
			_calculateStandardSlotPositions(childSlot.second, positionAlignment, slotMeasurements);

		}

		if (slotType == ChildSlotType::ABSOLUTE_POSITION) {

			int slot = _getSlotIndex(childSlot.first) - 1;
			_calculateAbsoluteSlotPositions(childSlot.second, slot, slotMeasurements);

		}
	
		//Loop through all the children in this slot and calculate their positions based on the offsets we've already calculated
		//and the parents position
		int childCount = 0;

		auto childSlotItr = childSlot.second.begin();
		while (childSlotItr != childSlot.second.end()) {

			if (childSlotItr->gameObject.has_value()) {
				b2Vec2 newPosition =
				{ parent()->getCenterPosition().x + childSlotItr->calculatedOffset.x,
				parent()->getCenterPosition().y + childSlotItr->calculatedOffset.y };


				if (m_matchParentRotation == true) {

					childSlotItr->gameObject->get()->setPosition(newPosition, parent()->getAngle());
				}
				else {

					childSlotItr->gameObject->get()->setPosition(newPosition, -1);
				}

				if (childSlotItr->isStepChild == false) {
					childSlotItr->gameObject->get()->update();
				}

			}

			childCount++;
			++childSlotItr;
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

	for (auto slotItr = m_childSlots.begin(); slotItr != m_childSlots.end();) {

		//Each child slot can have multiple gameObjects that live in a vector
		auto childItr = slotItr->second.begin();
		while (childItr != slotItr->second.end() ) {

			//Some slots may be empty so check
			if (childItr->gameObject.has_value() && childItr->gameObject.value()->removeFromWorld() == true) {

				//Remove object from gloabl index collection
				parent()->parentScene()->deleteIndex(childItr->gameObject.value()->id());

				//Erase the object from the map
				childItr = slotItr->second.erase(childItr);
			}
			else {

				++childItr;
			}

		}

		// Check if the vector is empty and remove the slot if it is
		if (slotItr->second.empty()) {
			slotItr = m_childSlots.erase(slotItr);
		}
		else {
			++slotItr;
		}

	}

}



void ChildrenComponent::removeChildrenByType(std::string gameObjectType)
{

	auto childSlotItr = m_childSlots.begin();
	while (childSlotItr != m_childSlots.end()) {

		//Each child slot can have multiple gameObjects that live in a vector
		auto childItr = childSlotItr->second.begin();
		while (childItr != childSlotItr->second.end()) {

			if (childItr->gameObject.has_value() && childItr->gameObject.value()->type() == gameObjectType) {

				childItr->gameObject.value()->isChild = false;
				childItr->gameObject.value()->setRemoveFromWorld(true);
				++childItr;
			}
			
		}

		++childSlotItr;
	}

}

void ChildrenComponent::changeChildPosition(std::string childType, SDL_FPoint newPosition)
{

	for (auto& pair : m_childSlots) {

		const std::string& key = pair.first;

		for (auto& child : pair.second) {

			if (child.gameObject.has_value() && child.gameObject.value()->type() == childType) {

				child.absolutePositionOffset = newPosition;

			}

		}

	}


}

std::vector<std::shared_ptr<GameObject>> ChildrenComponent::getChildrenByType(std::string gameObjectType)
{

	std::vector<std::shared_ptr<GameObject>> foundGameObjects;

	auto childSlotItr = m_childSlots.begin();
	while (childSlotItr != m_childSlots.end()) {

		//Each child slot can have multiple gameObjects that live in a vector
		auto childItr = childSlotItr->second.begin();
		while (childItr != childSlotItr->second.end()) {

			if (childItr->gameObject.has_value() && childItr->gameObject.value()->type() == gameObjectType) {

				foundGameObjects.push_back(childItr->gameObject.value());
				
			}

			++childItr;
		}

		++childSlotItr;
	}

	return foundGameObjects;

}

std::optional<std::shared_ptr<GameObject>> ChildrenComponent::getFirstChildByType(std::string gameObjectType)
{

	std::shared_ptr<GameObject> foundGameObject;

	auto childSlotItr = m_childSlots.begin();
	while (childSlotItr != m_childSlots.end()) {

		//Each child slot can have multiple gameObjects that live in a vector
		auto childItr = childSlotItr->second.begin();
		while (childItr != childSlotItr->second.end()) {

			if (childItr->gameObject.has_value() && childItr->gameObject.value()->type() == gameObjectType) {

				return childItr->gameObject.value();

			}

			++childItr;
		}

		++childSlotItr;
	}

	return std::nullopt;

}

//A stepChild lives outside of the parent so we just remove him as a child and do not delete its 
//index from the main lookup tanle
void ChildrenComponent::removeChild(std::string id)
{

	auto childSlotItr = m_childSlots.begin();
	while (childSlotItr != m_childSlots.end()) {

		//Each child slot can have multiple gameObjects that live in a vector
		auto childItr = childSlotItr->second.begin();
		while (childItr != childSlotItr->second.end()) {

			if (childItr->gameObject.has_value() && childItr->gameObject.value()->id() == id) {

				childItr->gameObject.value()->isChild = false;

				if (childItr->isStepChild == false) {

					childItr->gameObject.value()->setRemoveFromWorld(true);
					_removeAllChildren(childItr->gameObject.value().get());
				}

				childItr = childSlotItr->second.erase(childItr);
			}
			else {

				++childItr;
			}
			
		}

		childSlotItr->second.shrink_to_fit();

		++childSlotItr;

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

					childItr->gameObject.value()->isChild = false;

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

//A stepChild is an object that is added as a child later and does not get the same naming
//and removal treatment. If a stepChild has other children, they are not removed
void ChildrenComponent::addStepChild(std::shared_ptr<GameObject> gameObject, PositionAlignment positionAlignment, bool addToEnd)
{

	std::string slotKey = _determineSlotKey(positionAlignment);

	Child child;
	child.isStepChild = true;
	child.gameObject = gameObject;

	if (m_childSlots.contains(slotKey)) {

		if (addToEnd) {
			m_childSlots.at(slotKey).push_back(child);
		}
		else {
			m_childSlots.at(slotKey).insert(m_childSlots.at(slotKey).begin(), child);
		}
	}
	else {

		std::vector<Child> children = { child };
		m_childSlots.emplace(std::pair<std::string, std::vector<Child>>(slotKey, children));

	}


}

//A stepChild is an object that is added as a child later and does not get the same naming
//and removal treatment. If a stepChild has other children, they are not removed
void ChildrenComponent::addStepChild(std::shared_ptr<GameObject> gameObject, SDL_FPoint position, bool addToEnd)
{

	std::string slotKey = _determineSlotKey(position);

	Child child;
	child.gameObject = gameObject;
	child.isStepChild = true;
	child.absolutePositionOffset = position;

	if (m_childSlots.contains(slotKey)) {

		if (addToEnd) {
			m_childSlots.at(slotKey).push_back(child);
		}
		else {
			m_childSlots.at(slotKey).insert(m_childSlots.at(slotKey).begin(), child);
		}
	}
	else {

		std::vector<Child> children = { child };
		m_childSlots.emplace(std::pair<std::string, std::vector<Child>>(slotKey, children));

	}

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

				//Stepchildren will render theirselves
				if (child.isStepChild == false) {

					child.gameObject.value()->render();

				}
			}
		}
	}


}


std::string ChildrenComponent::_buildChildName(std::string parentName, int childCount)
{

	auto name = std::format("{}_CH{:03}", parentName, childCount);

	return name;

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

	SDL_assert(false && "No match for slotType!");

}

std::vector<SlotMeasure> ChildrenComponent::_calculateSlotMeasurements(std::string slotKey)
{
	std::vector<SlotMeasure> slotMeasurements;
	slotMeasurements = std::vector<SlotMeasure>(9);

	// Loop through all of the childobjects in the giev slot so that we can calulate the combined WIDTH and HEIGHT MEASUREMENTS 
	// of all objects together including the child object padding, in order to later calculate an accurate 
	// center point for the slot and the minimum and maximum width and height of the slot.
	auto slotItr = m_childSlots.find(slotKey);
	while (slotItr != m_childSlots.end()) {

		ChildSlotType slotType = _getSlotType(slotKey);

		//slot index for S2 is 2, A5 is 5, etc.
		int slotIndex = _getSlotIndex(slotKey) - 1;

		//Start with adding in the child object padding value
		float totalPaddingValue = fmin(0, (slotItr->second.size() - 1) * m_childPadding);
		slotMeasurements[slotIndex].sizeTotal.x += totalPaddingValue;
		slotMeasurements[slotIndex].sizeTotal.y += totalPaddingValue;

		//Add up every objects width and height and store the maximum width and height for each slot
		for (auto& child : slotItr->second) {

			if (child.gameObject.has_value()) {

				slotMeasurements[slotIndex].sizeTotal.x += child.gameObject.value()->getSize().x;
				slotMeasurements[slotIndex].sizeTotal.y += child.gameObject.value()->getSize().y;

				slotMeasurements[slotIndex].maxDimension.x =
					fmax(slotMeasurements[slotIndex].maxDimension.x, child.gameObject.value()->getSize().x);
				slotMeasurements[slotIndex].maxDimension.y =
					fmax(slotMeasurements[slotIndex].maxDimension.y, child.gameObject.value()->getSize().y);

			}

		}

		++slotItr;
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
			SDL_FPoint slotCenterPositionOffset = _calculateStandardSlotCenterPosition(positionAlignment, slotMeasurements, childObjects.size());

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
	//if (totalSlotObjectCount > 1 && m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

	//	newPositionOffset.y / totalSlotObjectCount;
	//}
	//else {

	//	newPositionOffset.x / totalSlotObjectCount;
	//}

	return newPositionOffset;

}

SDL_FPoint ChildrenComponent::_calculateStandardSlotCenterPosition(PositionAlignment positionAlignment, std::vector<SlotMeasure>& slotSizes, int slotChildCount)
{
	SDL_FPoint slotCenterPositionOffset{};

	int slot = (int)positionAlignment;

	switch (positionAlignment) {

		case PositionAlignment::TOP_LEFT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].maxDimension.x );
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].sizeTotal.y / slotChildCount);
			}
			else {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].sizeTotal.x / slotChildCount);
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::TOP_CENTER:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].sizeTotal.y / slotChildCount);
			}
			else {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::TOP_RIGHT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].sizeTotal.y / slotChildCount);
			}
			else {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].sizeTotal.x / slotChildCount);
				slotCenterPositionOffset.y = 0 - (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::CENTER_LEFT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0;
			}
			else {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].sizeTotal.x / slotChildCount);
				slotCenterPositionOffset.y = 0;
			}

			break;

		case PositionAlignment::CENTER:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0;
			}
			else {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0;
			}

			break;

		case PositionAlignment::CENTER_RIGHT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0;
			}
			else {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].sizeTotal.x / slotChildCount);
				slotCenterPositionOffset.y = 0;
			}


			break;
		case PositionAlignment::BOTTOM_LEFT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].sizeTotal.y / slotChildCount);
			}
			else {

				slotCenterPositionOffset.x = 0 - (slotSizes[slot].sizeTotal.x);
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].maxDimension.y / slotChildCount);
			}

			break;

		case PositionAlignment::BOTTOM_CENTER:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].sizeTotal.y / slotChildCount);
			}
			else {

				slotCenterPositionOffset.x = 0;
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].maxDimension.y);
			}

			break;

		case PositionAlignment::BOTTOM_RIGHT:

			if (m_childSameSlotTreatment == ChildSlotTreatment::VERTICAL) {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].maxDimension.x);
				slotCenterPositionOffset.y = 0 + (slotSizes[slot].sizeTotal.y / slotChildCount);
			}
			else {

				slotCenterPositionOffset.x = 0 + (slotSizes[slot].sizeTotal.x / slotChildCount);
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