#include "ButtonCloseInventoryAction.h"
#include <memory>
#include "../game.h"

void ButtonCloseInventoryAction::perform()
{
	//This gameObject is the inventory display object. 
	//The Inventory display objects parent is the game object whose inventory we have to close
	const auto& inventoryHoldingObject = m_parent->parent();

	if (inventoryHoldingObject.has_value()) {

		const auto& inventoryComponent = inventoryHoldingObject.value()->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);
		if (inventoryComponent->isOpen()) {
			const auto& actionComponent = inventoryHoldingObject.value()->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);

			const auto& closeAction = actionComponent->getAction(Actions::CLOSE);
			closeAction->perform();
		}

	}

}

