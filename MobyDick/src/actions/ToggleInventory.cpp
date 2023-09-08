#include "ToggleInventory.h"
#include <memory>
#include "../game.h"

extern std::unique_ptr<Game> game;

void ToggleInventory::perform(GameObject* gameObject)
{

	const auto& inventoryComponent = gameObject->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

	if (inventoryComponent->isOpen()) {
		
		inventoryComponent->hideInventory();

	}
	else {

		const auto& childComponent = gameObject->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
		childComponent->addStepChild(inventoryComponent->getDisplayBackdropObject().value(), PositionAlignment::TOP_LEFT);

		inventoryComponent->showInventory();

	}

}

