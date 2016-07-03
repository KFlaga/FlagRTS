#pragma once

#include <UnitStates.h>

namespace FlagRTS
{
	class DllExclusive MinePackageReadyState : public UnitState
	{
	public:
		MinePackageReadyState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	class DllExclusive MineLoadingPackageToTransporterState : public UnitState
	{
	private:
		bool _packagePicked;
		bool _packageHandedOver;

	public:
		MineLoadingPackageToTransporterState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);

	private:
		void OnPackagePickedUp(float overms);
		void OnPackageHandedOver(float overms);
		void OnCraneReturned(float overms);
	};

	class DllExclusive MineReturningWithPackageState : public UnitState
	{
	private:
		bool _packageDropped;

	public:
		MineReturningWithPackageState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};

	class DllExclusive MineWaitingForPackageState : public UnitState
	{
	public:
		MineWaitingForPackageState(Unit* owner);

		void Begin();
		void End();
		void Update(float ms);
	};
}