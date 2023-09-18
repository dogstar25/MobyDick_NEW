#include "TakeItemAction.h"
#include <memory>
#include "../game.h"

extern std::unique_ptr<Game> game;

void TakeItemAction::perform(GameObject* gameObject)
{

	//Only if this object is obtainable. This action can fire for inventory items that are sometime in inventory and NOT obtainable
	 
	if (gameObject->hasTrait(TraitTag::obtainable)) {
		//Get the players inventory
		const auto& player = gameObject->parentScene()->getFirstGameObjectByTrait(TraitTag::player);
		const auto& inventoryComponent = player.value()->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

		const auto& gameObjectShrPtr = gameObject->parentScene()->getGameObject(gameObject->id());

		
		if(inventoryComponent->addItem(gameObjectShrPtr.value()) != std::nullopt){

			gameObjectShrPtr.value()->setRemoveFromWorld(true);
			//gameObjectShrPtr.value()->setRemoveFromWorld(true);
			inventoryComponent->refreshInventoryDisplay();
		}
	}
}

