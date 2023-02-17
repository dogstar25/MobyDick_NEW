#include "BBEnumMap.h"
#include "GameConstants.h"

BBEnumMap::BBEnumMap()
{

	//Custom Collision Tags
	addEnumItem("ContactTag::HEAVY_PARTICLE", ContactTag::HEAVY_PARTICLE);
	addEnumItem("ContactTag::LIGHT_PARTICLE", ContactTag::LIGHT_PARTICLE);
	addEnumItem("ContactTag::PLAYER_COLLISION", ContactTag::PLAYER_COLLISION);
	addEnumItem("ContactTag::PLAYER_TOUCH", ContactTag::PLAYER_TOUCH);
	addEnumItem("ContactTag::PLAYER_HITBOX", ContactTag::PLAYER_HITBOX);
	addEnumItem("ContactTag::WALL", ContactTag::WALL);
	addEnumItem("ContactTag::DOOR", ContactTag::DOOR);
	addEnumItem("ContactTag::WARP_ENTRY", ContactTag::WARP_ENTRY);

	//Custom Component Types
	addEnumItem("BOBBY_PLAYER_CONTROL_COMPONENT", ComponentTypes::BOBBY_PLAYER_CONTROL_COMPONENT);

	//Custom Status/Context Item Types
	addEnumItem("StatusItemId::PLAYERS_HEART_COUNT", StatusItemId::PLAYERS_HEART_COUNT);

	//Custom animation states
	addEnumItem("AnimationState::WALK_LEFT", AnimationState::WALK_LEFT);
	addEnumItem("AnimationState::WALK_RIGHT", AnimationState::WALK_RIGHT);
	addEnumItem("AnimationState::IDLE_LEFT", AnimationState::IDLE_LEFT);
	addEnumItem("AnimationState::IDLE_RIGHT", AnimationState::IDLE_RIGHT);

}