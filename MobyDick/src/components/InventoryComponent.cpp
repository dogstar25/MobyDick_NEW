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
	m_displayBackdropObjectType = componentJSON["display"]["backdropObject"].asString();
	m_maxCapacity = componentJSON["maxCapacity"].asFloat();
	m_activeItem = 0;

	//Create the Backdrop object
	m_displayBackdropObject = parentScene->createGameObject(m_displayBackdropObjectType, -50.0F, -50.0F, 0.F, parentScene, GameLayer::GUI_1);

	m_items = std::vector<std::optional<std::shared_ptr<GameObject>>>(m_maxCapacity);
}

InventoryComponent::~InventoryComponent()
{

}

int InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject)
{

	//Find first available inventory slot
	bool slotFound{};
	for (int i = 0; i < m_items.size();i++) {

		if (m_items[i].has_value() == false) {
			m_items[i] = gameObject;
			slotFound = i;
			break;
		}
	}

	return slotFound;

}


int InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject, int slot)
{

	if (slot <= m_items.size()) {
		m_items[slot] = gameObject;
	}

	return slot;

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

	if (m_isOpen) {

		for (const auto& inventoryObject : m_items) {

			if (inventoryObject.has_value() == true) {

				inventoryObject.value()->update();
			}

		}

	}

}

//Show the inventory as a child of a specific object that is not the inventory holder
void InventoryComponent::showInventory(GameObject* proxyObject)
{
	m_isOpen = true;

	const auto& gridDisplayComponent = proxyObject->getComponent<GridDisplayComponent>(ComponentTypes::GRID_DISPLAY_COMPONENT);

	for (int i = 0; i < m_items.size(); i++) {

		if (m_items[i].has_value()) {

			m_items[i].value()->enablePhysics();
			gridDisplayComponent->addItem(m_items[i].value(), i);
		}
	}
	

}


void InventoryComponent::hideInventory()
{

	for (int i = 0; i < m_items.size(); i++) {

		if (m_items[i].has_value()) {

			m_items[i].value()->setOffGrid();
		}
	}

	m_isOpen = false;

}


