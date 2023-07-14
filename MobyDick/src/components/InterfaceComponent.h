#pragma once
#include "Component.h"
#include <optional>

#include "../GameObject.h"


struct InterfaceEvent {

	int eventId{};
	std::string label{};
	int actionId{};

	//virtual bool isAvailable() { return true; };

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
	void render();
	void postInit() override;
	void setParent(GameObject* gameObject) override;
	bool isAutoInteractOnPuzzleComplete() { return m_autoInteractOnPuzzleComplete; }
	bool isDragging() { return m_dragging; }
	bool isHovered() { return m_hovered; }
	SDL_FPoint dragOffset() { return m_dragOffset; }
	std::map<int, std::shared_ptr<InterfaceEvent>> events() { return m_events; }

	virtual void setCursor(GameObject*, bool) {};
	virtual bool isEventAvailable(int eventId) { return true; }

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
	bool m_dragging{};
	bool m_hovered{};
	SDL_FPoint m_dragOffset{};
	b2MouseJoint* m_b2MouseJoint{};

	virtual void handleDragging();
	bool hasEvent(int eventId);
	
	std::map<int, std::shared_ptr<InterfaceEvent>> m_events{};


private:
	bool _mouseWithinHintRange();
	
	void _initializeDragging(SDL_FPoint mouseWorldPosition);
	void _clearDragging();

	


};

