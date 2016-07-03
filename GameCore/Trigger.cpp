#include "Trigger.h"

namespace FlagRTS
{
	Trigger::Trigger(const string& name) :
		_name(name),
		_event(0),
		_actions(0),
		_isEnabled(false)
	{ }

	Trigger::~Trigger()
	{
		xDeleteSafe(_actions);
	}

	void Trigger::Enable()
	{
		if(_isEnabled == false && _event != 0 && _actions != 0)
		{
			_event->AddAction(_actions);
			_isEnabled = true;
		}
	}

	void Trigger::Disable()
	{
		if(_isEnabled == true &&_event != 0 && _actions != 0)
		{
			_event->RemoveAction(_actions);
			_isEnabled = false;
		}
	}

	void Trigger::SetEvent(IEvent* evt)
	{
		if(_isEnabled && _actions != 0)
		{
			if(_event != 0)
				_event->RemoveAction(_actions);
			if(evt != 0)
				_event->AddAction(_actions);
		}

		_event = evt;
	}

	void Trigger::SetActions(MultiAction* actions)
	{
		if(_isEnabled &&  _event != 0)
		{
			if(_actions != 0)
				_event->RemoveAction(_actions);
			if(actions != 0)
				_event->AddAction(actions);
		}

		xDeleteSafe(_actions);
		_actions = actions;
	}
}