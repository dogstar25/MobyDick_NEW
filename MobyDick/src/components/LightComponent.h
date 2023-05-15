#pragma once
#include "Component.h"
#include "../Scene.h"

class LightComponent : public Component
{

	public:
		LightComponent() = default;
		LightComponent(Json::Value componentJSON);
	
		void update() override;

	protected:
		LightType m_lightType{};
		float m_intensity{};
		float m_flicker{};


};
