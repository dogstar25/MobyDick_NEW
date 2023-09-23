#ifndef CHILDREN_COMPONENT_H
#define CHILDREN_COMPONENT_H

#include <array>
#include <vector>
#include <optional>
#include <SDL2/SDL.h>

#include <box2d/box2d.h>
#include <json/json.h>

#include "Component.h"
#include "../Util.h"
#include "../BaseConstants.h"

#include <unordered_map>


class GameObject;
class Scene;
class TransformComponent;


enum class ChildSlotType {

	STANDARD_SLOT,
	ABSOLUTE_POSITION
};

enum class ChildSlotTreatment {

	HORIZONTAL,
	VERTICAL,
	STACKED
};

struct SlotMeasure {
	b2Vec2 sizeTotal{};
	b2Vec2 maxDimension{};
};

struct Child {
	SDL_FPoint absolutePositionOffset{};
	SDL_FPoint calculatedOffset{};
	bool isStepChild{};
	std::optional<std::shared_ptr<GameObject>> gameObject{};
};




class ChildrenComponent : public Component
{
public:
	ChildrenComponent();
	ChildrenComponent(Json::Value componentJSON, std::string parentName, Scene* scene);
	~ChildrenComponent();

	void update() override;
	void render();
	void postInit();
	void addStepChild(std::shared_ptr<GameObject> gameObject, PositionAlignment positionAlignment, bool addToEnd=true);
	void addStepChild(std::shared_ptr<GameObject> gameObject, SDL_FPoint position, bool addToEnd = true);
	void removeChild(std::string id);
	std::unordered_map<std::string, std::vector<Child>> childSlots() { return m_childSlots; }

private:
	int	  m_childCount{};
	float m_childPadding{};
	bool  m_matchParentRotation{};
	ChildSlotTreatment m_childSameSlotTreatment{ ChildSlotTreatment::VERTICAL };
	std::array<int, CHILD_POSITIONS> m_childSlotCount{};

	std::string _buildChildName(std::string parentName, int childCount);
	void _removeFromWorldPass();
	void _removeAllChildren(GameObject* gameObject);

	std::string _determineSlotKey(PositionAlignment positionAlignment);
	std::string _determineSlotKey(SDL_FPoint position);
	std::string _addChild(std::shared_ptr<GameObject> childObject, PositionAlignment positionAlignment);
	std::string _addChild(std::shared_ptr<GameObject> childObject, SDL_FPoint position);

	ChildSlotType _getSlotType(std::string slotKey);
	int  _getSlotIndex(std::string slotKey);
	void _buildStandardSlots();
	void _buildGridSlots();
	PositionAlignment _translateStandardSlotPositionAlignment(std::string slotKey);

	void _calculateStandardSlotPositions(std::vector<Child>& childObjects, PositionAlignment positionAlignment, std::vector<SlotMeasure>& slotMeasurements);
	void _calculateAbsoluteSlotPositions(std::vector<Child>& childObjects, int slot, std::vector<SlotMeasure>& slotMeasurements);

	std::vector<SlotMeasure> _calculateSlotMeasurements(std::string slotKey);
	SDL_FPoint _calculateStandardSlotCenterPosition(PositionAlignment positionAlignment, std::vector<SlotMeasure>& slotSize, int slotChildCount);
	SDL_FPoint _calculateAbsoluteSlotCenterPosition(SDL_FPoint position, int slot, std::vector<SlotMeasure>& slotSize);
	SDL_FPoint _calculateSlotMultiChild(SDL_FPoint slotCenterPositionOffset, int slotGameObjectNumber, int totalSlotObjectCount, GameObject* childGameObject);

	//Every slot has to support multiple children since standrad slots you can have more than one child which gets spread out
	std::unordered_map<std::string, std::vector<Child>> m_childSlots{};


};

#endif
