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

	size_t addItem(std::shared_ptr<GameObject> gameObject);
	std::vector<std::weak_ptr<GameObject>> items() { return m_items; }
	int activeItem() {	return m_activeItem; }

	std::optional<GameObject*> getItem(const int traitTag);
	int addCollectible(const CollectibleTypes, int count);
	
	const std::map<CollectibleTypes, int>& collectibles() { return m_collectibles; }
	void render();
	void update();

private:

	int m_activeItem{ 0 };
	std::vector<std::weak_ptr<GameObject>> m_items{};
	std::map<CollectibleTypes, int> m_collectibles{};



};

#endif
