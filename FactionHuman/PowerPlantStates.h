#pragma once

#include <UnitStates.h>

namespace FlagRTS
{
	// Base and probably only powerplant state - just generate energy
	class DllExclusive PowerPlantWorkingState : public UnitState
	{
		float _tickTimer;
	public:
		PowerPlantWorkingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};
	
}