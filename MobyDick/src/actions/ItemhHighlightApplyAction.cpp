#include "ItemhHighlightApplyAction.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

void ItemhHighlightApplyAction::perform(GameObject* gameObject)
{

	const auto& renderComponent = gameObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->applyDisplayOverlay(displayOverlays::outline_SUBTLE_1);


}
