#include "TakeItemAction.h"
#include <memory>
#include "../game.h"

extern std::unique_ptr<Game> game;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// gameObject is the object that has the inventory to be shown or hidden
//		We have a separate CLOSE action that we want to reuse. Grab this action from this gameObject and perform it, passing in 
//		this gameObject as the parameter
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TakeItemAction::perform(GameObject* gameObject)
{

	//Get the players inventory
	const auto& player = gameObject->parentScene()->getFirstGameObjectByTrait(TraitTag::player);
	const auto& inventoryComponent = player.value()->getComponent<InventoryComponent>(ComponentTypes::INVENTORY_COMPONENT);

	const auto& gameObjectShrPtr = gameObject->parentScene()->getGameObject(gameObject->id());

	gameObjectShrPtr.value()->setRemoveFromWorld(true);
	inventoryComponent->addItem(gameObjectShrPtr.value());
	//gameObjectShrPtr.value()->setRemoveFromWorld(true);
	inventoryComponent->refreshInventoryDisplay();

}

