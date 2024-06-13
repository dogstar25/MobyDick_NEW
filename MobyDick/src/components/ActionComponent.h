#pragma once
#include "Component.h"

#include <map>
#include <vector>
#include <optional>

#include <json/json.h>
#include <box2d/box2d.h>

#include "../actions/Action.h"
#include "../actions/InteractAction.h"
#include "../actions/UseAction.h"
#include "../actions/NoAction.h"
#include "../Util.h"
#include "../BaseConstants.h"
#include "../JsonSerialization.h"


class GameObject;
class Scene;

class ActionComponent : public Component
{

friend void Json::serialize(Json::Value& value, ActionComponent& o);
friend void Json::deserialize(Json::Value& value, ActionComponent& o);

public:
	ActionComponent(Json::Value definitionJSON, Scene* parentScene);
	~ActionComponent() = default;

	void update() {};

	std::shared_ptr<Action> getAction(int actionId);
	bool hasAction(int actionId);

private:
	std::vector<std::shared_ptr<Action>>m_actions;

};


// Serialization and Deserialization
namespace Json {

	template<>
	void serialize<ActionComponent>(Json::Value& value, ActionComponent& o);

	template<>
	void deserialize<ActionComponent>(Json::Value& value, ActionComponent& o);
}
