#pragma once
#include "Component.h"
#include "../texture.h"
class LightedTreatmentComponent : public Component
{

	public:
		LightedTreatmentComponent() = default;
		LightedTreatmentComponent(Json::Value componentJSON, GameObject* parent);
		~LightedTreatmentComponent();

		void buildDynamicLightTexture();

		void update() override;
		void render();
		void postInit() override;

	private:
		std::vector<std::weak_ptr<GameObject>> m_lights{};
		std::shared_ptr<Texture> m_lightCompositeTexture{};
		bool _hasLineOfSightToLitArea(GameObject* lightObject);


};

