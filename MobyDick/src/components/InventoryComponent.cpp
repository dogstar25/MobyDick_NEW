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
	m_displayStartOffset.x = componentJSON["display"]["startOffset"]["x"].asFloat();
	m_displayStartOffset.y = componentJSON["display"]["startOffset"]["y"].asFloat();
	m_itemDisplaySize = componentJSON["display"]["itemSize"].asFloat();
	m_itemDisplayPadding = componentJSON["display"]["itemPadding"].asFloat();
	m_displayRows = componentJSON["display"]["rows"].asFloat();
	m_displayColumns = componentJSON["display"]["columns"].asFloat();
	m_maxCapacity = componentJSON["maxCapacity"].asFloat();
	m_activeItem = 0;

	//Create the Backdrop object
	m_displayBackdropObject = parentScene->createGameObject(m_displayBackdropObjectType, -50.0F, -50.0F, 0.F, parentScene, GameLayer::GUI_1);

	//Calculate slot position offsets
	_calculateSlotOffsets();

	//Reserve max inventory slots
	assert(m_displayRows * m_displayColumns == m_maxCapacity && "MaxCapacity inventory setting does not match display rows and columns!");

}

InventoryComponent::~InventoryComponent()
{

}


void InventoryComponent::_calculateSlotOffsets()
{

	int totalSlots = m_displayRows * m_displayColumns;

	//SDL_FPoint displayObjectPosition{};
	//if (m_displayBackdropObject.has_value()) {

	//	displayObjectPosition = { displayObjectPosition };

	//}



	for (auto col=0; col < m_displayColumns; col++) {
		for (auto row = 0; row < m_displayRows; row++) {

			InventoryItem item;
			item.displayOffset.x = m_displayStartOffset.x + (col * m_itemDisplayPadding) + (col * m_itemDisplaySize);
			item.displayOffset.y = m_displayStartOffset.y + (row * m_itemDisplayPadding) + (row * m_itemDisplaySize);
			
			m_items.insert(m_items.end(), item);
		}

		

	}

}


bool InventoryComponent::addItem(std::shared_ptr<GameObject> gameObject)
{

	//Find first available inventory slot
	bool slotFound{};
	for (auto& item : m_items) {

		if (item.gameObject.has_value() == false) {
			item.gameObject = gameObject;
			slotFound = true;
			break;
		}
	}

	return slotFound;

}

bool InventoryComponent::addItem(std::string gameObjectType)
{

	auto gameObject = parent()->parentScene()->createGameObject(gameObjectType, -50.0F, -50.0F, 0.F,
		parent()->parentScene(), GameLayer::GUI_2);

	return addItem(gameObject);

}


std::optional<std::shared_ptr<GameObject>> InventoryComponent::getItem(const int traitTag)
{

	for (auto item : m_items) {

		//This assumes only one item with this trait - returns first one
		if(item.gameObject.has_value() && item.gameObject.value()->hasTrait(traitTag)) {
			return item.gameObject;
			
		}
	}
	return std::nullopt;
}



int InventoryComponent::addCollectible(const CollectibleTypes collectableType, int count)
{

	m_collectibles.at(collectableType) += count;

	return m_collectibles.at(collectableType);
}

void InventoryComponent::render()
{

	if (m_isOpen) {

		//Display the backdrop object
		//if (m_displayBackdropObject.has_value()) {

		//	m_displayBackdropObject.value()->render();

		//}

		//Display all inventory items
		for (const auto& inventoryObject : m_items) {

			if (inventoryObject.gameObject.has_value() == true) {

				inventoryObject.gameObject.value()->render();
			}

		}

	}

}

void InventoryComponent::update()
{


	SDL_FPoint displayObjectPosition{};
	if (m_displayBackdropObject.has_value()) {

		displayObjectPosition = { 
			m_displayBackdropObject.value()->getTopLeftPosition().x, 
			m_displayBackdropObject.value()->getTopLeftPosition().y 
		};

	}

	//Calculate the positions of the displayed inventory items
	if (m_isOpen) {

		for (const auto& inventoryObject : m_items) {

			if (inventoryObject.gameObject.has_value() == true) {

				inventoryObject.gameObject.value()->setPosition(
					displayObjectPosition.x + inventoryObject.displayOffset.x,
					displayObjectPosition.y + inventoryObject.displayOffset.y
				);

				inventoryObject.gameObject.value()->update();
			}

		}

	}
}

//Show the inventory as a child of a specific object that is not the inventory holder
void InventoryComponent::showInventory(GameObject* proxyObject)
{
	m_isOpen = true;

	const auto& childrenComponent = proxyObject->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
	//childrenComponent->addStepChild(m_displayBackdropObject.value(), 5);

}


//Show the inventory as a child of the inventory holding object
void InventoryComponent::showInventory()
{
	m_isOpen = true;

	const auto& childrenComponent = parent()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
	//childrenComponent->addStepChild(m_displayBackdropObject.value(), 2);

}

void InventoryComponent::hideInventory()
{

	m_isOpen = false;

}


