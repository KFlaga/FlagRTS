#pragma once

#include <Command.h>

namespace FlagRTS
{
	class Unit;
	class SceneObject;

	// Orders transporter to deliver loaded package to valid drop point (building)
	// (works if transporter have package loaded)
	class DllExclusive DeliverPackageCommand : public Command
	{
		Vector3 _storeSpot;

	public:
		DeliverPackageCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		void Suspend();
		void Resume();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
		
	protected:
		/*DEFINE_DELEGATE1(TargetDespawnedEventHandler, BasicMoveGroundCommand,
			OnTargetDespawned, SceneObject*);
		TargetDespawnedEventHandler _onTargetDespawned;
		void OnTargetDespawned(SceneObject* target)
		{
			_moveToObject = false;
		}*/
	};

	// Orders transporter to move to metal mine and pick up package
	// ( first orders to move to mine's driveway by supplying move commands
	// then wait for free slot in mine. Command finishes when transporter is able to
	// get to driveway, rest is handled by tarnsporter state)
	// If transporter have a package it will deliver it first and then return to
	// target mine.
	// LATER : add transporter ability to abandon packages if there will be 
	// another resource packages available, then if package is another resource
	// it will abandon it and get one from this mine
	class DllExclusive PickUpPackageFromMetalMineCommand : public Command
	{
		Vector3 _mineDriveInSpot;

	public:
		PickUpPackageFromMetalMineCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		void Suspend();
		void Resume();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};

	// Orders transporter to leave mine's driveway
	// Not casted directly, but only if transporter is forces to leave by another commands/objects
	// It is used for normal ( package is loaded ) and anormal circumstances ( like forced transporter movement )
	// Also executes Mine's AbanodonLoad command ( which will do nothing if package is loaded successfully )
	class DllExclusive LeaveMetalMineDriveWayCommand : public Command
	{
	public:
		LeaveMetalMineDriveWayCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};
}