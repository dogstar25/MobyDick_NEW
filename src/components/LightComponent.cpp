#include "LightComponent.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

LightComponent::LightComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::LIGHT_COMPONENT, parent)
{

	m_componentType = ComponentTypes::LIGHT_COMPONENT;

	m_lightType = static_cast<LightType>(game->enumMap()->toEnum(componentJSON["type"].asString()));
	m_flicker = componentJSON["flicker"].asBool();

	if (componentJSON.isMember("brightness")) {
		m_brightness = componentJSON["brightness"].asInt();
	}
	else {
		//default to maximum brightness
		m_brightness = 100;
	}

	if (componentJSON.isMember("spreadsToOtherAreas")) {
		m_spreadsToOtherAreas = componentJSON["spreadsToOtherAreas"].asBool();
	}


}


void LightComponent::postInit()
{

	//The brightness is controlled by setting the alpha value of the color
	const auto& lightRenderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Light brightness should be 1 to 100
	int brightness = static_cast<int>(m_brightness * 255.0 / 100.0);
	if (brightness > 255) {
		brightness = 255;
	}
	lightRenderComponent->setColorAlpha(brightness);

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