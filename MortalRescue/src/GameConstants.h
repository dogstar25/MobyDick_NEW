 #pragma once
#include "BaseConstants.h"
#include "imgui.h"

namespace MRColors {

	inline SDL_Color green1 = { 10, 75, 10, 255 };
	inline SDL_Color green2 = { 10, 125, 10, 255 };
	inline SDL_Color grey1 = { 25, 25, 25, 255 };
	inline SDL_Color grey2 = { 39, 52, 39, 255 };
}


namespace CollisionTag {
	
	inline constexpr int SMOKE_PARTICLE = 2;
	inline constexpr int NAVIGATION_POINT = 3;
	inline constexpr int HEAVY_PARTICLE = 4;
	inline constexpr int LIGHT_PARTICLE = 5;
	inline constexpr int PLAYER = 6;
	inline constexpr int WALL = 7;
	inline constexpr int DEFLECT_EFFECT = 11;
	inline constexpr int DRONE_BRAIN = 12;
	inline constexpr int DRONE_FRAME = 13;
	inline constexpr int DRONE_SHIELD = 14;
	inline constexpr int DRONE_WEAPON = 15;
	inline constexpr int ENEMY_BULLET = 16;
	inline constexpr int FRIENDLY_BULLET = 17;
	inline constexpr int PLAYER_BULLET = 18;
	inline constexpr int SHIELD_SCRAP = 19;
	inline constexpr int SURVIVOR = 20;
	inline constexpr int MEDKIT = 21;
	inline constexpr int WEAPON_PICKUP = 22;

}

namespace CollectibleTypes {
	inline constexpr int DRONE_SCRAP = 1;
}

namespace ComponentTypes {

	inline constexpr int BRAIN_DRONE_COMPONENT = 22;
	inline constexpr int WEAPON_PISTOL_COMPONENT = 23;
	inline constexpr int TURRET_BRAIN_COMPONENT = 24;

}
