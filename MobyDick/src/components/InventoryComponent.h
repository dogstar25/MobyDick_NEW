#ifndef INVENTORY_COMPONENT_H
#define INVENTORY_COMPONENT_H

#include <vector>
#include <assert.h>
#include <memory>

#include "Component.h"
#include "../GameObject.h"

enum class CollectibleTypes {

	COIN,

	CollectibleTypes_COUNT
};

class InventoryComponent : public Component
{
public:

	InventoryComponent();
	InventoryComponent(Json::Value componentJSON, std::string parentName, Scene* parentScene);
	~InventoryComponent();

	void setParent(GameObject* gameObject) override;

	bool addItem(std::shared_ptr<GameObject> gameObject, int slot);
	int addItem(std::shared_ptr<GameObject> gameObject);
	bool addItem(std::string gameObjectType);
	bool addItem(std::string gameObjectType, int slot);
	std::shared_ptr<GameObject> removeItem(int slot);
	std::shared_ptr<GameObject> removeItem(GameObject* gameObject);
	std::vector<std::optional<std::shared_ptr<GameObject>>> items() { return m_items; }
	int activeItem() {	return m_activeItem; }

	std::optional<std::shared_ptr<GameObject>> getItem(const int traitTag);
	int addCollectible(const CollectibleTypes, int count);
	
	const std::map<CollectibleTypes, int>& collectibles() { return m_collectibles; }
	void update();

	void refreshInventoryDisplay();
	void showInventory();
	void hideInventory();
	bool isOpen() { return m_isOpen; }
	std::optional<std::shared_ptr<GameObject>> getDisplayBackdropObject() { return m_displayBackdropObject; }

private:

	int m_activeItem{ 0 };
	bool m_isOpen{};
	std::vector<std::optional<std::shared_ptr<GameObject>>> m_items{};
	std::map<CollectibleTypes, int> m_collectibles{};
	std::string m_displayBackdropObjectType{};
	std::optional<std::shared_ptr<GameObject>> m_displayBackdropObject{};
	float m_maxCapacity{};

};

#endif
