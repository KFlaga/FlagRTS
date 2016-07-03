#pragma once

#include <Command.h>

namespace FlagRTS
{
	class Unit;
	class SceneObject;

	// Orders transporter to deliver package to this store
	class DllExclusive StoreCallTransporterCommand : public Command
	{
		Unit* _targetTransporter;

	public:
		StoreCallTransporterCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};

	// Changes state to opening doors
	class DllExclusive StoreOpenDoorsCommand : public Command
	{
		Unit* _targetMine;

	public:
		StoreOpenDoorsCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};
	
	// Changes state to closing doors
	class DllExclusive StoreCloseDoorsCommand : public Command
	{
		Unit* _targetMine;

	public:
		StoreCloseDoorsCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};
}