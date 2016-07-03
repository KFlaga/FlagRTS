#pragma once

#include <ObjectHandle.h>
#include <DataTypes.h>
#include <Event.h>


namespace FlagRTS
{
	class Unit;
	class PhysicalObject;

	struct DllExclusive FactoryTypeData
	{
		Vector3 DoorCenter; // Center of factory doors ( doors are treated as line )
		float DoorsHalfLength;
		Vector3 InsideSpot; // Point inside factory where units are spawned
		FactoryTypeData();
	};

	struct DllExclusive FactoryObjectData
	{
		Unit* UnitInside; // Unit which currently moves out of factory or 0
		bool DoorsOpened; // Doors are fully closed
		bool DoorsClosed; // Doors are fully opened
		FactoryObjectData();
	};
}