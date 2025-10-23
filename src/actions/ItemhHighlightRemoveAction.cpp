#include "ItemhHighlightRemoveAction.h"
#include "../game.h"

void ItemhHighlightRemoveAction::perform()
{

	const auto& renderComponent = m_parent->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->removeDisplayOverlay();


}
