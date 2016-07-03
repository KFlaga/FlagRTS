#pragma once

#include "IEvent.h"

namespace FlagRTS
{
	class MultiAction;

	// Trigger specifies event-actions relation
	// Stores event and actions associated with it
	// If trigger is enabled, it adds actions to event ( so they are executed if event is fired )
	// Triggers themselves doesn't create events/actions and must
	// be supplied with them
	// Actions are owned by Triggers and destroyed when Trigger is destroyed
	class Trigger
	{
	protected:
		string _name;
		bool _isEnabled;
		IEvent* _event;
		MultiAction* _actions;

	public:
		Trigger(const string& name);
		~Trigger();

		const string& GetName() const { return _name; }
		void SetName(const string& name) { _name = name; }

		// Sets enabled to true and adds action to event they are set
		void Enable();
		// Set enabled to false and removes action from event if they are set
		void Disable();

		bool IsEnabled() const { return _isEnabled; }

		IEvent* GetEvent() const { return _event; }
		// When setting new event, actions are removed from old one if
		// trigger is enabled and both are set
		void SetEvent(IEvent* evt);

		MultiAction* GetActions() const { return _actions; }
		// When setting new actions old ones are destroyed and removed from event
		// if trigger is enabled and both are set
		// If is enabled actions are added to new event
		void SetActions(MultiAction* actions);
	};
}