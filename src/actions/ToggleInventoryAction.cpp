#include "ToggleInventoryAction.h"
#include <memory>
#include "../game.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// gameObject is the object that has the inventory to be shown or hidden
//		We have a separate CLOSE action that we want to reuse. Grab this action from this gameObject and perform it, passing in 
//		this gameObject as the parameter
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ToggleInventoryAction::perform()
{

	const auto& inventoryComponent = m_parent->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

	if (inventoryComponent->isOpen()) {

		const auto& actionComponent = m_parent->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);

		const auto& closeAction = actionComponent->getAction(Actions::CLOSE);
		closeAction->perform();

	}
	else {

		const auto& childComponent = m_parent->parent().value()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
		childComponent->addStepChild(inventoryComponent->getDisplayObject().value().lock(), PositionAlignment::TOP_CENTER);

		inventoryComponent->setOpen(true);
		inventoryComponent->showInventory();

	}

}

