#pragma once

#include <Command.h>

namespace FlagRTS
{
	class Unit;
	class SceneObject;

	// Orders transporter to load packages from this mine ( do not affect mine but 
	// executes proper command on targeted mine )
	class DllExclusive MetalMineCallTransporterCommand : public Command
	{
		Unit* _targetTransporter;

	public:
		MetalMineCallTransporterCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};

	// Orders mine to pick up a package ( if package is unavailable it will
	// wait for it first ) and load it to transporter
	// Should be called by transporter
	class DllExclusive LoadPackageToTransporterCommand : public Command
	{
		Unit* _targetMine;

	public:
		LoadPackageToTransporterCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};

	// If transporter is ordered to leave driveway or is destroyed etc.
	// it will order mine to abandon package loading
	// Applicable only when waiting for package or loading and before mine's crane reached
	// If mine is waiting for package ot will turn idle after ( or waiting for
	// driveway to be cleared state ) it is in place instead of lifting it
	// If mine is in loading state it should reverse the crane animation
	class DllExclusive AbandonPackageLoadingCommand : public Command
	{
		Unit* _targetMine;

	public:
		AbandonPackageLoadingCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};
}