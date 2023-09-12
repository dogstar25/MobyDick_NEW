#include "InventoryComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

InventoryComponent::InventoryComponent()
{
	m_isDependentObjectOwner = true;
}

InventoryComponent::InventoryComponent(Json::Value componentJSON, std::string parentName, Scene* parentScene)
{

	m_componentType = ComponentTypes::INVENTORY_COMPONENT;
	
	m_maxCapacity = componentJSON["maxCapacity"].asFloat();
	m_activeItem = 0;

	//Create the display object
	m_displayObjectType = componentJSON["displayObject"].asString();
	if (componentJSON.isMember("displayLayer")) {
		m_displayLayer = (GameLayer)game->enumMap()->toEnum(componentJSON["displayLayer"].asString());
	}
	else {
		m_displayLayer = GameLayer::MAIN;
	}
	

	auto displayObject = parentScene->createGameObject(m_displayObjectType, -50.0F, -50.0F, 0.F, parentScene, m_displayLayer);
	parentScene->addGameObject(displayObject, m_displayLayer);
	displayObject->disablePhysics();
	displayObject->disableRender();
	displayObject->disableCollision();
	m_displayObject = displayObject;

	m_items = std::vector<std::optional<std::shared_ptr<GameObject>>>(m_maxCapacity);
}

InventoryComponent::~InventoryComponent()
{

}

void InventoryComponent::setParent(GameObject* gameObject)
{
	//Call base component setParent
	Component::setParent(gameObject);

	//Parent for this interactionMenuObject if it exists
	if (m_displayObject) {
		m_displayObject.value().lock()->setParent(gameObject);

	}

}

int InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject)
{

	//Find first available inventory slot
	int slotIndexFound{-1};
	for (int i = 0; i < m_items.size();i++) {

		if (m_items[i].has_value() == false) {
			m_items[i] = gameObject;
			slotIndexFound = i;
			break;
		}
	}

	addItem(gameObject, slotIndexFound);

	return slotIndexFound;

}


bool InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject, int slot)
{
	bool itemAdded = false;
	if (slot <= (m_items.size() - 1) && slot != -1) {
		m_items[slot] = gameObject;
		itemAdded = true;

		//Set this items new parent to this inventory holding object
		gameObject->setParent(parent());
	}

	return itemAdded;

}

bool InventoryComponent::addItem(std::string gameObjectType)
{

	auto gameObject = parent()->parentScene()->createGameObject(gameObjectType, -50.0F, -50.0F, 0.F,
		parent()->parentScene(), GameLayer::GUI_2);

	return addItem(gameObject);

}

bool InventoryComponent::addItem(std::string gameObjectType, int slot)
{

	auto gameObject = parent()->parentScene()->createGameObject(gameObjectType, -50.0F, -50.0F, 0.F,
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

//void InventoryComponent::render()
//{
//
//	std::shared_ptr<GridDisplayComponent> gridDisplayComponent;
//	if (m_displayBackdropObject) {
//		gridDisplayComponent = m_displayBackdropObject.value()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
//	}
//	else {
//		gridDisplayComponent = parent()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
//	}
//
//	gridDisplayComponent->render();
//
//}

void InventoryComponent::update()
{

	if (m_isOpen) {

		for (const auto& inventoryObject : m_items) {

			if (inventoryObject.has_value() == true) {

				inventoryObject.value()->update();
			}

		}

	}

}

void InventoryComponent::refreshInventoryDisplay() {

	if (m_displayObject && m_displayObject.value().expired() == false) {

		//Get displayObjects grid display component
		const auto& gridDisplayComponent = m_displayObject.value().lock()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
		gridDisplayComponent->clear();

		showInventory();
	}

}

//Show the inventory as a child of a specific object that is not the inventory holder
void InventoryComponent::showInventory()
{
	//m_isOpen = true;

	const auto& gridDisplayComponent = m_displayObject.value().lock()->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);
	m_displayObject.value().lock()->enablePhysics();

	for (int i = 0; i < m_items.size(); i++) {

		if (m_items[i].has_value()) {

			m_items[i].value()->enablePhysics();
			gridDisplayComponent->addItem(m_items[i].value(), i);
		}
	}

}

void InventoryComponent::hideInventory()
{

	m_displayObject.value().lock()->setOffGrid();

	for (int i = 0; i < m_items.size(); i++) {

		if (m_items[i].has_value()) {

			m_items[i].value()->setOffGrid();
		}
	}

	//m_isOpen = false;

}


