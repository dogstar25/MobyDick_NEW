#include "PoolComponent.h"


PoolComponent::PoolComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::POOL_COMPONENT, parent)
{

	m_isAvailable = true;
	m_isActive = false;
}

PoolComponent::~PoolComponent()
{

}

void PoolComponent::update()
{

}

void PoolComponent::reset()
{
	m_isActive = false;
	m_isAvailable = true;

}


