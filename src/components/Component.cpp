#include "Component.h"


Component::Component(int componentType, GameObject* parent)
{

	m_componentType = componentType;
	setParent(parent);

}

Component::~Component()
{

}

void Component::setParent(GameObject* gameObject)
{

	m_parentGameObject = gameObject;

}

