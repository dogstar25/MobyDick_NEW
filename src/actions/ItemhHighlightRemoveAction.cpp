#include "ItemhHighlightRemoveAction.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

void ItemhHighlightRemoveAction::perform()
{

	const auto& renderComponent = m_parent->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->removeDisplayOverlay();


}
