#include "TransformComponent.h"
#include "../Game.h"

extern std::unique_ptr<Game> game;

TransformComponent::TransformComponent() :
	Component(ComponentTypes::TRANSFORM_COMPONENT)
{
}

TransformComponent::TransformComponent(Json::Value componentJSON, float xMapPos, float yMapPos, float angleAdjust, b2Vec2 sizeOverride) :
	Component(ComponentTypes::TRANSFORM_COMPONENT)
{
		m_angle = m_originalAngle = angleAdjust;
		b2Vec2 size{};

		//Apply override if it's not zero
		if (sizeOverride != b2Vec2_zero) {
			size = sizeOverride;
		}
		else {
			size.x = componentJSON["size"]["width"].asFloat();
			size.y = componentJSON["size"]["height"].asFloat();
		}

		m_originalTilePosition = { xMapPos , yMapPos };
		setPosition(util::tileToPixelPlacementLocation(xMapPos, yMapPos, size.x, size.y)	);

		m_originalPosition = m_position;
		m_originalSize = m_size = size;

		m_absolutePositioning = m_originalAbsolutePositioning = componentJSON["absolutePositioning"].asBool();

		//Determine location
		auto locationJSON = componentJSON["location"];
		if (locationJSON.isMember("windowPosition")) {

			PositionAlignment windowPosition = static_cast<PositionAlignment>(game->enumMap()->toEnum(componentJSON["location"]["windowPosition"].asString()));
			m_windowRelativePosition = windowPosition;

			if (locationJSON.isMember("adjust")) {
				auto adjustX = locationJSON["adjust"]["x"].asFloat();
				auto adjustY = locationJSON["adjust"]["y"].asFloat();
				m_windowPositionAdjustment = { adjustX , adjustY };
			}

		}

}


TransformComponent::~TransformComponent()
{

}


void TransformComponent::update()
{

	//If this is using the window relative positioning, then we may need to adjust the position
	//because the object may have changed size, i.e ImGui Window
	if (m_windowRelativePosition) {
		parent()->setPosition(m_windowRelativePosition.value(), m_windowPositionAdjustment.x, m_windowPositionAdjustment.y);
	}

	if (m_absolutePositioning == true && parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == false) {

		SDL_FPoint cameraPosition = { Camera::instance().frame().x, Camera::instance().frame().y };

		m_position = { m_position.x + cameraPosition.x, m_position.y + cameraPosition.y};

	}


}

SDL_FRect TransformComponent::getPositionRect()
{
	SDL_FRect positionRect{};
	SDL_FPoint position{};

	positionRect.w = m_size.x;
	positionRect.h = m_size.y;

	//Adjust to make it top left
	position = getTopLeftPosition();
	positionRect.x = position.x;
	positionRect.y = position.y;

	return positionRect;

}

SDL_FPoint TransformComponent::getCenterPosition()
{
	return SDL_FPoint{ m_position.x, m_position.y };

}

SDL_FPoint TransformComponent::getTopLeftPosition()
{
	SDL_FPoint position{};

	position.x = m_position.x - (m_size.x / 2);
	position.y = m_position.y - (m_size.y / 2);

	return position;
}

void TransformComponent::setPosition(b2Vec2 position)
{
	m_position = position;
}

void TransformComponent::setPosition(SDL_FPoint position)
{
	m_position.x = position.x;
	m_position.y = position.y;
}

void TransformComponent::setPosition(float xPosition, float yPosition)
{
	m_position.x = xPosition;
	m_position.y = yPosition;
}

void TransformComponent::setPosition(b2Vec2 position, float angle)
{
	m_position = position;
	m_angle = angle;
}

void TransformComponent::setPosition(float xPosition, float yPosition, float angle)
{
	m_position.x = xPosition;
	m_position.y = yPosition;
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

void TransformComponent::setLine(b2Vec2 start, b2Vec2 end)
{
	m_lineStart = start;
	m_lineEnd = end;
}

void TransformComponent::setAbsolutePositioning(bool absolutePositioning)
{
	m_absolutePositioning = absolutePositioning;
}


