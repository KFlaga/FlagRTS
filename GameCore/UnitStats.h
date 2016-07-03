#pragma once

#include <Xml.h>
#include <Array.h>

namespace FlagRTS
{
	// Defines cost in resource to build unit
	// Multiple costs with same resource num may be defined for unit
	struct ResourceCost
	{
		const char* ResName; // Name of resource.
		int ResAmount; // Amount of resources unit costs
		bool ReturnOnDeath; // If true resource will be returned when unit dies ( or production is aborted )
	};

	// Contains all common basic stats for units
	struct UnitStats
	{
	public:
		int MaxHitPoints;
		int HitPoints;
		//int Energy;
		//float HpRegen;
		//float EnergyRegen;
		//int Armor;
		float MoveSpeed;
		float RotateSpeed;

		float BuildTime; // Time in ms it take to build/train unit
		Array<ResourceCost> Cost;

		UnitStats();
		UnitStats(UnitStats* stats);
		UnitStats(const UnitStats& stats);
		~UnitStats();

		void Parse(XmlNode* statsNode);
	};
}