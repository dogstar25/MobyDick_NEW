#include "LightComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

LightComponent::LightComponent(Json::Value componentJSON)
{

	m_componentType = ComponentTypes::LIGHT_COMPONENT;

	m_lightType = static_cast<LightType>(game->enumMap()->toEnum(componentJSON["type"].asString()));
	m_intensity = componentJSON["intensity"].asFloat();
	m_flicker = componentJSON["flicker"].asFloat();
	
	if (m_flicker > 0) {

		m_flickerTimer = Timer(.1, true);
	}

}


void LightComponent::update()
{

	//If this light is supposed to flicker then apply flicker logic

	static int count = 0;
	if (m_flicker > 0) {

		//if (m_flickerTimer.hasMetTargetDuration()) {
			
			//std::cout << "Time" << count++ << std::endl;
			_applyFlicker(m_flicker);
			auto timerValue = util::generateRandomNumber((float).033, (float).066);
			m_flickerTimer = Timer(timerValue);
		//}
	}


}

void LightComponent::_applyFlicker(float flickerValue)
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