#pragma once

#include <string>

namespace FlagRTS
{
	namespace FogVisibility
	{
		enum FogVisibility : int
		{
			Visible = 1,
			Dimmed,
			Hidden
		};

		FogVisibility Parse(const std::string& fvString);
	}

	namespace CollisionType
	{
		enum CollisionType : int
		{
			None = 0,
			Ground = 1,
			Air = 2,
			GroundAndAir = 3
		};

		CollisionType Parse(const std::string& ctString);
	}

	namespace PlacementType
	{
		enum PlacementType : int
		{
			None = 0,
			Ground = 1,
			Air = 2
		};

		PlacementType Parse(const std::string& ptString);
	}
}