#include "ItemhHighlightRemoveAction.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

void ItemhHighlightRemoveAction::perform(GameObject* gameObject)
{

	const auto& renderComponent = gameObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->removeDisplayOverlay();


}
