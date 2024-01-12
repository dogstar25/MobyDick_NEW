#include "TakeItemAction.h"
#include <memory>
#include "../game.h"

extern std::unique_ptr<Game> game;

void TakeItemAction::perform(GameObject* gameObject)
{

	//Only if this object is obtainable. This action can fire for inventory items that are sometime in inventory and NOT obtainable
	 
	if ( (gameObject->hasState(GameObjectState::ITEM_OBTAINABLE) && gameObject->hasState(GameObjectState::ITEM_LOOSE) == false ) ||
		(gameObject->hasState(GameObjectState::ITEM_OBTAINABLE) && gameObject->hasState(GameObjectState::ITEM_LOOSE) == true && gameObject->isTouchingByTrait(TraitTag::player)) ||
		(gameObject->hasState(GameObjectState::ITEM_OBTAINABLE) && gameObject->hasState(GameObjectState::ITEM_STORED_OPEN) == true && gameObject->isTouchingByTrait(TraitTag::player))) {
		//Get the players inventory
		const auto& player = gameObject->parentScene()->getFirstGameObjectByTrait(TraitTag::player);
		const auto& inventoryComponent = player.value()->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

		const auto& gameObjectShrPtr = gameObject->parentScene()->getGameObject(gameObject->id());

		//If this item is loose then add it to the inventory and remove it from the world
		if (gameObject->hasState(GameObjectState::ITEM_LOOSE) && gameObject->hasState(GameObjectState::ITEM_STORED_OPEN) == false) {

			if (inventoryComponent->addItem(gameObjectShrPtr.value()) == true) {

				gameObjectShrPtr.value()->setRemoveFromWorld(true);
				inventoryComponent->refreshInventoryDisplay();


			}
			else {
				std::cout << "Inventory is full!" << std::endl;
			}

		}
		else {

			const auto& gameObjectParent = gameObject->parent();
			const auto& sourceInventoryObject = gameObjectParent.value()->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

			//First remove it
			std::shared_ptr<GameObject> gameObjectSharedPtr = sourceInventoryObject->removeItem(gameObject);

			if (inventoryComponent->addItem(gameObjectShrPtr.value()) == true) {
				
				inventoryComponent->refreshInventoryDisplay();
				sourceInventoryObject->refreshInventoryDisplay();
			}
			else {
				//Add it back to where we removed it from 
				sourceInventoryObject->addItem(gameObjectShrPtr.value());
				std::cout << "Inventory is full!" << std::endl;
			}

		}

		//Make sure we clear of this gameObject as the current object interface
		if (gameObject->hasComponent(ComponentTypes::INTERFACE_COMPONENT)) {

			const auto& interfaceComponent = gameObject->getComponent<InterfaceComponent>(ComponentTypes::INTERFACE_COMPONENT);
			interfaceComponent->clearSpecificGameObjectInterface(gameObject);
		}

		

		
	}
}

