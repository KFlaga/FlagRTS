#pragma once

#include <string>

namespace FlagRTS
{
	// Types of targets mouse can be hovered with
	// (provide basic checks for command targets and cursor)
	// For more specific check UnitClass or other members can be checked
	namespace HoverTargetTypes
	{
		enum HoverTargetType : int
		{
			NoTarget = 0,
			Terrain = 1<<0, // hover over terrain / non-unit
			Minimap = 1<<1, // hover over minimap
			Hud = 1<<2, // hover over hud
			Menu = 1<<3, // hover over in-game menu
			Invalid = 1<<4, // Bad target for command
			Valid = 1<<5, // Valid target for command

			DefaultTarget = 1 << 8,
			Unit = 1 << 9, // Object is unit
			ObstacleGround = 1 << 10, // Non-passable object/terrain for ground units
			ObstacleAir = 1 << 11, // Non-passable object/terrain for air units
			ObstacleNaval = 1 << 12, // Non-passbale object/terrain for naval units
			Obstacle = ObstacleGround | ObstacleAir | ObstacleNaval, // Non-passable object for all units
			Passable = 1 << 13, // Object/terrain that all units can pass through
			PassableGround = ObstacleAir | ObstacleNaval, // Object that only ground unit can pass through
			PassableAir = ObstacleGround | ObstacleNaval, // Object that only air unit can pass through
			PassableNaval = ObstacleGround | ObstacleAir, // Object that only naval unit can pass through
			OwnUnit = Unit | 1 << 14, // Unit owned by client player
			AllyUnit = Unit | 1 << 15, // Unit owned by ally
			NeutralUnit = Unit | 1 << 16, // Unit owned by player with neutral alignment in respect to client player
			EnemyUnit = Unit | 1 << 17, // Unit by client player's enemy
			HostileUnit = Unit | 1 << 18, // Unit owned by hostile non-player
			Marker = 1 << 24, // Interactable scene marker
			OtherObject = 1<<30
		};

		int ParseHoverTargetType(const std::string& name);
	}
	typedef HoverTargetTypes::HoverTargetType HoverTargetType;
}