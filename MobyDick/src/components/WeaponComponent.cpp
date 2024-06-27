#include "WeaponComponent.h"

#include "../game.h"
#include "../ColorMap.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

//WeaponComponent::WeaponComponent() :
//	Component(ComponentTypes::WEAPON_COMPONENT)
//{
//
//}

WeaponComponent::WeaponComponent(Json::Value componentJSON, GameObject* parent) :
	Component(ComponentTypes::WEAPON_COMPONENT, parent)
{
	
	m_componentType = ComponentTypes::WEAPON_COMPONENT;

	m_currentLevel = 1;

	m_fireOffset = componentJSON["fireOffset"].asFloat();
	m_levelUpIncrement = componentJSON["levelUpIncrement"].asFloat();

	//We either have 1 set of weapon parameters or multiple levels
	if (componentJSON.isMember("weaponLevels")) {
		for (Json::Value itrWeaponLevel : componentJSON["weaponLevels"])
		{
			int level = itrWeaponLevel["level"].asInt();
			WeaponLevelDetail weaponLevelDetail;

			weaponLevelDetail.level = itrWeaponLevel["level"].asInt();
			weaponLevelDetail.force = itrWeaponLevel["force"].asFloat();
			if (itrWeaponLevel.isMember("color")) {
				weaponLevelDetail.color = game->colorMap()->toSDLColor(itrWeaponLevel["color"].asString());
			}
			weaponLevelDetail.bulletPoolId = itrWeaponLevel["bulletPoolId"].asString();
			m_weaponLevelDetails.emplace(level, std::move(weaponLevelDetail));

		}
	}
	else {
		WeaponLevelDetail weaponLevelDetail;
		weaponLevelDetail.force = componentJSON["force"].asFloat();
		if (componentJSON.isMember("color")) {
			weaponLevelDetail.color = game->colorMap()->toSDLColor(componentJSON["color"].asString());
		}
		weaponLevelDetail.bulletPoolId = componentJSON["bulletPoolId"].asString();
		m_weaponLevelDetails.emplace(1, std::move(weaponLevelDetail));

	}

}

WeaponComponent::~WeaponComponent()
{

}

void WeaponComponent::update()
{

}

void WeaponComponent::fire(const b2Vec2& origin, const float& angle, std::string bulletPoolId, float force, std::optional<SDL_Color> color)
{

	//Get a free bullet
	std::optional<std::shared_ptr<GameObject>> bullet = parent()->parentScene()->objectPoolManager().getPooledObject(bulletPoolId);

	if (bullet.has_value()) {
		//Get references to the bullets components
		const auto& vitalityComponent = bullet.value()->getComponent<VitalityComponent>(ComponentTypes::VITALITY_COMPONENT);
		const auto& physicsComponent = bullet.value()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		const auto& renderComponent = bullet.value()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

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

		//Only set the bullets color if one was defined in the weapons config
		//otherwise leave bullet whatever color it is
		if (color.has_value()) {
			renderComponent->setColor(m_weaponLevelDetails.at(m_currentLevel).color.value());
		}

		//Add the bullet object to the main gameObject collection
		parent()->parentScene()->addGameObjectFromPool(bullet.value(), GameLayer::MAIN);
	}
	else {
		std::cout << "No Bullet available" << std::endl;
	}

}

void WeaponComponent::fire(const b2Vec2& origin, const float& angle)
{
	float force = m_weaponLevelDetails.at(m_currentLevel).force;
	std::string bulletPoolId = m_weaponLevelDetails.at(m_currentLevel).bulletPoolId;
	
	std::optional<SDL_Color> color = m_weaponLevelDetails.at(m_currentLevel).color;

	fire(origin, angle, bulletPoolId, force, color);

}

std::string WeaponComponent::getBulletPoolId()
{
	assert(m_weaponLevelDetails.find(m_currentLevel) != m_weaponLevelDetails.end() && "There is no bulletId at the set level!");

	return m_weaponLevelDetails.at(m_currentLevel).bulletPoolId;
}

void WeaponComponent::_levelUp()
{

	if (m_currentLevel < m_weaponLevelDetails.size()) {
		m_currentLevel++;
	}


}

void WeaponComponent::setColor(SDL_Color color)
{
	//This only works for weapons with 1 level

	m_weaponLevelDetails.at(1).color = color;

}

void WeaponComponent::setForce(float force)
{
	//This only works for weapons with 1 level

	m_weaponLevelDetails.at(1).force = force;

}


