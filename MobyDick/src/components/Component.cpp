#include "Component.h"


Component::Component(int componentType)
{

	switch (componentType) {

		case ComponentTypes::ACTION_COMPONENT:
			m_componentType = ComponentTypes::ACTION_COMPONENT;
			break;
		case ComponentTypes::ANIMATION_COMPONENT:
			m_componentType = ComponentTypes::ANIMATION_COMPONENT;
			break;
		case ComponentTypes::ATTACHMENTS_COMPONENT:
			m_componentType = ComponentTypes::ATTACHMENTS_COMPONENT;
			break;
		case ComponentTypes::BRAIN_COMPONENT:
			m_componentType = ComponentTypes::BRAIN_COMPONENT;
			break;
		case ComponentTypes::CHECKPOINT_COMPONENT:
			m_componentType = ComponentTypes::CHECKPOINT_COMPONENT;
			break;
		case ComponentTypes::CHILDREN_COMPONENT:
			m_componentType = ComponentTypes::CHILDREN_COMPONENT;
			break;
		case ComponentTypes::COMPOSITE_COMPONENT:
			m_componentType = ComponentTypes::COMPOSITE_COMPONENT;
			break;
		case ComponentTypes::CONTAINER_COMPONENT:
			m_componentType = ComponentTypes::CONTAINER_COMPONENT;
			break;
		case ComponentTypes::GRID_DISPLAY_COMPONENT:
			m_componentType = ComponentTypes::GRID_DISPLAY_COMPONENT;
			break;
		case ComponentTypes::HUD_COMPONENT:
			m_componentType = ComponentTypes::HUD_COMPONENT;
			break;
		case ComponentTypes::IMGUI_COMPONENT:
			m_componentType = ComponentTypes::IMGUI_COMPONENT;
			break;
		case ComponentTypes::INTERFACE_COMPONENT:
			m_componentType = ComponentTypes::INTERFACE_COMPONENT;
			break;
		case ComponentTypes::INVENTORY_COMPONENT:
			m_componentType = ComponentTypes::INVENTORY_COMPONENT;
			break;
		case ComponentTypes::LIGHT_COMPONENT:
			m_componentType = ComponentTypes::LIGHT_COMPONENT;
			break;
		case ComponentTypes::LIGHTED_TREATMENT_COMPONENT:
			m_componentType = ComponentTypes::LIGHTED_TREATMENT_COMPONENT;
			break;
		case ComponentTypes::NAVIGATION_COMPONENT:
			m_componentType = ComponentTypes::NAVIGATION_COMPONENT;
			break;
		case ComponentTypes::PARTICLE_COMPONENT:
			m_componentType = ComponentTypes::PARTICLE_COMPONENT;
			break;
		case ComponentTypes::PARTICLE_X_COMPONENT:
			m_componentType = ComponentTypes::PARTICLE_X_COMPONENT;
			break;
		case ComponentTypes::PHYSICS_COMPONENT:
			m_componentType = ComponentTypes::PHYSICS_COMPONENT;
			break;
		case ComponentTypes::PLAYER_CONTROL_COMPONENT:
			m_componentType = ComponentTypes::PLAYER_CONTROL_COMPONENT;
			break;
		case ComponentTypes::POOL_COMPONENT:
			m_componentType = ComponentTypes::POOL_COMPONENT;
			break;
		case ComponentTypes::PUZZLE_COMPONENT:
			m_componentType = ComponentTypes::PUZZLE_COMPONENT;
			break;
		case ComponentTypes::RENDER_COMPONENT:
			m_componentType = ComponentTypes::RENDER_COMPONENT;
			break;
		case ComponentTypes::SOUND_COMPONENT:
			m_componentType = ComponentTypes::SOUND_COMPONENT;
			break;
		case ComponentTypes::STATE_COMPONENT:
			m_componentType = ComponentTypes::STATE_COMPONENT;
			break;
		case ComponentTypes::TEXT_COMPONENT:
			m_componentType = ComponentTypes::TEXT_COMPONENT;
			break;
		case ComponentTypes::TRANSFORM_COMPONENT:
			m_componentType = ComponentTypes::TRANSFORM_COMPONENT;
			break;
		case ComponentTypes::VITALITY_COMPONENT:
			m_componentType = ComponentTypes::VITALITY_COMPONENT;
			break;
		case ComponentTypes::WEAPON_COMPONENT:
			m_componentType = ComponentTypes::WEAPON_COMPONENT;
			break;

		default:
			SDL_assert(false && "InvalidComponentType");

	}




}

Component::~Component()
{

}


