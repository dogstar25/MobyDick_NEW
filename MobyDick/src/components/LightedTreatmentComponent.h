#pragma once
#include "Component.h"
class LightedTreatmentComponent : public Component
{


	LightedTreatmentComponent() = default;
	LightedTreatmentComponent(Json::Value componentJSON);

public:
	void update() override;
	void render();



};

