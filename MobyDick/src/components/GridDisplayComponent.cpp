#include "GridDisplayComponent.h"
#include "../EnumMap.h"
#include "../GameObject.h"
#include "../game.h"
#include "../EnumMap.h"

extern std::unique_ptr<Game> game;

GridDisplayComponent::GridDisplayComponent()
{

}

GridDisplayComponent::GridDisplayComponent(Json::Value componentJSON, Scene* parentScene)
{


	m_componentType = ComponentTypes::GRID_DISPLAY_COMPONENT;

	m_itemPadding = componentJSON["itemPadding"].asFloat();

	m_rows = componentJSON["rows"].asInt();
	m_columns = componentJSON["columns"].asInt();
	m_marginPadding = { componentJSON["marginPadding"]["x"].asFloat(), componentJSON["marginPadding"]["y"].asFloat() };
	m_itemSize = componentJSON["itemSize"].asFloat();

	std::optional<int> locationSlot{};
	bool centeredOnLocation{true};

	m_isDependentObjectOwner = true;
	std::optional<b2Vec2> sizeOverride{};

	//Build all possible Standard Slot - they will be empty
	//_buildStandardSlots();

	//Build all possible Grid Slots - they will be empty
	_buildGridSlots();

	m_slotImageObject = parentScene->createGameObject("GRID_SLOT_BOX", -1.0F, -1.0F, 0.F, parentScene, GameLayer::MAIN, false);

	

}


GridDisplayComponent::~GridDisplayComponent()
{

	

}

void GridDisplayComponent::postInit()
{



}

void GridDisplayComponent::_buildGridSlots()
{

	//Calculate the total width, height, and center point of the grid object we are going to build
	m_fullGridSize.x = (m_itemSize * m_columns) + (m_itemPadding * (m_columns - 1));
	m_fullGridSize.y = (m_itemSize * m_rows) + (m_itemPadding * (m_rows - 1));
	m_gridCenterPoint = { m_fullGridSize.x / 2.0f, m_fullGridSize.y / 2.0f };

	m_gridSlots = std::vector<GridSlot>(m_rows * m_columns);

	//Calculate each slots offset from center location
	//first calulate top left corner grid location
	//b2Vec2 topLeftGridLocation = { 0 - ((m_fullGridSize.x * m_columns) + (m_itemPadding * (m_columns-1))) / 2 + m_marginPadding.x, 
	//	0 - ((m_fullGridSize.y * m_rows) + (m_itemPadding * (m_rows - 1))) / 2 + m_marginPadding.y };
	SDL_FPoint topLeftGridLocation = { 0 - m_gridCenterPoint.x, 0 - m_gridCenterPoint.y };
	topLeftGridLocation.x = topLeftGridLocation.x + m_marginPadding.x;
	topLeftGridLocation.y = topLeftGridLocation.y + m_marginPadding.y;

	m_gridSlots[0] = GridSlot(topLeftGridLocation, std::nullopt);

	//Now calculate the rest of the grid locations
	for (int row = 0; row < m_rows; row++) {
		for (int column = 0; column < m_columns; column++) {

			//skip first top left slot since its already calculated
			if (row == 0 && column == 0) continue;

			SDL_FPoint location =
			{ topLeftGridLocation.x + column * ((m_itemSize)+(m_itemPadding)),
				topLeftGridLocation.y + row * ((m_itemSize)+(m_itemPadding)) };

			int slotIndex = row * m_columns + column;
			m_gridSlots[slotIndex] = GridSlot(location, std::nullopt);
		}

	}


	//for (int row = 0; row < m_rows; row++) {
	//	for (int col = 0; col < m_columns; col++) {

	//		SDL_FPoint location =
	//		{  ((m_itemSize + m_itemPadding) * (col+1)) / 2,
	//			((m_itemSize + m_itemPadding) * (row + 1)) / 2 };



	//		int slotIndex = row * m_columns + col;
	//		m_gridSlots[slotIndex] = GridSlot(location, std::nullopt);
	//	}

	//}

}

void GridDisplayComponent::update()
{



	for (auto& gridSlot : m_gridSlots)
	{

		if (gridSlot.gameObject.has_value()) {

			if (gridSlot.gameObject.value().lock()->isDragging() == false) {
				b2Vec2 newPosition =
				{ parent()->getCenterPosition().x + gridSlot.positionOffset.x,
				parent()->getCenterPosition().y + gridSlot.positionOffset.y };


				gridSlot.gameObject.value().lock()->setPosition(newPosition, -1);
			}
		}


	}


}

std::optional<int> GridDisplayComponent::getClosestSlot(SDL_FPoint position)
{

	int slotIndex{};
	for (const auto& slot : m_gridSlots) {

		//calc the world position of the center of this slot
		SDL_FPoint slotWorldPosition =
		{ parent()->getCenterPosition().x + slot.positionOffset.x, parent()->getCenterPosition().y + slot.positionOffset.y };

		if (abs(slotWorldPosition.x - position.x) < (m_itemSize / 2) &&
			abs(slotWorldPosition.y - position.y) < (m_itemSize / 2) ) {
			return slotIndex;
			}

		slotIndex++;
	}

	return std::nullopt;

}

void GridDisplayComponent::addItem(std::shared_ptr<GameObject> itemObject, int slot)
{

	m_gridSlots[slot].gameObject = itemObject;

}

void GridDisplayComponent::addItem(std::shared_ptr<GameObject> itemObject, SDL_FPoint gridLocation)
{

	int slotIndex = gridLocation.y * m_columns + gridLocation.x;
	m_gridSlots[slotIndex].gameObject = itemObject;

}

void GridDisplayComponent::clear()
{

	for (auto& slot : m_gridSlots) {

		slot.gameObject = std::nullopt;

	}

}


void GridDisplayComponent::render()
{

	
	for (auto& slot : m_gridSlots) {


		SDL_FRect slotImageObjectLocation{};
		slotImageObjectLocation.x = parent()->getCenterPosition().x + slot.positionOffset.x;
		slotImageObjectLocation.y = parent()->getCenterPosition().y + slot.positionOffset.y;
		slotImageObjectLocation.w = m_itemSize + 3;
		slotImageObjectLocation.h = m_itemSize + 3;

		//Get top left for rendering
		slotImageObjectLocation.x -= slotImageObjectLocation.w /2;
		slotImageObjectLocation.y -= slotImageObjectLocation.h /2;

		const auto& slotImageRenderComponent = m_slotImageObject->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
		SDL_FRect slotImageObjectRenderLocation = slotImageRenderComponent->getRenderDestRect(slotImageObjectLocation);

		m_slotImageObject->render(slotImageObjectRenderLocation);

		if (slot.gameObject.has_value()) {

			const auto& renderCompo = slot.gameObject.value().lock()->getComponent<RenderComponent>(ComponentTypes::RENDER_COMPONENT);
			
			slot.gameObject.value().lock()->render();
		}

	}


}




