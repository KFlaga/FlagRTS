#pragma once

#include <UnitStates.h>

namespace FlagRTS
{
	class DllExclusive FactoryDoorOpeningState : public UnitTrainingState
	{
	public:
		FactoryDoorOpeningState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);

		bool TryFinishTrain(Unit* trainedUnit);
	};

	class DllExclusive FactoryDoorClosingState : public UnitTrainingState
	{
	public:
		FactoryDoorClosingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);

		bool TryFinishTrain(Unit* trainedUnit);
	};

	class DllExclusive FactoryTrainingState : public UnitTrainingState
	{
	public:
		FactoryTrainingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);

		bool TryFinishTrain(Unit* trainedUnit);
	};

	class DllExclusive FactoryUnitMovingOutState : public UnitTrainingState
	{
		Vector3 _doorsLeft;
		Vector3 _doorsRight;
		Vector3 _moveOutDir;
		float _y2y1;
		float _x2x1;
		float _centerD;
	public:
		FactoryUnitMovingOutState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);

		bool TryFinishTrain(Unit* trainedUnit);
	};
}