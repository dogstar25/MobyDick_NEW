#include "WeaponComponent.h"

#include "../game.h"
#include "../ColorMap.h"

WeaponComponent::WeaponComponent()
{

}

WeaponComponent::WeaponComponent(Json::Value componentJSON)
{
	
	m_currentLevel = 1;

	m_fireOffset = componentJSON["fireOffset"].asFloat();

	for (Json::Value itrWeaponLevel : componentJSON["weaponLevels"])
	{
		int level = itrWeaponLevel["level"].asInt();

		WeaponLevelDetail weaponLevelDetail;

		weaponLevelDetail.level = itrWeaponLevel["level"].asInt();
		weaponLevelDetail.levelUpTarget = itrWeaponLevel["levelUpTarget"].asInt();
		weaponLevelDetail.force = itrWeaponLevel["force"].asFloat();
		if (itrWeaponLevel.isMember("color")) {
			weaponLevelDetail.color = ColorMap::instance().toSDLColor(itrWeaponLevel["color"].asString());
		}
		else {
			weaponLevelDetail.color = Colors::WHITE;
		}
		weaponLevelDetail.bulletPoolId = itrWeaponLevel["bulletPoolId"].asString();
		m_weaponLevelDetails.emplace(level, std::move(weaponLevelDetail));

	}


}

WeaponComponent::~WeaponComponent()
{

}

void WeaponComponent::update()
{

}

void WeaponComponent::fire(const b2Vec2& origin, const float& angle)
{
	std::string bulletPoolId =
		m_weaponLevelDetails.at(m_currentLevel).bulletPoolId;
	
	//Get a free bullet
	std::optional<std::shared_ptr<GameObject>> bullet = parent()->parentScene()->objectPoolManager().getPooledObject(bulletPoolId);

	if(bullet.has_value()){
		//Get references to the bullets components
		const auto& vitalityComponent = bullet.value()->getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);
		const auto& physicsComponent = bullet.value()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		const auto& renderComponent = bullet.value()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

		SDL_Color color = m_weaponLevelDetails.at(m_currentLevel).color;
		float force = m_weaponLevelDetails.at(m_currentLevel).force;

		//Calculate the origin of the bullet
		float dx = origin.x + cos(angle);
		float dy = origin.y + sin(angle);

		//Calculate offset values of bullet spawning origin adding an offset for the fireing object
		float xAdj = cos(angle) * (m_fireOffset);
		float yAdj = sin(angle) * (m_fireOffset);

		dx += xAdj;
		dy += yAdj;

		//Bullet Strength
		vitalityComponent->setAttackPower(force);

		b2Vec2 positionVector = b2Vec2(dx, dy);

		dx = cos(angle) * vitalityComponent->speed();
		dy = sin(angle) * vitalityComponent->speed();
		b2Vec2 velocityVector = b2Vec2(dx, dy);

		physicsComponent->setFixedRotation(true);
		physicsComponent->setTransform(positionVector, angle);
		physicsComponent->setLinearVelocity(velocityVector);
		physicsComponent->setBullet(true);

		renderComponent->setColor(color);

		//Add the bullet object to the main gameObject collection
		parent()->parentScene()->addGameObject(bullet.value(), LAYER_MAIN);
	}
	else {
		std::cout << "No Bullet available" << std::endl;
	}

}

void WeaponComponent::_levelUp()
{

	if (m_currentLevel < m_weaponLevelDetails.size()) {
		m_currentLevel++;
	}


}


