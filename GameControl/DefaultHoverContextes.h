#pragma once 

#include <HoverContext.h>

namespace FlagRTS
{
	// Hover context used when there's no actual hover check
	class DisabledHoverContext : public HoverContext
	{
	public:
		DisabledHoverContext();

		void UpdateHoverTarget(CommandTarget*);
	};

	// Hover context used when some units are selected
	class UnitGroupHoverContext : public HoverContext
	{
	public:
		UnitGroupHoverContext();

		void UpdateHoverTarget(CommandTarget*);
	};

	// Hover context to be used when no units are selected
	class EmptySelectionHoverContext : public HoverContext
	{
	public:
		EmptySelectionHoverContext();

		void UpdateHoverTarget(CommandTarget*);
	};

	// Hover context for move command, dependent on group class
	class MoveCommandHoverContext : public HoverContext
	{
	public:
		MoveCommandHoverContext();

		void UpdateHoverTarget(CommandTarget*);
	};
	
	// Hover context for attack command, dependent on group class
	class AttackCommandHoverContext : public HoverContext
	{
	public:
		AttackCommandHoverContext();

		void UpdateHoverTarget(CommandTarget*);
	};
}