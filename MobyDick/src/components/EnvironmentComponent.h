#pragma once
#include "Component.h"
#include <optional>
#include <set>
#include <map>
#include <unordered_map>
#include "../BaseConstants.h"

#include "../GameObject.h"


class EnvironmentComponent : public Component
{

public:
	EnvironmentComponent(Json::Value definitionJSON);
	~EnvironmentComponent() = default;

	virtual void update() override;
	virtual void postInit() override;
	virtual void setParent(GameObject* gameObject) override;

protected:


private:

	
	

	


};

