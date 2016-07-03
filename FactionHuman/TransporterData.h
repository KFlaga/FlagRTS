#pragma once

#include <ObjectHandle.h>
#include <DataTypes.h>
#include <Event.h>


namespace FlagRTS
{
	class Unit;
	class PhysicalObject;

	struct DllExclusive TransporterTypeData
	{
		Vector3 TransporterPackageSpot;
		bool AutoLookForMine; // If true when transporter is idle it will automaticaly go to mine
		float AutoMineTime; // Idle time after which transporter will look for mine

		TransporterTypeData();
	};

	struct DllExclusive TransporterObjectData
	{
		Unit* TargetMine;
		Unit* TargetStore;
		PhysicalObject* Package;
		bool IsStorePlayerChoosen; // If true player ordered to move to this store and it wont be auto switched to closer one

		TransporterObjectData();

	private:
		void ResetMine(Unit* transporter)
		{
			TargetMine = 0;
		}

		void ResetStore(Unit* transporter)
		{
			TargetStore = 0;
		}

	public:
		DEFINE_DELEGATE1(MineDiedEventHandler, 
			TransporterObjectData, 
			ResetMine, Unit*);
		MineDiedEventHandler OnMineDied;

		DEFINE_DELEGATE1(StoreDiedEventHandler, 
			TransporterObjectData, 
			ResetStore, Unit*);
		StoreDiedEventHandler OnStoreDied;
	};
}