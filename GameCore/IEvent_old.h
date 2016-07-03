#pragma once

#include <TypeId.h>
#include <Pointers.h>
#include <IGameObject.h>

namespace ORTS
{
	// EventId fully classifies event type - defines its source
	// and source specific type - used for quick searching if proper events
	class EventId
	{
	public:
		TypeId FamilyType;
		TypeId EventType;

		EventId(TypeId type, TypeId source) :
			FamilyType(source), EventType(type)
		{ }
	};

	// Event objects represents user/ingame
	// interaction. Each event contain info on event type,
	// method to check if event occured and optional
	// condition
	class IEvent : public IGameObject
	{
	public:
		IEvent() : IGameObject() { }

		virtual EventId GetEventId() const = 0;
		virtual bool CheckEventOccured() = 0;
	};


}