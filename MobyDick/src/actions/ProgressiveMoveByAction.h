#pragma once
#include "Action.h"
#include <optional>
#include <box2d/box2d.h>

class ProgressiveMoveByAction : public Action
{

public:

	ProgressiveMoveByAction(Json::Value properties, GameObject* parent)
		:Action(properties, parent) {}

	void perform() override;

	virtual void update() override;


private:

	b2Vec2 m_destination{};
	std::optional<float> m_lastSavedDistance{};
};


