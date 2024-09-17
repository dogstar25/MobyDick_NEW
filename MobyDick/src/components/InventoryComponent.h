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

		InventoryComponent()=default;
		InventoryComponent(Json::Value componentJSON, GameObject* parent, std::string parentName, Scene* parentScene);
		~InventoryComponent();

		//void setParent(GameObject* gameObject) override;
		void postInit() override;

		bool addItem(std::shared_ptr<GameObject> gameObject, int slot);
		bool addItem(std::shared_ptr<GameObject> gameObject);
		bool addItem(std::string gameObjectType, std::string name="");
		bool addItem(std::string gameObjectType, int slot, std::string name="");
		bool hasItem(int slot);
		std::optional<int> getSlot(GameObject* gameObject);
		std::shared_ptr<GameObject> removeItem(int slot);
		std::shared_ptr<GameObject> removeItem(GameObject* gameObject);
		std::vector<std::optional<std::shared_ptr<GameObject>>> items() { return m_items; }

		std::optional<std::shared_ptr<GameObject>> getFirstByType(std::string gameObjectType);
		std::optional<std::shared_ptr<GameObject>> getFirstByTrait(const int traitTag);

		int addCollectible(const CollectibleTypes, int count);
	
		const std::map<CollectibleTypes, int>& collectibles() { return m_collectibles; }
		void update();

		void refreshInventoryDisplay();
		bool isFull();
		void showInventory();
		void hideInventory();
		void clearInventory();
		bool isOpen() { return m_isOpen; }
		void setOpen(bool open) { m_isOpen = open; }
		std::optional<std::weak_ptr<GameObject>> getDisplayObject() { return m_displayObject; }
		void setDisplayObjectColor(SDL_Color color);
		void setDisplayObjectTexture(std::string textureId);

	private:

		bool m_isOpen{};
		bool m_isAlwaysOpen{};
		std::vector<std::optional<std::shared_ptr<GameObject>>> m_items{};
		std::map<CollectibleTypes, int> m_collectibles{};
		std::optional<std::weak_ptr<GameObject>> m_displayObject{};
		float m_maxCapacity{};

		void _removeFromWorldPass();

};

#endif
