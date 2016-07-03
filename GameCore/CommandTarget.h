#pragma once

//#include "PhysicalObject.h"
#include "DataTypes.h"

namespace FlagRTS
{
	class Unit;

	// Universal target for commands
	class CommandTarget
	{
	protected:
		int _targetType;
		Unit* _objectTarget;
		Vector3 _pointTarget;
		int _prefferedCommand; // Command that is preffered to be executed -> unit still should check if it can
							   // execute this command and otherwise choose one based on HoverTargetType

	public:
		CommandTarget() :
			_objectTarget(0)
		{ }

		int GetTargetType() const { return _targetType; }
		void SetTargetType(int value) { _targetType = value; }
		void AddTargetType(int value) { _targetType |= value; }

		Unit* GetTargetUnit() const { return _objectTarget; }
		void SetTargetUnit(Unit* value) { _objectTarget = value; }

		const Vector3& GetPointTarget() const { return _pointTarget; }
		void SetPointTarget(const Vector3& value) { _pointTarget = value; }

		int GetPrefferedCommand() const { return _prefferedCommand; }
		void SetPrefferedCommand(int value) { _prefferedCommand = value; }
	};
}