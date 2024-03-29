#pragma once

#include <memory>
#include <bitset>
#include <json/json.h>

#include "../Util.h"
#include "../BaseConstants.h"

class GameObject;

class Component
{

public:

	Component(int componentType);
	~Component();
	virtual void update() = 0;
	virtual void render() {};
	virtual void postInit() {};
	virtual void setParent(GameObject* gameObject) { m_parentGameObject = gameObject; }

	void disable() { disabled = true; }
	void enable() { disabled = false; }
	bool isDisabled() { return disabled; }
	bool isDependentObjectOwner() { return m_isDependentObjectOwner; }
	
	GameObject* parent() { return m_parentGameObject; }
	int componentType() { return m_componentType; }

protected:
	std::string m_name{};
	GameObject* m_parentGameObject{ nullptr };
	int m_componentType{};
	bool disabled{ false };
	bool m_isDependentObjectOwner{};


};

