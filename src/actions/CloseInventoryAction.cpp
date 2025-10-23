#include "CloseInventoryAction.h"
#include "../components/InventoryComponent.h"
#include <memory>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// gameObject is the object that has the inventory to be hidden
//		Grab the inventoryComponent and make sure it in an 'open' state. Get the childComponetn and remove the inventory diaply object.
//		Set the inventoryComponent to a 'closed' state.
//		Execute the normal hideInventory function.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CloseInventoryAction::perform()
{

	const auto& inventoryComponent = m_parent->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

	if (inventoryComponent->isOpen()) {

		if (m_parent->parent().has_value()) {

			if (inventoryComponent->getDisplayObject().has_value() == true &&
				inventoryComponent->getDisplayObject().value().expired() == false) {

				const auto& childComponent = m_parent->parent().value()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);

				if (inventoryComponent->getDisplayObject()->expired() == false) {
					childComponent->removeChild(inventoryComponent->getDisplayObject()->lock()->id());
					inventoryComponent->setOpen(false);
					inventoryComponent->hideInventory();
				}
			}
		}

	}

}

