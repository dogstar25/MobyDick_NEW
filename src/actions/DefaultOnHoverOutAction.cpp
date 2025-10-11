#include "DefaultOnHoverOutAction.h"


#include "../GameObject.h"


void DefaultOnHoverOutAction::perform()
{
	const auto& renderComponent = m_parent->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	renderComponent->removeDisplayOverlay();


}

