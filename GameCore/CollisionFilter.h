#pragma once

#include <TypeDefs.h>

namespace FlagRTS
{
	namespace CollisionGroups
	{
		enum CollisionGroupType : uint16
		{
			None = 0,
			Ground = 1<<0,
			Air = 1<<1,
			Naval = 1<<2,
			Building = 1<<3,
			Missle = 1<<4,

			Invalid = (uint16)-1,
		};

		CollisionGroupType ParseCollisionGroupType(const char* name);
	}
	typedef CollisionGroups::CollisionGroupType CollisionGroup;

	struct CollisionFilter
	{
		union 
		{
			uint32 Value;
			struct 
			{
				uint16 Groups; // Some ORed CollisionGroups object belongs to
				uint16 Blocking; // Some ORed CollisionGroups object blocks
			};
		};

		CollisionFilter() : Value(0l) { }
		CollisionFilter(const uint32 value) : Value(value) { }
		CollisionFilter(const uint16 groups, const uint16 blocks) : 
			Groups(groups), Blocking(blocks) { }
		operator uint32() const { return Value; }

		bool Blocks(const CollisionFilter otherFilter) const
		{
			return (this->Blocking & otherFilter.Groups) != 0;
		}

		bool IsBlockedBy(const CollisionFilter otherFilter) const
		{
			return (this->Groups & otherFilter.Blocking) != 0;
		}

		void operator+=(CollisionFilter filter2)
		{
			Value |= filter2.Value;
		}

		void operator-=(CollisionFilter filter2)
		{
			Value &= ~filter2.Value;
		}

		CollisionFilter operator+(CollisionFilter filter2)
		{
			return CollisionFilter(Value | filter2.Value);
		}
		
		CollisionFilter operator-(CollisionFilter filter2)
		{
			return CollisionFilter(Value & (~filter2.Value));
		}
	};
}