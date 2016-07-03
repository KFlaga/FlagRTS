#pragma once

#include <ObjectHandle.h>
#include <DataTypes.h>
#include <Event.h>

namespace FlagRTS
{
	class Unit;

	struct DllExclusive ResourceStoreTypeData
	{
		Vector3 DoorSpot; // Point before doors where units should go before goin in
		Vector3 InsideSpot; // Point inside store where units should go
		float PackageProcessingTime;

		ResourceStoreTypeData();
	};

	struct DllExclusive ResourceStoreObjectData
	{
		Unit* Transporter; // Transporter which currently moves in/out of store or 0
		bool DoorsOpened; // Doors are fully closed
		bool DoorsClosed; // Doors are fully opened
		// !opened + !closed = during animation
		bool NewResourcesStored; // Set by transporter after it left package here

		ResourceStoreObjectData();

	public:
		// When setting transporter, set also callback on its death to 
		// reset transporter pointer
		DEFINE_DELEGATE1(TransporterDiedEventHandler, 
			ResourceStoreObjectData, 
			ResetTransporter, Unit*);
		TransporterDiedEventHandler OnTransporterDied;

	private:
		void ResetTransporter(Unit* transporter)
		{
			Transporter = 0;
		}
	};
}