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

	for (Json::Value actionJSON : componentJSON["interfaceActions"])
	{

		InterfaceAction action;
		action.actionId = game->enumMap()->toEnum(actionJSON["actionId"].asString());
		action.label = actionJSON["label"].asString();
		action.conditionOperator = (ConditionOperator)game->enumMap()->toEnum(actionJSON["condition"]["operator"].asString());

		for (Json::Value actionDetailsJSON : actionJSON["condition"]["events"]) {

			auto eventValue = (int)game->enumMap()->toEnum(actionDetailsJSON.asString());
			action.conditionEvents.push_back((InterfaceEvents)eventValue);

		}

		m_eventActions[action.actionId] = std::make_shared<InterfaceAction>(action);

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

}

void InterfaceComponent::update()
{

	std::bitset<(int)InterfaceEvents::COUNT> newEventsState{};
	bool showInterfaceMenu{};

	//convenience reference to outside component(s)
	const auto& actionComponent = parent()->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);
	const auto& transformComponent = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//Mouse Contact - ON_HOVER
	if (util::isMouseOverGameObject(renderComponent->getRenderDestRect())) {
		newEventsState.set((int)InterfaceEvents::ON_HOVER, true);
	}
	else {
		if (m_currentEventsState.test((int)InterfaceEvents::ON_HOVER) == true) {
			newEventsState.set((int)InterfaceEvents::ON_HOVER_OUT, true);
		}
	}

	//Player Object Contact
	if (parent()->isTouchingByTrait(TraitTag::player)) {
		newEventsState.set((int)InterfaceEvents::ON_TOUCHING, true);
	}
	else {
		newEventsState.set((int)InterfaceEvents::ON_NO_TOUCHING, true);
	}

	//Always Initilize to being hidden - THIS IS NEW
	if (actionComponent->hasAction(Actions::HIDE_INTERFACE)) {
		const auto& action = actionComponent->getAction(Actions::HIDE_INTERFACE);
			action->perform(parent());
	}

	//Handle dragging an object
	if (m_dragging == true) {

		handleDragging();
	}

	//Should we allow the interface?
	if (shouldInterfaceBeActivated(newEventsState)) {

		for (const auto& actionEvent : m_eventActions) {

			bool actionMetConditions = hasActionMetEventRequirements(actionEvent.second.get(), newEventsState);
			if (actionMetConditions) {

				//Cecks for any puzzles that may be connected to this action
				if (isEventAvailable(actionEvent.second->actionId)) {

					const auto& action = actionComponent->getAction(actionEvent.second->actionId);
					action->perform(parent());
				}
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

				//Has mouse contact then start drag if its draggable
				//otherwise execture ON_CLICK action if one exists
				if (newEventsState.test((int)InterfaceEvents::ON_HOVER)) {

					//If this object is draggable then deal with it
					if (parent()->hasTrait(TraitTag::draggable)) {
						_initializeDragging(mouseWorldPosition);
					}

					newEventsState.set((int)InterfaceEvents::ON_CLICK, true);
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
				newEventsState.set((int)InterfaceEvents::ON_KEY_DOWN, true);

				//we need to specify the different key events to store here. 

				break;

			}

			default:
			{
				break;
			}
		}

		//Loop through every possible action and see if the mouse and player touching state matches 
		//what is required to execute
		if (shouldInterfaceBeActivated(newEventsState)) {

			for (const auto& actionEvent : m_eventActions) {

				//Check mouse and touching state
				bool actionMetConditions = hasActionMetEventRequirements(actionEvent.second.get(), newEventsState);
				if (actionMetConditions) {

					//If there is an unsolved puzzle attached
					if (isEventAvailable(actionEvent.second->actionId)) {

						const auto& action = actionComponent->getAction(actionEvent.second->actionId);
						action->perform(parent());

					}
				}
			}
		}

		//Set The cursor
		setCursor(parent(), newEventsState);


	}

	//Save the current state
	m_currentEventsState = newEventsState;

}


bool InterfaceComponent::hasActionMetEventRequirements(InterfaceAction* action, std::bitset<(int)InterfaceEvents::COUNT> currentEventsState)
{

	bool hasMetEventConditions{};

	//We need a special check here for Showing the interface Menu action
	if (action->actionId == Actions::SHOW_INTERFACE && shouldInterfaceMenuBeShown(currentEventsState) == false) {

		return false;
	}

	//If our operator is AND, then all conditions in the conditionsEvent that are
	//true have to match the current state of those conditions
	if (action->conditionOperator == ConditionOperator::AND) {

		hasMetEventConditions = true;

		for (const auto& eventIndex : action->conditionEvents) {

			if (currentEventsState[(int)eventIndex] != true) {

				hasMetEventConditions = false;
				break;
			}
		}

	}

	//If our operator is OR, then only one of the conditions in the conditionsEvent that are
	//true have to match the current state of those conditions
	else {

		hasMetEventConditions = false;

		for (const auto& eventIndex : action->conditionEvents) {

			if (currentEventsState[(int)eventIndex] == true) {

				hasMetEventConditions = true;
				break;
			}
		}
	}

	return hasMetEventConditions;
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

		//If there was a overlay added to the dragging somewhere then make sure its clear
		const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
		renderComponent->removeDisplayOverlay();

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

