#pragma once

#include <Event.h>
#include <ObjectHandle.h>
#include <DataTypes.h>

namespace FlagRTS
{
	class Unit;
	class PhysicalObject;

	struct DllExclusive MetalMineTypeData
	{
		ObjectHandle PackageDefinitionHandle;
		float PackageWaitTime;
		Vector3 TransporterDriveInSpot;
		Vector3 PackageSpawnSpot;
		Vector3 PackageFinalSpot;
		int PackageMetalAmount;

		MetalMineTypeData();
	};

	struct DllExclusive MetalMineObjectData
	{
		float PackageTimer;
		int AvailableMetal;
		Unit* Transporter;
		PhysicalObject* Package;
		bool PackageReady;

		MetalMineObjectData();

	public:
		// When setting transporter, set also callback on its death to 
		// reset transporter pointer
		DEFINE_DELEGATE1(TransporterDiedEventHandler, 
			MetalMineObjectData, 
			ResetTransporter, Unit*);
		TransporterDiedEventHandler OnTransporterDied;

	private:
		void ResetTransporter(Unit* transporter)
		{
			Transporter = 0;
		}
	};
}