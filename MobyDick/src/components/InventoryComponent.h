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
	void postInit() override;

	bool addItem(std::shared_ptr<GameObject> gameObject, int slot);
	bool addItem(std::shared_ptr<GameObject> gameObject);
	bool addItem(std::string gameObjectType);
	bool addItem(std::string gameObjectType, int slot);
	bool hasItem(int slot);
	std::optional<int> getSlot(GameObject* gameObject);
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
	void setOpen(bool open) { m_isOpen = open; }
	std::optional<std::weak_ptr<GameObject>> getDisplayObject() { return m_displayObject; }

private:

	int m_activeItem{ 0 };
	bool m_isOpen{};
	bool m_isAlwaysOpen{};
	std::vector<std::optional<std::shared_ptr<GameObject>>> m_items{};
	std::map<CollectibleTypes, int> m_collectibles{};
	std::optional<std::weak_ptr<GameObject>> m_displayObject{};
	GameLayer m_displayLayer{};
	float m_maxCapacity{};

	void _setItemSize(std::shared_ptr<GameObject> gameObject);

};

#endif
