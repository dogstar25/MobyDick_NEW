#include "CloseInventoryAction.h"
#include <memory>
#include "../game.h"

extern std::unique_ptr<Game> game;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// gameObject is the object that has the inventory to be hidden
//		Grab the inventoryComponent and make sure it in an 'open' state. Get the childComponetn and remove the inventory diaply object.
//		Set the inventoryComponent to a 'closed' state.
//		Execute the normal hideInventory function.
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CloseInventoryAction::perform(GameObject* gameObject)
{

	const auto& inventoryComponent = gameObject->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

	if (inventoryComponent->isOpen()) {

		if (gameObject->parent().has_value()) {

			if (inventoryComponent->getDisplayObject().has_value() == true &&
				inventoryComponent->getDisplayObject().value().expired() == false) {

				const auto& childComponent = gameObject->parent().value()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);

				if (inventoryComponent->getDisplayObject()->expired() == false) {
					childComponent->removeChild(inventoryComponent->getDisplayObject()->lock()->id());
					inventoryComponent->setOpen(false);
					inventoryComponent->hideInventory();
				}
			}
		}

	}

}

