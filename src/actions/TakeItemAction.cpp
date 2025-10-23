#include "TakeItemAction.h"
#include <memory>
#include "../include/GameGlobals.h"
#include "../game.h"


//extern std::unique_ptr<Game> game;

void TakeItemAction::perform()
{

	//Only if this object is obtainable. This action can fire for inventory items that are sometime in inventory and NOT obtainable
	 
	if ( (m_parent->hasState(GameObjectState::ITEM_OBTAINABLE) && m_parent->hasState(GameObjectState::ITEM_LOOSE) == false ) ||
		(m_parent->hasState(GameObjectState::ITEM_OBTAINABLE) && m_parent->hasState(GameObjectState::ITEM_LOOSE) == true && m_parent->isTouchingByTrait(TraitTag::player)) ||
		(m_parent->hasState(GameObjectState::ITEM_OBTAINABLE) && m_parent->hasState(GameObjectState::ITEM_STORED_OPEN) == true && m_parent->isTouchingByTrait(TraitTag::player))) {
		//Get the players inventory
		const auto& player = m_parent->parentScene()->player();
		const auto& inventoryComponent = player->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

		//If this item is loose then add it to the inventory and remove it from the world
		if (m_parent->hasState(GameObjectState::ITEM_LOOSE) && m_parent->hasState(GameObjectState::ITEM_STORED_OPEN) == false) {

			if (inventoryComponent->isFull() == false) {

				auto extractedGameObject = m_parent->parentScene()->extractGameObject(m_parent->id());

				//This object could have been a child of another object so if it is remove it from that parent as well
				if (extractedGameObject.value()->isChild == true) {

					const auto& childrenComponent = extractedGameObject.value()->parent().value()->getComponent<ChildrenComponent>(ComponentTypes::CHILDREN_COMPONENT);
					childrenComponent->removeChild(extractedGameObject.value()->id());

				}

				if (inventoryComponent->addItem(extractedGameObject.value()) == true) {

					inventoryComponent->refreshInventoryDisplay();

					const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
					physicsComponent->setIsSensor(true);

				}
				else {
					//If it failed to add for some reason then add it back to the world, because we extracted it above
					m_parent->parentScene()->addGameObject(extractedGameObject.value(), extractedGameObject.value()->layer());
					std::cout << "Failed to add!" << std::endl;
				}
			}

		}
		else {

			const auto& gameObjectParent = m_parent->parent();
			const auto& sourceInventoryObject = gameObjectParent.value()->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

			const auto& gameObjectShrPtr = m_parent->parentScene()->getGameObject(m_parent->id());

			//First remove it
			std::shared_ptr<GameObject> gameObjectSharedPtr = sourceInventoryObject->removeItem(m_parent);

			if (inventoryComponent->addItem(gameObjectShrPtr.value()) == true) {
				
				inventoryComponent->refreshInventoryDisplay();
				sourceInventoryObject->refreshInventoryDisplay();

				const auto& physicsComponent = m_parent->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
				physicsComponent->setIsSensor(true);

			}
			else {
				//Add it back to where we removed it from 
				sourceInventoryObject->addItem(gameObjectShrPtr.value());
				std::cout << "Inventory is full!" << std::endl;
			}

		}

		//Make sure we clear of this gameObject as the current object interface
		if (m_parent->hasComponent(ComponentTypes::INTERFACE_COMPONENT)) {

			const auto& interfaceComponent = m_parent->getComponent<InterfaceComponent>(ComponentTypes::INTERFACE_COMPONENT);
			interfaceComponent->clearSpecificGameObjectInterface(m_parent);
		}

		

		
	}
}

