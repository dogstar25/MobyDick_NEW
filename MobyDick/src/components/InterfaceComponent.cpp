#include "InterfaceComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

InterfaceComponent::InterfaceComponent(Json::Value componentJSON, Scene* parentScene)
{
	m_componentType = ComponentTypes::INTERFACE_COMPONENT;

	//Interactive object if exists
	if (componentJSON.isMember("menuObject")) {
		auto interfaceMenuObjectId = componentJSON["menuObject"].asString();
		m_interfaceMenuObject = std::make_shared<GameObject>(interfaceMenuObjectId, -5.f, -5.f, 0.f, parentScene);
		m_interfaceMenuObject->get()->disableRender();
	}

	for (Json::Value itrAction : componentJSON["interfaceEvents"])
	{

		InterfaceEvent event;
		event.eventId = game->enumMap()->toEnum(itrAction["eventId"].asString());
		event.label = itrAction["label"].asString();
		event.actionId = game->enumMap()->toEnum(itrAction["actionId"].asString());

		m_events[event.eventId] = std::make_shared< InterfaceEvent>(event);

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
	//GameObjectDefinition gameObjectDefinition = parent()->gameObjectDefinition();
	//Json::Value componentDefinition = util::getComponentConfig(gameObjectDefinition.definitionJSON(), ComponentTypes::INTERFACE_COMPONENT);
	//if (componentDefinition.isMember("remoteLocationObject")) {

	//	std::string name = componentDefinition["remoteLocationObject"].asString();
	//	m_remoteLocationObject = parent()->parentScene()->getFirstGameObjectByName(name);

	//}

}

bool InterfaceComponent::hasEvent(int eventId)
{

	if (m_events.find(eventId) != m_events.end()) {

		const auto& event = m_events.find(eventId)->second;
		if (isEventAvailable(event->eventId)) {
			return true;
		}
		else {
			return false;
		}
		
	}

	return false;

}


void InterfaceComponent::update()
{
	bool hasMouseContact{};
	bool hasPlayerContact{};
	bool hasPlayerAndMouseContact{};

	//convenience reference to outside component(s)
	const auto& actionComponent = parent()->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);
	const auto& transformComponent = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Mouse Contact
	hasMouseContact = util::isMouseOverGameObject(renderComponent->getRenderDestRect());

	//Player Object Contact
	hasPlayerContact = parent()->isTouchingByTrait(TraitTag::player);

	//Player Object Contact AND Mouse Contact
	if (hasMouseContact && hasPlayerContact) {
		hasPlayerAndMouseContact = true;
	}

	//Handle dragging an object
	if (m_dragging == true) {

		handleDragging();
	}
	else 
	{

		//Handle Mouse OnHover
		if (hasMouseContact) {

			m_hovered = true;

			if (hasEvent(InterfaceEvents::ON_HOVER)) {
				const auto& event = m_events[InterfaceEvents::ON_HOVER];
				const auto& action = actionComponent->getAction(event->actionId);
				action->perform(parent());

			}
		}
		//Handle Mouse Hover Out
		else {

			if (m_hovered == true) {
				if (hasEvent(InterfaceEvents::ON_HOVER_OUT)) {
					const auto& event = m_events[InterfaceEvents::ON_HOVER_OUT];
					const auto& action = actionComponent->getAction(event->actionId);
					action->perform(parent());
				}

			}
		}

		//Handle Player Touching
		if (hasPlayerContact) {

			if (hasEvent(InterfaceEvents::ON_TOUCHING)) {
				const auto& event = m_events[InterfaceEvents::ON_TOUCHING];
				const auto& action = actionComponent->getAction(event->actionId);
				action->perform(parent());

			}

		}

		//Player touching AND mouse hover
		if (hasPlayerAndMouseContact) {
			if (hasEvent(InterfaceEvents::ON_HOVER_AND_TOUCHING)) {
				const auto& event = m_events[InterfaceEvents::ON_HOVER_AND_TOUCHING];
				const auto& action = actionComponent->getAction(event->actionId);
				action->perform(parent());

			}
		}
		else {

			if (hasEvent(InterfaceEvents::ON_NO_HOVER_AND_NO_TOUCHING)) {
				const auto& event = m_events[InterfaceEvents::ON_NO_HOVER_AND_NO_TOUCHING];
				const auto& action = actionComponent->getAction(event->actionId);
				action->perform(parent());

			}

		}



	}

	// Handle Mouse Clicks and Key Presses that are not part of the Player Movement Control or Scene Control
	for (auto& inputEvent : SceneManager::instance().playerInputEvents())
	{
		switch (inputEvent.event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_Point mouseLocation{};
				SDL_GetMouseState(&mouseLocation.x, &mouseLocation.y);
				SDL_FPoint mouseWorldPosition = util::screenToWorldPosition({ (float)mouseLocation.x, (float)mouseLocation.y });

				if (hasMouseContact) {

					//If this object is draggable then deal with it
					if (parent()->hasTrait(TraitTag::draggable)) {
						_initializeDragging(mouseWorldPosition);
					}

					//Handle On click if it exists
					if (hasEvent(InterfaceEvents::ON_CLICK)) {
						const auto& event = m_events[InterfaceEvents::ON_CLICK];
						const auto& action = actionComponent->getAction(event->actionId);
						action->perform(parent());
					}
				}

				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				_clearDragging();
				break;
			}
			case SDL_KEYDOWN:
			{
				SDL_Scancode keyScanCode = SDL_GetScancodeFromKey(inputEvent.event.key.keysym.sym);
				//Handle On Key down if it exists
				if (hasEvent(InterfaceEvents::ON_KEY_DOWN)) {
					const auto& event = m_events[InterfaceEvents::ON_KEY_DOWN];
					const auto& action = actionComponent->getAction(event->actionId);
					action->perform();
				}

				break;

			}
			case SDL_KEYUP:
			{
				SDL_Scancode keyScanCode = SDL_GetScancodeFromKey(inputEvent.event.key.keysym.sym);
				//Handle On Key down if it exists
				if (hasEvent(InterfaceEvents::ON_KEY_UP)) {
					const auto& event = m_events[InterfaceEvents::ON_KEY_UP];
					const auto& action = actionComponent->getAction(event->actionId);
					action->perform();
				}

				break;
			}

			default:
			{
				break;
			}
		}

	}

}


void InterfaceComponent::render()
{

	if (m_interfaceMenuObject) {
		m_interfaceMenuObject.value()->render();
	}

















	//SDL_FPoint position{ parent()->getCenterPosition() };

	////If this an interactiveObject and a playerObject is touching it, then display its interactive menu, if one exists
	//if (parent()->hasTrait(TraitTag::contact_interface)) {

	//	if (parent()->isTouchingByTrait(TraitTag::player)){

	//		GameObject* interactingObject = parent()->getFirstTouchingByTrait(TraitTag::player).value().lock().get();

	//		//Is the player is pointing at this interactive object?
	//		if (m_interfaceMenuRequiresPointingAt == false ||
	//			(m_interfaceMenuRequiresPointingAt == true && interactingObject->isPointingAt(parent()->getCenterPosition()))) {

	//			//If there is a menu then display the interaction menu and it will execute the selected action
	//			if (m_interfaceMenuObject) {

	//				if (m_remoteLocationObject) {
	//					position = m_remoteLocationObject.value().lock()->getCenterPosition();
	//				}
	//				else {
	//					position = determineInterfaceMenuLocation(interactingObject, parent(), m_interfaceMenuObject.value().get());
	//				}

	//				m_interfaceMenuObject.value()->setPosition(position);
	//				m_interfaceMenuObject.value()->render();

	//			}
	//		}
	//	}

	//}
	//else if (parent()->hasTrait(TraitTag::mouse_interface)) {

	//	//Is the mouse touching this area
	//	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	//	if (util::isMouseOverGameObject(renderComponent->getRenderDestRect())) {

	//		//If the player is within reach then show the interface 
	//		if (parent()->isTouchingByTrait(TraitTag::player)) {

	//			setCursor(parent(), true);

	//			if (m_remoteLocationObject) {
	//				position = m_remoteLocationObject.value().lock()->getCenterPosition();
	//			}
	//			else {
	//				const auto& player = parent()->parentScene()->getFirstGameObjectByTrait(TraitTag::player);
	//				position = determineInterfaceMenuLocation(player.value().get(), parent(), m_interfaceMenuObject.value().get());
	//			}

	//			m_interfaceMenuObject.value()->setPosition(position);
	//			m_interfaceMenuObject.value()->render();
	//		}

	//	}
	//	else if (m_LocationHintDistance.has_value() == true && _mouseWithinHintRange()) {

	//		setCursor(parent(), false);

	//	}
	//	else {

	//		setCursor(parent(), false);

	//	}

	//}

}

void InterfaceComponent::_clearDragging()
{
	SDL_ShowCursor(SDL_TRUE);

	if (m_dragging == true) {

		m_dragging = false;
		if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true) {

			parent()->parentScene()->physicsWorld()->DestroyJoint(m_b2MouseJoint);
			m_b2MouseJoint = nullptr;
		}
	}

}

void InterfaceComponent::_initializeDragging(SDL_FPoint mouseWorldPosition)
{

	m_dragging = true;

	SDL_ShowCursor(SDL_FALSE);

	if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == false) {

		m_dragOffset = { parent()->getCenterPosition().x - mouseWorldPosition.x,
						parent()->getCenterPosition().y - mouseWorldPosition.y };
	}
	else {

		m_dragOffset = { parent()->getCenterPosition().x - mouseWorldPosition.x,
				parent()->getCenterPosition().y - mouseWorldPosition.y };

		//Build a mouse joint for physics dragging
		const auto& physicsComponent = parent()->getComponent<PhysicsComponent>(ComponentTypes::PHYSICS_COMPONENT);
		m_b2MouseJoint = physicsComponent->createB2MouseJoint();

	}



}
void InterfaceComponent::handleDragging()
{

	if (parent()->hasTrait(TraitTag::draggable)) {

		if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == false) {

			SDL_Point mouseLocation{};
			SDL_GetMouseState(&mouseLocation.x, &mouseLocation.y);

			SDL_FPoint newPosition{};
			SDL_FPoint mouseWorldPosition = util::screenToWorldPosition({ (float)mouseLocation.x, (float)mouseLocation.y });

			newPosition.x = mouseWorldPosition.x + m_dragOffset.x;
			newPosition.y = mouseWorldPosition.y + m_dragOffset.y;

			parent()->setPosition(newPosition.x, newPosition.y);

		}
		else {

			SDL_Point mouseLocation{};
			SDL_GetMouseState(&mouseLocation.x, &mouseLocation.y);

			SDL_FPoint newPosition{};
			SDL_FPoint mouseWorldPosition = util::screenToWorldPosition({ (float)mouseLocation.x, (float)mouseLocation.y });

			mouseWorldPosition = util::toBox2dPoint(mouseWorldPosition);

			m_b2MouseJoint->SetTarget({ mouseWorldPosition.x, mouseWorldPosition.y });

			//Right side the object
			parent()->setAngleInDegrees(0);

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

