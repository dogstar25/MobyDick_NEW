#pragma once
#include "Component.h"
#include <json/json.h>

#include <memory>

class Scene;
class HudItem;

class HudComponent : public Component 
{

public:
	HudComponent() = default;
	HudComponent(Json::Value componentJSON, GameObject* parent, Scene* parentScene);

	void update();
	void render();

private:

	//std::vector<std::shared_ptr<HudItem>> m_hudItems;
	std::shared_ptr<HudItem> m_hudItem;

};
