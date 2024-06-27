#pragma once
#include "Component.h"
#include "../Scene.h"

class LightComponent : public Component
{

	public:
		LightComponent() = default;
		LightComponent(Json::Value componentJSON, GameObject* parent);
	
		void update() override;

	protected:
		LightType m_lightType{};
		bool m_flicker{};
		void _applyFlicker();
		int m_flickerTargetBrightness{255};


};
