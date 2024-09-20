#include "InterfaceComponent.h"
#include "../SceneManager.h"
#include "../game.h"

extern std::unique_ptr<Game> game;

InterfaceComponent::InterfaceComponent(Json::Value componentJSON, GameObject* parent, Scene* parentScene) :
	Component(ComponentTypes::INTERFACE_COMPONENT, parent)
{

	//Interactive object if exists
	if (componentJSON.isMember("menuObject")) {
		auto interfaceMenuObjectId = componentJSON["menuObject"].asString();
		m_interfaceMenuObject = parentScene->createGameObject(interfaceMenuObjectId, parent, -5.f, -5.f, 0.f, parentScene);

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


}

void InterfaceComponent::clearSpecificGameObjectInterface(GameObject* gameObject)
{

	if (m_currentGameObjectInterfaceActive.has_value() && m_currentGameObjectInterfaceActive.value() == gameObject) {

		m_currentGameObjectInterfaceActive = std::nullopt;
	}


}


void InterfaceComponent::update()
{

	////Make sure the current interface object is still valid
	//if (m_currentGameObjectInterfaceActive.has_value()) {

	//	const auto& gameObject = parent()->parentScene()->getGameObject(m_currentGameObjectInterfaceActive.value()->id());

	//}


	std::bitset<MAX_EVENT_STATES> newEventsState{};
	bool showInterfaceMenu{};

	//Need to carry over the ON_DRAG event sense it only starts on a mouse click and end on mouse release
	newEventsState.set( (int)InterfaceEvents::ON_DRAG, m_currentEventsState.test((int)InterfaceEvents::ON_DRAG));

	//convenience reference to outside component(s)
	const auto& actionComponent = parent()->getComponent<ActionComponent>(ComponentTypes::ACTION_COMPONENT);
	const auto& transformComponent = parent()->getComponent<TransformComponent>(ComponentTypes::TRANSFORM_COMPONENT);
	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);

	//This object may have been deleted so clear it out if its lookup table reference is expired
	if (m_currentGameObjectInterfaceActive) {

		const auto& temp = parent()->parentScene()->getGameObject(m_currentGameObjectInterfaceActive.value()->id());
		if (temp.has_value() == false) {

			m_currentGameObjectInterfaceActive = std::nullopt;
		}
	}


	if (util::isMouseOverGameObject(renderComponent->getRenderDestRect())) {

		newEventsState.set((int)InterfaceEvents::ON_HOVER, true);
	}
	else {

		if (m_currentEventsState.test((int)InterfaceEvents::ON_HOVER) == true) {

			newEventsState.set((int)InterfaceEvents::ON_HOVER_OUT, true);
		}
	}

	//Player Object Contact
	
	if (parent()->isTouchingByTrait(TraitTag::player) ) {

		const auto& playerObject = parent()->parentScene()->player();

		if (util::hasLineOfSight(playerObject.get(), parent())) {
			newEventsState.set((int)InterfaceEvents::ON_TOUCHING, true);
		}

	}
	else {

		if (m_currentEventsState.test((int)InterfaceEvents::ON_TOUCHING) == true) {
			newEventsState.set((int)InterfaceEvents::ON_STOP_TOUCHING, true);
		}
	}

	//Handle dragging an object
	if (m_currentEventsState.test((int)InterfaceEvents::ON_DRAG) == true) {
		handleDragging();
	}

	if (doesInterfaceHavePriority(newEventsState)) {

		for (const auto& actionEvent : m_eventActions) {

			bool actionMetConditions = hasActionMetEventRequirements(actionEvent.second.get(), newEventsState);
			if (actionMetConditions) {

				//Checks for any puzzles that may be connected to this action
				if (isEventAvailable(actionEvent.second->actionId)) {

					//If this is the ShowInterface action then make it the new active interface
					if (actionEvent.second->actionId == Actions::SHOW_INTERFACE) {

						m_currentGameObjectInterfaceActive = parent();
					}

					//If this is the HideInterface action and this is the current active interface, then clear
					//out the current active interface
					if (actionEvent.second->actionId == Actions::HIDE_INTERFACE &&
						m_currentGameObjectInterfaceActive == parent()) {

						m_currentGameObjectInterfaceActive = std::nullopt;
					}

					//Only execute actions that are NOT tied to a user input like mouse click or keydown
					if (isUserInputTiedAction(actionEvent.second->actionId) == false) {
						const auto& action = actionComponent->getAction(actionEvent.second->actionId);
						action->perform();
					}
				}
			}
		}
	}

	// Handle Mouse Clicks and Key Presses that are not part of the Player Movement Control or Scene Control
	for (auto inputEvent = SceneManager::instance().playerInputEvents().begin(); inputEvent != SceneManager::instance().playerInputEvents().end();)
	{

		switch (inputEvent->event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
			{
				SDL_Point mouseLocation{};
				auto buttonState = SDL_GetMouseState(&mouseLocation.x, &mouseLocation.y);
				SDL_FPoint mouseWorldPosition = util::screenToWorldPosition({ (float)mouseLocation.x, (float)mouseLocation.y });

				//Has mouse contact then start drag if its draggable
				//otherwise execture ON_LCLICK action if one exists
				if (newEventsState.test((int)InterfaceEvents::ON_HOVER)) {


					//If this object is draggable and we have a left click then deal with it
					if (parent()->isDragging() == false && parent()->hasState(GameObjectState::DRAGGABLE) && (buttonState & SDL_BUTTON(SDL_BUTTON_LEFT))
						&& isDraggingAllowed()) {

						newEventsState.set((int)InterfaceEvents::ON_DRAG, true);
						_initializeDragging(mouseWorldPosition);
					}

					//Set click state
					else if (buttonState & SDL_BUTTON(SDL_BUTTON_LEFT)) {

						newEventsState.set((int)InterfaceEvents::ON_LCLICK, true);

					} else if (buttonState & SDL_BUTTON(SDL_BUTTON_RIGHT)) {

						newEventsState.set((int)InterfaceEvents::ON_RCLICK, true);
					}
				}

				break;
			}
			case SDL_MOUSEBUTTONUP:
			{
				if (m_currentEventsState.test((int)InterfaceEvents::ON_DRAG)) {

					newEventsState.set((int)InterfaceEvents::ON_DROP, true);
					newEventsState.set((int)InterfaceEvents::ON_DRAG, false);

					clearDragging();

				}
				break;
			}
			case SDL_KEYDOWN:
			{
				SDL_Scancode keyScanCode = SDL_GetScancodeFromKey(inputEvent->event.key.keysym.sym);
				if (keyScanCode < MAX_EVENT_STATES) {
					newEventsState.set((int)keyScanCode, true);
				}

				break;

			}

			default:
			{
				break;
			}
		}

		//Loop through every possible action and see if the mouse and player touching state matches 
		//what is required to execute
		bool found{};
		if (doesInterfaceHavePriority(newEventsState)) {

			for (const auto& actionEvent : m_eventActions) {

				//Check mouse and touching state
				bool actionMetConditions = hasActionMetEventRequirements(actionEvent.second.get(), newEventsState);
				if (actionMetConditions) {

					//If there is an unsolved puzzle attached
					if (isEventAvailable(actionEvent.second->actionId)) {

						//Only execute actions that ARE tied to a user input like mouse click or keydown
						if (isUserInputTiedAction(actionEvent.second->actionId) == true) {

							const auto& action = actionComponent->getAction(actionEvent.second->actionId);
							action->perform();
							inputEvent = SceneManager::instance().playerInputEvents().erase(inputEvent);
							found = true;
							break;
						}

					}
				}
			}

		}

		if (found == false) {

			++inputEvent;

		}

	}

	//Save the current state
	m_currentEventsState = newEventsState;

}


bool InterfaceComponent::hasActionMetEventRequirements(InterfaceAction* action, std::bitset<MAX_EVENT_STATES> currentEventsState)
{

	bool hasMetEventConditions{};

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

bool InterfaceComponent::isUserInputTiedAction(int actionId)
{

	if (m_eventActions.find(actionId) != m_eventActions.end()) {

		for (auto event : m_eventActions.find(actionId)->second->conditionEvents) {

			if (event == InterfaceEvents::ON_LCLICK || 
				event == InterfaceEvents::ON_RCLICK ||
				event == InterfaceEvents::ON_DROP ||
				((int)event > 0 && (int)event < 100)) {

				return true;
			}

		}

	}

	return false;

}


bool InterfaceComponent::doesInterfaceHavePriority(std::bitset<MAX_EVENT_STATES>)
{

	return true;
}



void InterfaceComponent::render()
{

	//if (m_interfaceMenuObject) {
	//	m_interfaceMenuObject.value()->render();
	//}

	////new
	if (m_currentGameObjectInterfaceActive.has_value() && 
		m_currentGameObjectInterfaceActive.value() == this->parent()) {

		if (m_interfaceMenuObject.has_value()) {
			m_interfaceMenuObject.value()->render();
		}
	}

}

void InterfaceComponent::clearDragging()
{
	SDL_ShowCursor(SDL_TRUE);

	//Pass in a weak_ptr to clear out the draggingObject optional
	parent()->parentScene()->setDraggingObject(std::weak_ptr<GameObject>());

	//If there was a overlay added to the dragging somewhere then make sure its clear
	const auto& renderComponent = parent()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
	renderComponent->removeDisplayOverlay();

	m_currentEventsState.set((int)InterfaceEvents::ON_DRAG, false);

	if (parent()->hasComponent(ComponentTypes::PHYSICS_COMPONENT) == true && m_b2MouseJoint != nullptr) {

		parent()->parentScene()->physicsWorld()->DestroyJoint(m_b2MouseJoint);
		m_b2MouseJoint = nullptr;
	}

}

bool InterfaceComponent::hasEvent(int eventId)
{
	if (m_eventActions.find(eventId) != m_eventActions.end()) {
		return true;
	}

	return false;
}

void InterfaceComponent::_initializeDragging(SDL_FPoint mouseWorldPosition)
{

	SDL_ShowCursor(SDL_FALSE);

	//Get the shared pointer for this object and set it as the dragging object for the scene to 
	//apply special treatment to dragging object to render always on top
	const auto& gameObjectShrPtr = parent()->parentScene()->getGameObject(parent()->id());
	if (gameObjectShrPtr.has_value()) {
		parent()->parentScene()->setDraggingObject(gameObjectShrPtr.value());
	}

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

	if (parent()->hasState(GameObjectState::DRAGGABLE)) {

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

		//Items can change displaysize in certain situations. WHen dragging we want the size to be the original size
		parent()->revertToOriginalSize();

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

bool InterfaceComponent::isDragging()
{

	if (m_currentEventsState.test((int)InterfaceEvents::ON_DRAG)) {

		return true;
	}

	return false;
}

bool InterfaceComponent::isHovering()
{

	if (m_currentEventsState.test((int)InterfaceEvents::ON_HOVER)) {

		return true;
	}

	return false;
}
