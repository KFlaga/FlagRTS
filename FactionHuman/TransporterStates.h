#pragma once

#include <UnitStates.h>

namespace FlagRTS
{
	class PhysicalObject;

	// Non-default idle state : transporter may look for closest mine to go
	class DllExclusive TransporterIdleState : public UnitIdleState
	{
		float _autoMineTimer;

	public:
		TransporterIdleState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	// In this state unit is commandable unless theres free spot on mine
	// Transporter is near metal mine drive-in, but mine is currently occupied
	// It just waits for free spot, occupies ot and moves to DriveInSpot precisely
	class DllExclusive TransporterWaitingForMetalMineToBeFreedState : public UnitState
	{
		bool _waitingForSpot;

	public:
		TransporterWaitingForMetalMineToBeFreedState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	// In this state unit is uncommandable
	// On start transorter needs to be in correct spot near metal mine
	// ( rotation is not important - it will rotate in this state )
	// Truck turns its back to mine and moves up the driveway
	// State ends when truck reached its final spot and switches to 'TransporterWaitingForMetalMinePackageState'
	// State can be ended prematurely only by transporter/mine death ( if one can say machines can die )
	class DllExclusive TransporterMovingIntoMetalMineDrivewayState : public UnitState
	{
	private:
		bool _onSpot;
		bool _rotated;
		bool _driveinFinished;

	public:
		TransporterMovingIntoMetalMineDrivewayState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);

	private:

	};

	// In this state unit is uncommandable
	// On start transorter needs to be in correct spot on metal mine driveway
	// It just waits for mine to signal it loaded package and switches to 'TransporterLeavingMetalMineState'
	class DllExclusive TransporterWaitingForMetalMinePackageState : public UnitState
	{
	public:
		TransporterWaitingForMetalMinePackageState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	// In this state unit is uncommandable
	// On start transorter needs to be in correct spot on metal mine driveway
	// Truck leaves the drive way and switches to 'TransporterDeliveringPackageState'
	class DllExclusive TransporterLeavingMetalMineState : public UnitState
	{
	public:
		TransporterLeavingMetalMineState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	class DllExclusive TransporterLeavingMetalMineNoPackageState : public UnitState
	{
	public:
		TransporterLeavingMetalMineNoPackageState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	// Transprter moves to target store 
	// On start if no store is targetet is scans for closest one
	class DllExclusive TransporterDeliveringPackageState : public UnitState
	{
	public:
		TransporterDeliveringPackageState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	// In this state unit is commandable unless theres free spot on store
	// Transporter is near store doors, but store is currently occupied
	// It just waits for free spot, occupies ot and moves to DoorSpot precisely
	class DllExclusive TransporterWaitingForResourceStoreToBeFreedState : public UnitState
	{
		bool _waitingForSpot;

	public:
		TransporterWaitingForResourceStoreToBeFreedState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	class DllExclusive TransporterMovingIntoStoreState : public UnitState
	{
		bool _rotated;
		bool _onSpot;
		bool _doorsOpened;

	public:
		TransporterMovingIntoStoreState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	class DllExclusive TransporterInStoreStoreState : public UnitState
	{
		bool _waitingForDoorClosed;
		bool _waitingForDoorOpened;

	public:
		TransporterInStoreStoreState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	class DllExclusive TransporterMovingOutOfStoreState : public UnitState
	{
	public:
		TransporterMovingOutOfStoreState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};


	// Transprter moves to target mine 
	// On start if no mine is targetet is scans for closest one
	class DllExclusive TransporterPickingUpPackageState : public UnitState
	{
	public:
		TransporterPickingUpPackageState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};
}