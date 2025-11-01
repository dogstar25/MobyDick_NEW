#include "ItemhHighlightApplyAction.h"
#include "../game.h"

void ItemhHighlightApplyAction::perform()
{

	const auto& renderComponent = m_parent->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->applyDisplayOverlay(displayOverlays::outline_SUBTLE_1);


}
