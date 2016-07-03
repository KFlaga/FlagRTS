#pragma once

#include "IUnitCommandSupplier.h"

namespace FlagRTS
{
	// Adds DoNothing command
	class BaseCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};

	// Adds MoveGround and Rotate commands to unit
	class MoveGroundCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};
	
	// Adds Rotate command to unit
	class RotateCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};

	// Adds AttackGround command to unit
	class AttackGroundCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};

	// Adds TrainX command to unit -> for each unit in trainable array
	class TrainUnitCommandSupplier : public IUnitCommandSupplier
	{
	public:
		void AddCommandsToUnit(Unit* unit);
	};
}