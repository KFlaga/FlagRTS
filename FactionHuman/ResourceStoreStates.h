#pragma once

#include <UnitStates.h>

namespace FlagRTS
{
	// Change to this state when transporter is close enough and orders store
	// to open itself ( must also mark it as occupied (set Transporter field in data) )
	// ( or transporter is in and want to move out )
	// Doors opens and on anim finish field doors opened flag is set ( transporter should
	// check it by itself )
	class DllExclusive StoreDoorOpeningState : public UnitState
	{
	public:
		StoreDoorOpeningState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	// Transporter moved in and ordered to close the doors behind him ( or move out )
	// Doors closes and on anim finish doors closed flag is set
	class DllExclusive StoreDoorClosingState : public UnitState
	{
	public:
		StoreDoorClosingState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	// Store have non-standard idle state, as it do not end animation on state end
	// and add 'processing' animation after package is recieveied
	class DllExclusive StoreIdleState : public UnitState
	{
		float _processingTimer;
		bool _processing;

	public:
		StoreIdleState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};
}