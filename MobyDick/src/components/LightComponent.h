#pragma once
#include "Component.h"
#include "../Scene.h"

class LightComponent : public Component
{

	LightComponent() = default;
	LightComponent(Json::Value componentJSON);

	public:
		void update() override;

	protected:
		LightType m_lightType{};
		float m_intensity{};
		float m_flicker{};


};