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
	m_displayObjectType = componentJSON["displayObject"].asString();
	m_activeItem = 0;

}

InventoryComponent::~InventoryComponent()
{

}

size_t InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject)
{

	m_items.emplace_back(gameObject);

	return(m_items.size());

}

std::optional<GameObject*> InventoryComponent::getItem(const int traitTag)
{
	std::optional<GameObject*> foundItem{};

	for (auto item : m_items) {

		//This assumes only one item with this trait - returns first one
		if(item.expired() == false && item.lock()->hasTrait(traitTag)) {
			foundItem = item.lock().get();
			break;
		}
	}
	return foundItem;
}



int InventoryComponent::addCollectible(const CollectibleTypes collectableType, int count)
{

	m_collectibles.at(collectableType) += count;

	return m_collectibles.at(collectableType);
}

void InventoryComponent::render()
{

	if (m_displayInventoryObject.has_value()) {

		m_displayInventoryObject.value()->render();

	}

	for (const auto& inventoryObject : m_items) {

		if (inventoryObject.expired() == false) {

			inventoryObject.lock()->render();
		}

	}

}

void InventoryComponent::update()
{
	//Have each inventory game object update itself
	/*for (auto& item : m_items)
	{
		item.gameObject->update();
	}*/

}

void InventoryComponent::showInventory(std::string mainHudHolderType)
{

	//Create a local inventory display object
	auto displayInventoryObject = std::make_shared<GameObject>(m_displayObjectType, -50.0F, -50.0F, 0.F,
		parent()->parentScene(), GameLayer::GUI);

	m_displayInventoryObjectId = displayInventoryObject.get()->id();

	//Add index 
	parent()->parentScene()->addGameObjectIndex(displayInventoryObject);

	//Add the inventory items to its children component's center slot
	const auto& displayObjectChildrenComponent = displayInventoryObject->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
	for (auto& inventoryObject : m_items) {

		if (inventoryObject.expired() == false) {
			displayObjectChildrenComponent->addChild(inventoryObject.lock(), 5);
		}
	}

	//Add the m_displayInventoryObject to the hud object that is named in the paramter as a child of that hud object
	const auto& mainHudHolder = parent()->parentScene()->getFirstGameObjectByType(mainHudHolderType);
	if (mainHudHolder.has_value()) {
		const auto& mainHudChildrenComponent = mainHudHolder.value()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
		mainHudChildrenComponent->addChild(displayInventoryObject, 5);
	}

	

}

void InventoryComponent::showInventory(b2Vec2 offset)
{

	//Create the inventory display object
	m_displayInventoryObject = std::make_shared<GameObject>(m_displayObjectType, -50.0F, -50.0F, 0.F,
		parent()->parentScene(), GameLayer::GUI);

	//Add index 
	parent()->parentScene()->addGameObjectIndex(m_displayInventoryObject.value());

	//Add the inventory items to its children component's center slot
	const auto& displayObjectChildrenComponent = m_displayInventoryObject.value()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
	for (auto& inventoryObject : m_items) {

		if (inventoryObject.expired() == false) {
			displayObjectChildrenComponent->addChild(inventoryObject.lock(), 5);
		}
	}

}

void InventoryComponent::hideInventory()
{

	for (auto& inventoryObject : m_items) {

		if (inventoryObject.expired() == false) {
			inventoryObject.lock()->setOffGrid();
		}
	}

	m_displayInventoryObject.reset();
	clearDisplayInventoryObjectId();

}

