#pragma once
#include "Component.h"
#include <optional>

#include "../GameObject.h"

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

	virtual void update() {};
	void render();
	void postInit() override;

	void setParent(GameObject* gameObject) override;
	virtual void setCursor(GameObject*, bool) {};

	bool isAutoInteractOnPuzzleComplete() { return m_autoInteractOnPuzzleComplete; }


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

private:
	bool _mouseWithinHintRange();


};

