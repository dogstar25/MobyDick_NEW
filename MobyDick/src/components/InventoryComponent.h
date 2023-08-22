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

struct InventoryItem {

	SDL_FPoint displayOffset{};
	std::optional <std::shared_ptr<GameObject>> gameObject{};

};


class InventoryComponent : public Component
{
public:

	InventoryComponent();
	InventoryComponent(Json::Value componentJSON, std::string parentName, Scene* parentScene);
	~InventoryComponent();

	bool addItem(std::shared_ptr<GameObject> gameObject);
	bool addItem(std::string gameObjectType);
	std::vector<InventoryItem> items() { return m_items; }
	int activeItem() {	return m_activeItem; }

	std::optional<std::shared_ptr<GameObject>> getItem(const int traitTag);
	int addCollectible(const CollectibleTypes, int count);
	
	const std::map<CollectibleTypes, int>& collectibles() { return m_collectibles; }
	void render();
	void update();

	void showInventory(GameObject* parentObject);
	void showInventory();
	void hideInventory();
	bool isOpen() { return m_isOpen; }
	std::optional<std::shared_ptr<GameObject>> getDisplayBackdropObject() { return m_displayBackdropObject; }

private:

	int m_activeItem{ 0 };
	bool m_isOpen{};
	std::vector<InventoryItem> m_items{};
	std::map<CollectibleTypes, int> m_collectibles{};
	std::string m_displayBackdropObjectType{};
	std::optional<std::shared_ptr<GameObject>> m_displayBackdropObject{};
	SDL_FPoint m_displayStartOffset{};
	float m_itemDisplaySize{};
	float m_itemDisplayPadding{};
	float m_displayRows{};
	float m_displayColumns{};
	float m_maxCapacity{};

	void _calculateSlotOffsets();




};

#endif
