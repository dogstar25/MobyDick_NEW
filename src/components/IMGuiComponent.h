#pragma once
#include "Component.h"
#include <json/json.h>
#include <memory>
#include "../Scene.h"

class IMGuiItem;

class IMGuiComponent : public Component {


public:
	IMGuiComponent() = default;
	IMGuiComponent(Json::Value componentJSON, GameObject* parent, std::string gameObjectType, Scene* parentScene);

	void update() override;
	void render();

	std::shared_ptr<IMGuiItem> getIMGuiItem() { return m_IMGuiItem; }

private:

	std::shared_ptr<IMGuiItem> m_IMGuiItem;
	
};
