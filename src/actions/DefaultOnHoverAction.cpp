#include "DefaultOnHoverAction.h"


#include "../GameObject.h"

void DefaultOnHoverAction::perform()
{
	const auto& renderComponent = m_parent->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->applyDisplayOverlay(displayOverlays::outline_GREEN1);


}
