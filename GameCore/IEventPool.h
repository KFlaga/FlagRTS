#pragma once

#include <TypeId.h>
#include "DataTypes.h"
#include "IEvent.h"

namespace FlagRTS
{
	// Event pools stores IEvents from one family (like Input)
	// as well as check if they occured ( in Update, events may be set as pending
	// if they occured via Event<> ( not IEvent ), but TriggerSystem events are fired 
	// in Update only )
	class IEventPool
	{
	protected:
		string _familyName;
		
	public:
		IEventPool(const string& familyName) :
			_familyName(familyName)
		{ }
		virtual ~IEventPool() { }

		const string& GetEventFamilyName() const { return _familyName; }

		virtual IEvent* FindEvent(const string& typeName) = 0;

		virtual void Update(float ms) = 0;
	};
}