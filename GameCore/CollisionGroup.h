#pragma once

namespace FlagRTS
{
	namespace CollisionGroups
	{
		enum CollisionGroupType : short
		{
			None = 0,
			Ground = 1<<0,
			Air = 1<<1,
			Naval = 1<<2,
			Building = 1<<3,
			Missle = 1<<4
		};

		CollisionGroupType ParseCollisionGroupType(const char* name);
	}
	typedef CollisionGroups::CollisionGroupType CollisionGroup;
}