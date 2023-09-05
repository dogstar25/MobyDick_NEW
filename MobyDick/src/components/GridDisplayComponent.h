#pragma once

#include <array>
#include <vector>
#include <optional>
#include <SDL2/SDL.h>

#include <box2d/box2d.h>
#include <json/json.h>

#include "Component.h"
#include "../Util.h"
#include "../BaseConstants.h"

class GameObject;
class Scene;


struct GridSlot {

	SDL_FPoint positionOffset{};
	std::optional<std::weak_ptr<GameObject>> gameObject{};
};


class GridDisplayComponent : public Component
{
public:
	GridDisplayComponent();
	GridDisplayComponent(Json::Value componentJSON, Scene* parentScene);
	~GridDisplayComponent();

	void update() override;
	void render();
	void postInit() override;
	void addItem(std::shared_ptr<GameObject> gameObject, int slot);
	void addItem(std::shared_ptr<GameObject> itemObject, SDL_FPoint gridLocation);
	void removeItem(std::string id);
	std::vector<GridSlot> gridSlots() { return m_gridSlots; }

private:
	int m_rows{};
	int m_columns{};
	SDL_FPoint m_fullGridSize{};
	SDL_FPoint m_gridCenterPoint{};
	SDL_FPoint m_marginPadding{};
	float m_itemPadding{};
	float m_itemSize{};

	int	  m_itemCount{};
	void _buildGridSlots();

	//Every slot has to support multiple children since standrad slots you can have more than one child which gets spread out
	std::vector<GridSlot> m_gridSlots{};

	std::shared_ptr<GameObject> m_slotImageObject{};


};
