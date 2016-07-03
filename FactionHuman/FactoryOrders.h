#pragma once

#include <Command.h>

namespace FlagRTS
{
	class Unit;
	class SceneObject;

	// Changes state to opening doors
	class DllExclusive FactoryOpenDoorsCommand : public Command
	{
	public:
		FactoryOpenDoorsCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};

	class DllExclusive FactoryCloseDoorsCommand : public Command
	{
	public:
		FactoryCloseDoorsCommand(Unit* owner);

		void Update(float ms);
		void Execute();
		void Abort();
		Command* GetCopy();

		bool IsTargetValid(CommandTarget* target) const;
	};
}