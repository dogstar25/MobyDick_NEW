#include "InterfaceComponent.h"


InterfaceComponent::InterfaceComponent(Json::Value componentJSON, Scene* parentScene)
{
	m_componentType = ComponentTypes::INTERFACE_COMPONENT;

	//Interactive object if exists
	if (componentJSON.isMember("menuObject")) {
		auto interfaceMenuObjectId = componentJSON["menuObject"].asString();
		m_interfaceMenuObject = std::make_shared<GameObject>(interfaceMenuObjectId, -5.f, -5.f, 0.f, parentScene);
	}

	m_interfaceMenuRequiresPointingAt = componentJSON["interfaceMenuRequiresPointingAt"].asBool();

	m_LocationHintDistance = componentJSON["locationHintDistance"].asFloat();

}

void InterfaceComponent::postInit()
{
	//The interfaceMenuObject Lives in the interfaceComponent, so we are responsible for its
	//creation and destruction and setting its layer
	if (m_interfaceMenuObject.has_value()) {
		m_interfaceMenuObject.value()->setLayer(parent()->layer());
	}

	//The remoteLocationObject
	GameObjectDefinition gameObjectDefinition = parent()->gameObjectDefinition();
	Json::Value componentDefinition = util::getComponentConfig(gameObjectDefinition.definitionJSON(), ComponentTypes::INTERFACE_COMPONENT);
	if (componentDefinition.isMember("remoteLocationObject")) {

		std::string name = componentDefinition["remoteLocationObject"].asString();
		m_remoteLocationObject = parent()->parentScene()->getFirstGameObjectByName(name);

	}


}

void InterfaceComponent::render()
{

	//Do not render any Interface items if the item is in a scene that is paused
	if (parent()->parentScene()->state() != SceneState::RUN) {

		return;
	}

	SDL_FPoint position{ parent()->getCenterPosition() };

	//If this an interactiveObject and a playerObject is touching it, then display its interactive menu, if one exists
	if (parent()->hasTrait(TraitTag::contact_interface)) {

		if (parent()->isTouchingByTrait(TraitTag::player)){

			GameObject* interactingObject = parent()->getFirstTouchingByTrait(TraitTag::player).value().lock().get();

			//Is the player is pointing at this interactive object?
			if (m_interfaceMenuRequiresPointingAt == false ||
				(m_interfaceMenuRequiresPointingAt == true && interactingObject->isPointingAt(parent()->getCenterPosition()))) {

				//If there is a menu then display the interaction menu and it will execute the selected action
				if (m_interfaceMenuObject) {

					if (m_remoteLocationObject) {
						position = m_remoteLocationObject.value().lock()->getCenterPosition();
					}
					else {
						position = determineInterfaceMenuLocation(interactingObject, parent(), m_interfaceMenuObject.value().get());
					}

					m_interfaceMenuObject.value()->setPosition(position);
					m_interfaceMenuObject.value()->render();

				}
			}
		}

	}
	else if (parent()->hasTrait(TraitTag::mouse_interface)) {

		//Is the mouse touching this area
		const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
		if (util::isMouseOverGameObject(renderComponent->getRenderDestRect())) {

			//If the player is within reach then show the interface 
			if (parent()->isTouchingByTrait(TraitTag::player)) {

				setCursor(parent(), true);

				if (m_remoteLocationObject) {
					position = m_remoteLocationObject.value().lock()->getCenterPosition();
				}
				else {
					const auto& player = parent()->parentScene()->getFirstGameObjectByTrait(TraitTag::player);
					position = determineInterfaceMenuLocation(player.value().get(), parent(), m_interfaceMenuObject.value().get());
				}

				m_interfaceMenuObject.value()->setPosition(position);
				m_interfaceMenuObject.value()->render();
			}

		}
		else if (m_LocationHintDistance.has_value() == true && _mouseWithinHintRange()) {

			setCursor(parent(), false);

		}
		else {

			setCursor(parent(), false);

		}

	}

}

bool InterfaceComponent::_mouseWithinHintRange()
{

	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Make a rectangle that is size of hint reveal area

	return false;
}

SDL_FPoint InterfaceComponent::determineInterfaceMenuLocation(GameObject* playerObject, GameObject* contactGameObject, GameObject* menuObject)
{

	SDL_FPoint position{};

	//If we have a designated Object like a HUD to display this menu to, then Use its position
	if (m_remoteLocationObject.has_value()) {

		position = m_remoteLocationObject.value().lock()->getCenterPosition();

	}
	else {

		//is the ineteracting object(player) on the left or right side of the interaction object
		float orientationAngle = atan2(playerObject->getCenterPosition().y - contactGameObject->getCenterPosition().y,
			playerObject->getCenterPosition().x - contactGameObject->getCenterPosition().x);
		float orientationAngleDegrees = util::radiansToDegrees(orientationAngle);

		bool rightside = false;
		if (abs(orientationAngleDegrees) < 90) {

			rightside = true;
		}

		float xPos{};
		float yPos{};
		if (rightside == false) {

			xPos = contactGameObject->getCenterPosition().x + (contactGameObject->getSize().x / 2) + (menuObject->getSize().x / 2);
			yPos = contactGameObject->getCenterPosition().y + (contactGameObject->getSize().y / 2);

		}
		else {
			xPos = contactGameObject->getCenterPosition().x - (contactGameObject->getSize().x / 2) - (menuObject->getSize().x / 2);
			yPos = contactGameObject->getCenterPosition().y + (contactGameObject->getSize().y / 2);

		}

		position = { xPos, yPos };

	}

	return position;


}

void InterfaceComponent::setParent(GameObject* gameObject)
{
	//Call base component setParent
	Component::setParent(gameObject);

	//Parent for this interactionMenuObject if it exists
	if (m_interfaceMenuObject) {
		m_interfaceMenuObject.value()->setParent(gameObject);

	}

}

