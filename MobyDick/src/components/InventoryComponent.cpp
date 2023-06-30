#include "InventoryComponent.h"


InventoryComponent::InventoryComponent()
{
	m_isDependentObjectOwner = true;
}

InventoryComponent::InventoryComponent(Json::Value componentJSON, std::string parentName, Scene* parentScene)
{

	m_componentType = ComponentTypes::INVENTORY_COMPONENT;

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

	//for (auto& item : m_items)
	//{
	//	if (item.attached) {
	//		item.gameObject->render();
	//	}
	//}

}

void InventoryComponent::update()
{
	//Have each inventory game object update itself
	/*for (auto& item : m_items)
	{
		item.gameObject->update();
	}*/

}

