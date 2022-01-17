#include "VitalityComponent.h"

#include "../GameObject.h"


VitalityComponent::VitalityComponent()
{

}


VitalityComponent::VitalityComponent(Json::Value componentJSON)
{

	m_speed = componentJSON["speed"].asFloat();
	m_rotationSpeed = componentJSON["rotationSpeed"].asFloat();
	m_lifetime = std::chrono::duration<float>(componentJSON["lifetime"].asFloat());
	
	float lifetime = componentJSON["lifetime"].asFloat();
	m_lifetimeTimer = Timer(lifetime);

	m_isLifetimeAlphaFade = componentJSON["lifetimeAlphaFade"].asBool();

	//Regeneration related
	if (componentJSON.isMember("regenerating")) {
		m_isRegenerative = true;
		auto& regenJSON = componentJSON["regenerating"];

			m_regenSpeed = regenJSON["regenerateSpeed"].asFloat();
			m_resistance = regenJSON["resistance"].asFloat();

			auto level = 0;
			for (Json::Value itrlevel : regenJSON["levels"]) {

				LevelDefinition levelItem = {};
				levelItem.levelNum = itrlevel["level"].asInt();
				levelItem.resistance = itrlevel["resistance"].asInt();
				levelItem.color = util::JsonToColor(itrlevel["color"]);

				m_regenLevels[level] = levelItem;
				level++;
			}

		m_maxLevels = level;

		//Set values for first level
		m_resistance = m_regenLevels[0].resistance;
		m_currentLevel = 1;
	}

}

VitalityComponent::~VitalityComponent()
{

}

void VitalityComponent::update()
{

	if (m_lifetimeTimer.infiniteLifetime() == false) {
		_updateFiniteLifetime();
	}

	if (m_isRegenerative) {
		_updateRegeneration();
	}
}

void VitalityComponent::_levelUp()
{

	if (m_currentLevel < m_maxLevels) {

		m_currentLevel++;
		auto& level = m_regenLevels[(uint_fast64_t)m_currentLevel - 1];
		m_isBroken = false;
		parent()->resetCollisionTag();
		m_resistance = level.resistance;
		
		//Update the color based on the new level
		const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
		renderComponent->setColor(level.color);

	}

}

void VitalityComponent::_updateFiniteLifetime()
{

	if (m_lifetimeTimer.hasMetTargetDuration())
	{

		//Mark this object for removal so that the removal loop will delete it
		parent()->setRemoveFromWorld(true);

	}
	else
	{

		//If this particle should fade over time, then adjust its alpha value
		if (m_isLifetimeAlphaFade)
		{
			//Todo:move this to the render component and have it check the lifetime to adjust its alpha
			parent()->getComponent<RenderComponent>(
				ComponentTypes::RENDER_COMPONENT)->setColorAlpha(int(255 * (m_lifetimeTimer.percentTargetMet())));
		}

	}

}

void VitalityComponent::_updateRegeneration()
{

	//If the broken flag was set in the contact listener, then set the collision tag to FREE
	if (m_isBroken == true) {
		parent()->setCollisionTag(CollisionTag::GENERAL_FREE);
	}
	
	//If this gameObject is considered broken and we have met the regen time, then level up the piece
	if (m_isBroken == true && m_regenTimer.hasMetTargetDuration()) {

		_levelUp();

	}

}


void VitalityComponent::inflictDamage(float damage)
{

	m_health -= damage;
	if (m_health <= 0) {

		if (m_isRegenerative == false) {
			m_isDestroyed = true;
		}
		else {
			m_isBroken = true;
			m_regenTimer = Timer(m_regenSpeed);
		}

	}

}

bool VitalityComponent::testResistance(float force)
{

	if (force >= m_resistance) {
		if (m_isRegenerative == false) {
			m_isDestroyed = true;
			parent()->setRemoveFromWorld(true);
		}
		else {

			m_isBroken = true;
			m_regenTimer = Timer(m_regenSpeed);
		}
		return false;
	}
	else {
		return true;
	}

}

void VitalityComponent::setLifetimeTimer(float lifetime)
{

	m_lifetimeTimer = Timer(lifetime);

}

void VitalityComponent::resetLifetime()
{

	m_lifetimeTimer.reset();

}
