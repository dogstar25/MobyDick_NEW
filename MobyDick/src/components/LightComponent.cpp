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
			_applyFlicker2(m_flicker);
			auto timerValue = util::generateRandomNumber((float).016, (float).03);
			m_flickerTimer = Timer(timerValue);
		//}
	}


}

void LightComponent::_applyFlicker(float flickerValue)
{

	auto brightLevel = util::generateRandomNumber(-m_flicker, m_flicker);

	auto currentColor = parent()->getColor();
	if(currentColor.a + brightLevel > 255){
		currentColor.a = 255;
	}
	else if (currentColor.a + brightLevel < 100) {
		currentColor.a = 100;
	}
	else {
		currentColor.a += brightLevel;
	}

	parent()->setColor(currentColor);

	std::cout << "Color" << (float)parent()->getColor().a << std::endl;

}

void LightComponent::_applyFlicker2(float flickerValue)
{

	auto currentColor = parent()->getColor();

	//Have we reached the taget brightness
	if (currentColor.a == m_flickerTargetBrightness) {

		m_flickerTargetBrightness = util::generateRandomNumber(200, 255);
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