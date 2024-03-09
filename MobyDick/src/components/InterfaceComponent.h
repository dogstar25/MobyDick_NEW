#pragma once
#include "Component.h"
#include <optional>

#include "../GameObject.h"


struct InterfaceAction {

	int actionId{};
	std::string label{};
	std::vector<InterfaceEvents> conditionEvents{};
	ConditionOperator conditionOperator{};

};

class InterfaceComponent : public Component
{
public:
	InterfaceComponent(Json::Value definitionJSON, Scene* parentScene);
	~InterfaceComponent() = default;

	std::optional<std::shared_ptr<GameObject>> interfaceMenuObject() {
		return m_interfaceMenuObject;
	}
	std::optional<std::weak_ptr<GameObject>> remoteLocationObject() {
		return m_remoteLocationObject;
	}
	SDL_FPoint determineInterfaceMenuLocation(GameObject* playerObject, GameObject* contactGameObject, GameObject* menuObject);

	virtual void update() override;
	virtual void render() override;
	virtual void postInit() override;
	virtual void setParent(GameObject* gameObject) override;

	bool isAutoInteractOnPuzzleComplete() { return m_autoInteractOnPuzzleComplete; }
	SDL_FPoint dragOffset() { return m_dragOffset; }
	std::map<int, std::shared_ptr<InterfaceAction>> eventActions() { return m_eventActions; }

	virtual bool isEventAvailable(int eventId) { return true; }

	bool isDragging();
	bool isHovering();
	void setCurrentGameObjectInterfaceActive(GameObject* gameObject) { m_currentGameObjectInterfaceActive = gameObject; }
	void clearSpecificGameObjectInterface(GameObject* gameObject);
	void clearDragging();

protected:

	std::optional<std::shared_ptr<GameObject>> m_interfaceMenuObject{};
	bool m_interfaceMenuRequiresPointingAt{};

	//The sharedGlobalLocationObject is not required, but if you want the objects interactive menu
	//to display somewhere else other than around the object itself, then specify that specific
	//Object here by name and the interactive menu will display on its location. Think HUD, and all
	//objects' interactive menus appear in same HUD spot
	std::optional<std::weak_ptr<GameObject>> m_remoteLocationObject{};
	bool m_autoInteractOnPuzzleComplete{};
	std::optional<float> m_LocationHintDistance{};
	SDL_FPoint m_dragOffset{};
	b2MouseJoint* m_b2MouseJoint{};
	std::bitset<MAX_EVENT_STATES> m_currentEventsState{};

	
	bool hasActionMetEventRequirements(InterfaceAction* action, std::bitset<MAX_EVENT_STATES> currentEventsState);
	
	std::map<int, std::shared_ptr<InterfaceAction>> m_eventActions{};

	virtual void handleDragging();
	virtual bool doesInterfaceHavePriority(std::bitset<MAX_EVENT_STATES>);
	virtual bool isUserInputTiedAction(int actionId);
	virtual bool isDraggingAllowed() { return true; }
	

	////////Static Variables

	static inline std::optional<GameObject*> m_currentGameObjectInterfaceActive{};

	/////////////////////


private:
	bool _mouseWithinHintRange();
	
	void _initializeDragging(SDL_FPoint mouseWorldPosition);
	

	


};

