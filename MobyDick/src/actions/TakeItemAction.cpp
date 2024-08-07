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

		//If this item is loose then add it to the inventory and remove it from the world
		if (gameObject->hasState(GameObjectState::ITEM_LOOSE) && gameObject->hasState(GameObjectState::ITEM_STORED_OPEN) == false) {

			if (inventoryComponent->isFull() == false) {

				auto extractedGameObject = gameObject->parentScene()->extractGameObject(gameObject->id());

				//This object could have been a child of another object so if it is remove it from that parent as well
				if (extractedGameObject.value()->isChild == true) {

					const auto& childrenComponent = extractedGameObject.value()->parent().value()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
					childrenComponent->removeChild(extractedGameObject.value()->id());

				}

				if (inventoryComponent->addItem(extractedGameObject.value()) == true) {

					inventoryComponent->refreshInventoryDisplay();

					const auto& physicsComponent = gameObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
					physicsComponent->setIsSensor(true);

				}
				else {
					//If it failed to add for some reason then add it back to the world, because we extracted it above
					gameObject->parentScene()->addGameObject(extractedGameObject.value(), extractedGameObject.value()->layer());
					std::cout << "Failed to add!" << std::endl;
				}
			}

		}
		else {

			const auto& gameObjectParent = gameObject->parent();
			const auto& sourceInventoryObject = gameObjectParent.value()->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

			const auto& gameObjectShrPtr = gameObject->parentScene()->getGameObject(gameObject->id());

			//First remove it
			std::shared_ptr<GameObject> gameObjectSharedPtr = sourceInventoryObject->removeItem(gameObject);

			if (inventoryComponent->addItem(gameObjectShrPtr.value()) == true) {
				
				inventoryComponent->refreshInventoryDisplay();
				sourceInventoryObject->refreshInventoryDisplay();

				const auto& physicsComponent = gameObject->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
				physicsComponent->setIsSensor(true);

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

