#include "InventoryComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

//InventoryComponent::InventoryComponent() :
//	Component(ComponentTypes::INVENTORY_COMPONENT)
//{
//	m_isDependentObjectOwner = true;
//}

InventoryComponent::InventoryComponent(Json::Value componentJSON, GameObject* parent, std::string parentName, Scene* parentScene) :
	Component(ComponentTypes::INVENTORY_COMPONENT, parent)
{

	m_componentType = ComponentTypes::INVENTORY_COMPONENT;
	
	m_maxCapacity = componentJSON["maxCapacity"].asFloat();
	m_activeItem = 0;

	m_isAlwaysOpen = componentJSON["isAlwaysOpen"].asBool();

	//Create the display object
	if (componentJSON.isMember("displayObject")) {

		std::shared_ptr<GameObject> displayObject{};

		Json::Value displayObjectJSON = componentJSON["displayObject"];

		std::string displayObjectType = displayObjectJSON["gameObjectType"].asString();
		GameLayer displayLayer{};

		if (displayObjectJSON.isMember("displayLayer")) {
			displayLayer = (GameLayer)game->enumMap()->toEnum(displayObjectJSON["displayLayer"].asString());
		}
		else {
			displayLayer = GameLayer::MAIN;
		}

		if (displayObjectJSON.isMember("size")) {

			b2Vec2 sizeOverride = { displayObjectJSON["size"]["width"].asFloat(), displayObjectJSON["size"]["height"].asFloat() };
			displayObject = parentScene->createGameObject(displayObjectType, parent, -50.0F, -50.0F, 0.F, parentScene, displayLayer, false, "", sizeOverride);

		}
		else {

			displayObject = parentScene->createGameObject(displayObjectType, parent, -50.0F, -50.0F, 0.F, parentScene, displayLayer);

		}
		
		parentScene->addGameObject(displayObject, displayLayer);
		displayObject->disablePhysics();
		displayObject->disableRender();
		displayObject->disableCollision();
		m_displayObject = displayObject;

	}

	m_items = std::vector<std::optional<std::shared_ptr<GameObject>>>(m_maxCapacity);
}

InventoryComponent::~InventoryComponent()
{

}

void InventoryComponent::postInit()
{

	if (m_isAlwaysOpen == true) {

		refreshInventoryDisplay();
	}

}


//void InventoryComponent::setParent(GameObject* gameObject)
//{
//	//Call base component setParent
//	Component::setParent(gameObject);
//
//	//Parent for this interactionMenuObject if it exists
//	if (m_displayObject) {
//		m_displayObject.value().lock()->setParent(gameObject);
//
//	}
//
//}

bool InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject)
{

	//Find first available inventory slot
	std::optional<int> slotIndexFound{};
	for (int i = 0; i < m_items.size();i++) {

		if (m_items[i].has_value() == false) {
			slotIndexFound = i;
			break;
		}
	}

	if (slotIndexFound) {
		addItem(gameObject, slotIndexFound.value());
	}

	return slotIndexFound.has_value();

}


bool InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject, int slot)
{
	bool itemAdded = false;
	if (slot <= (m_items.size() - 1)) {

		m_items[slot] = gameObject;
		itemAdded = true;

		//Set this items new parent to this inventory holding object
		gameObject->setParent(parent());

		//All items in inventory are draggable and obtainable if they are not already in the players inventory
		gameObject->addState(GameObjectState::DRAGGABLE);

		//if this is a receptacle that has an inventory and gridDisplay in one, like a shelf, then the object is considered loose
		if (parent()->hasComponent(ComponentTypes::GRID_DISPLAY_COMPONENT) == false) {

			gameObject->removeState(GameObjectState::ITEM_LOOSE);
			gameObject->removeState(GameObjectState::ITEM_STORED_OPEN);

		}
		else {
			gameObject->addState(GameObjectState::ITEM_LOOSE);
			gameObject->addState(GameObjectState::ITEM_STORED_OPEN);

		}

		if (parent()->hasTrait(TraitTag::player) == false) {

			gameObject->addState(GameObjectState::ITEM_OBTAINABLE);
		}
		else {
			gameObject->removeState(GameObjectState::ITEM_STORED_OPEN);
			gameObject->removeState(GameObjectState::ITEM_OBTAINABLE);
		}


	}

	return itemAdded;

}

bool InventoryComponent::addItem(std::string gameObjectType)
{

	auto gameObject = parent()->parentScene()->createGameObject(gameObjectType, parent(),  - 50.0F, -50.0F, 0.F,
		parent()->parentScene(), GameLayer::GUI_2);

	if (addItem(gameObject) == false) {

		return false;
	}
	else {
		return true;
	}

}

bool InventoryComponent::addItem(std::string gameObjectType, int slot)
{

	auto gameObject = parent()->parentScene()->createGameObject(gameObjectType, parent(),  - 50.0F, -50.0F, 0.F,
		parent()->parentScene(), GameLayer::GUI_2);

	return addItem(gameObject, slot);

}

std::shared_ptr<GameObject> InventoryComponent::removeItem(int slot)
{
	std::shared_ptr<GameObject> removedObject{};
	removedObject = m_items[slot].value();
	removedObject->setParent(nullptr);
	m_items[slot] = std::nullopt;

	return removedObject;
}

bool InventoryComponent::hasItem(int slot)
{
	return m_items[slot].has_value();
}

std::shared_ptr<GameObject> InventoryComponent::removeItem(GameObject* gameObject)
{

	std::shared_ptr<GameObject> removedObject{};

	for (auto& slot : m_items) {

		if (slot.has_value() && slot.value().get() == gameObject) {
			removedObject = slot.value();
			removedObject->setParent(nullptr);
			slot = std::nullopt;
			break;
		}
	}

	return removedObject;

}

std::optional<std::shared_ptr<GameObject>> InventoryComponent::getItem(const int traitTag)
{

	for (auto item : m_items) {

		//This assumes only one item with this trait - returns first one
		if(item.has_value() && item.value()->hasTrait(traitTag)) {
			return item;
			
		}
	}
	return std::nullopt;
}

int InventoryComponent::addCollectible(const CollectibleTypes collectableType, int count)
{

	m_collectibles.at(collectableType) += count;

	return m_collectibles.at(collectableType);
}

void InventoryComponent::update()
{

	if (m_isAlwaysOpen == true) {

		m_isOpen = true;
	}

	if (m_isOpen) {

		for (const auto& inventoryObject : m_items) {


			if (inventoryObject.has_value() == true) {

				inventoryObject.value()->update();

				//Its possiblwe that this object was removed from this inventory during the update above
				if (inventoryObject.has_value()) {

					inventoryObject.value()->setLayer(parent()->layer());
				}

			}

		}

	}

	_removeFromWorldPass();

}

void InventoryComponent::refreshInventoryDisplay() {

	//If we have a separate gameObject that isued to display the inventory
	if (m_displayObject && m_displayObject.value().expired() == false) {

		//Get displayObjects grid display component
		const auto& gridDisplayComponent = m_displayObject.value().lock()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
		gridDisplayComponent->clear();

		showInventory();
	}

	//If this ivnetoryObject is also the inventory display object, like a shelf
	if (parent()->hasComponent(ComponentTypes::GRID_DISPLAY_COMPONENT)) {

		const auto& gridDisplayComponent = parent()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
		gridDisplayComponent->clear();

		showInventory();

	}

}

bool InventoryComponent::isFull()
{
	
	for (const auto& item: m_items) {
		if (item.has_value() == false) {
			return false;
		}

	}

	return true;
}

std::optional<int> InventoryComponent::getSlot(GameObject* gameObject)
{
	int slotIndex{};
	for (auto& slot : m_items) {

		if (slot.has_value() && slot.value().get() == gameObject) {
			return slotIndex;
		}

		slotIndex++;
	}

	return std::nullopt;

}


//Show the inventory as a child of a specific object that is not the inventory holder
void InventoryComponent::showInventory()
{
	std::shared_ptr<GridDisplayComponent> gridDisplayComponent{};

	//If we have a separate gameObject that is used to display the inventory
	if (m_displayObject && m_displayObject.value().expired() == false) {
		gridDisplayComponent = m_displayObject.value().lock()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
		m_displayObject.value().lock()->enableRender();
		m_displayObject.value().lock()->enablePhysics();
		m_displayObject.value().lock()->enableCollision();

		//This is an unfortunately needed workaround for when an inventory holder goes from disabled to enabled,
		//the box2d isTouching doesnt get triggered unless we very slightly move the player to force refresh
		const auto& player = parent()->parentScene()->getFirstGameObjectByTrait(TraitTag::player);
		const auto& playerPhysics = player.value()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		playerPhysics->applyImpulse(50, {0,0});
		playerPhysics->setLinearVelocity({0.,0.});

	}

	//If this ivnetoryObject is also the inventory display object, like a shelf
	if (parent()->hasComponent(ComponentTypes::GRID_DISPLAY_COMPONENT)) {

		gridDisplayComponent = parent()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
	}

	//Add all inventory items to the gridDisplayComponent
	for (int i = 0; i < m_items.size(); i++) {

		if (m_items[i].has_value()) {

			m_items[i].value()->enablePhysics();
			gridDisplayComponent->addItem(m_items[i].value(), i);
		}
	}

}

void InventoryComponent::hideInventory()
{

	if (m_displayObject && m_displayObject.value().expired() == false) {

		//Get displayObjects grid display component
		const auto& gridDisplayComponent = m_displayObject.value().lock()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
		m_displayObject.value().lock()->disableRender();
		m_displayObject.value().lock()->stash();


		gridDisplayComponent->clear();

		for (int i = 0; i < m_items.size(); i++) {

			if (m_items[i].has_value() ) {

				m_items[i].value()->stash();
				m_items[i].value()->clearDragging();

			}
		}

	}

}

void InventoryComponent::clearInventory()
{
	for (int i = 0; i < m_items.size(); i++) {

		if (m_items[i].has_value()) {

			m_items[i].value()->setRemoveFromWorld(true);
		}
	}

	_removeFromWorldPass();

}

void InventoryComponent::_removeFromWorldPass()
{

	auto inventoryItr = m_items.begin();
	while (inventoryItr != m_items.end()) {

		if (inventoryItr->has_value()) {

			if (inventoryItr->value()->removeFromWorld() == true) {

				//Remove object from gloabl index collection
				parent()->parentScene()->deleteIndex(inventoryItr->value()->id());

				//dont delte the item, just reset the optional and reset the shared_ptr
				inventoryItr->value().reset();
				inventoryItr->reset();

			}
			else {

				++inventoryItr;
			}

		}
		else {

			++inventoryItr;
		}

		
	}


}



