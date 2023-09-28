#include "CloseShelfInventoryAction.h"
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
void CloseShelfInventoryAction::perform(GameObject* gameObject)
{


	//This gameObject is the object that has the inventory to be hidden
	//This is a shelf so we're really just going to force any item currently being dragged that lives in this inventory
	const auto& inventoryComponent = gameObject->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);
	for (auto& item : inventoryComponent->items()) {

		if (item.has_value() && item.value()->isDragging()) {
			item.value()->clearDragging();
			break;
		}
	}



	//gameObject->stash();
	//gameObject->clearDragging();

	//inventoryComponent->refreshInventoryDisplay();

}

