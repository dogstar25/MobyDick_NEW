#pragma once
#include <memory>
#include <string>


class Scene;
class HudItem;
class GameObject;

enum class HudItemTypes {
	STATUS_SINGLE,
	STATUS_SERIES,

	LAST_INDEX,
	COUNT
};

class HudItemFactory
{

public:

	static HudItemFactory& instance();
	std::shared_ptr<HudItem> create(HudItemTypes type, GameObject* parent, std::string labelId, std::string statusId, std::string statusValueId,
		float labelPadding, Scene* parentScene);


private:

};

