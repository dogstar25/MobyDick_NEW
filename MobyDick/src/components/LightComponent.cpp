#include "LightComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

LightComponent::LightComponent(Json::Value componentJSON) :
	Component(ComponentTypes::LIGHT_COMPONENT)
{

	m_componentType = ComponentTypes::LIGHT_COMPONENT;

	m_lightType = static_cast<LightType>(game->enumMap()->toEnum(componentJSON["type"].asString()));
	m_flicker = componentJSON["flicker"].asBool();
	

}


void LightComponent::update()
{

	//If this light is supposed to flicker then apply flicker logic

	static int count = 0;
	if (m_flicker) {

		_applyFlicker();

	}


}

void LightComponent::_applyFlicker()
{

	auto currentColor = parent()->getColor();

	//Have we reached the taget brightness
	if (currentColor.a == m_flickerTargetBrightness) {

		m_flickerTargetBrightness = util::generateRandomNumber(100, 255);
	}
	else {
		if (m_flickerTargetBrightness > parent()->getColor().a) {

			currentColor.a += 1;

		}
		else {
			currentColor.a -= 1;
		}

		parent()->setColor(currentColor);

	}

}