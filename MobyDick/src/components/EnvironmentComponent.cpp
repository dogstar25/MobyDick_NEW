#include "EnvironmentComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

EnvironmentComponent::EnvironmentComponent(Json::Value componentJSON) :
	Component(ComponentTypes::ENVIRONMENT_COMPONENT)
{


}

void EnvironmentComponent::postInit()
{




}


void EnvironmentComponent::update()
{




}

void EnvironmentComponent::setParent(GameObject* gameObject)
{
	//Call base component setParent
	Component::setParent(gameObject);

}


