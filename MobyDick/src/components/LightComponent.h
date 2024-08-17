#pragma once
#include "Component.h"
#include "../Scene.h"

class LightComponent : public Component
{

	public:
		LightComponent() = default;
		LightComponent(Json::Value componentJSON, GameObject* parent);
	
		void update() override;
		void postInit() override;
		void setBrightness(int brightness) { m_brightness = brightness; }
		bool spreadsToOtherAreas() { return m_spreadsToOtherAreas; }
		void setSpreadsToOtherAreas(bool spreadsToOtherAreas) { m_spreadsToOtherAreas = spreadsToOtherAreas; }

	protected:
		LightType m_lightType{};
		bool m_flicker{};
		void _applyFlicker();
		int m_flickerTargetBrightness{255};
		int m_brightness{ 255 };
		bool m_spreadsToOtherAreas{ false };


};
