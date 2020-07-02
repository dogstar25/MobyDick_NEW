#include "TransformComponent.h"

#include "json/json.h"

TransformComponent::TransformComponent(Json::Value& componentDetailsJSON)
{
	//Convenience reference
	Json::Value itr = componentDetailsJSON;

	m_position.SetZero();
	m_angle = 0;

	m_size.Set(itr["size"]["width"].asFloat(), itr["size"]["height"].asFloat());
	m_absolutePositioning = itr["absolutePositioning"].asBool();
	

}

TransformComponent::~TransformComponent()
{

}

SDL_FRect TransformComponent::getPositionRect()
{
	SDL_FRect positionRect;

	positionRect.w = m_size.x;
	positionRect.h = m_size.y;

	positionRect.x = m_position.x;
	positionRect.y = m_position.y;

	return positionRect;

}

void TransformComponent::setPosition(b2Vec2 position)
{
	m_position = position;
}

void TransformComponent::setPosition(float xPosition, float yPosition)
{
	m_position.x = xPosition;
	m_position.x = yPosition;
}

void TransformComponent::setPosition(b2Vec2 position, float angle)
{
	m_position = position;
	m_angle = angle;
}

void TransformComponent::setPosition(float xPosition, float yPosition, float angle)
{
	m_position.x = xPosition;
	m_position.x = yPosition;
	m_angle = angle;
}

void TransformComponent::setSize(b2Vec2 size)
{
	m_size = size;
}

void TransformComponent::setSize(float xSize, float ySize)
{
	m_size.x = xSize;
	m_size.y = ySize;
}

void TransformComponent::setAngle(float angle)
{
	m_angle = angle;
}
