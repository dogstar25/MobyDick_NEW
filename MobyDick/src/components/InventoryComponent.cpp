#include "InventoryComponent.h"

#include "../GameObject.h"


InventoryComponent::InventoryComponent()
{

	m_activeItem = 0;

}

InventoryComponent::~InventoryComponent()
{

}

int InventoryComponent::addItem(GameObject* gameObject)
{

	m_items.push_back(gameObject);

	return(m_items.size());

}
