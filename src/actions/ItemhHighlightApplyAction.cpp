#include "ItemhHighlightApplyAction.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

void ItemhHighlightApplyAction::perform()
{

	const auto& renderComponent = m_parent->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->applyDisplayOverlay(displayOverlays::outline_SUBTLE_1);


}
