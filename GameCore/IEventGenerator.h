#pragma once



namespace ORTS
{
	// Common interface for classes that provides events for rest of game
	// ( like input observer, camera observer, units observer )
	// Each EventGenerator have TypeId which identifies event family
	// EventGenerators get notify requests for objects that want action to
	// be done when event occurs : any object can register ITrigger with
	// specific event type ( like key pressed ), condition ( like key == Escape )
	// and action. When event type occurs condition is checked, and if
	// is fulfilled action is taken.
	// Each event type is bind to list of triggers and in each update
	// only subscribed events ( list's size > ) are checked if occurred
	class IEventGenerator
	{
	protected:
		TypeId _finalType; // Type of subclass. Should be set after base class initialization
		typedef std::list<RefPtr<ITrigger>> TriggerList;
		typedef std::map<TypeId, RefPtr<TriggerList>> EventMap;
		
		EventMap _events;

	public:
		// In constructor lists for all event types should be allocated
		IEventGenerator() { _finalType = GetTypeId<IEventGenerator>(); }
		// Empty destructor - all triggers should be destoryed automaticaly
		virtual ~IEventGenerator() { }
		// Returns type of final subclass
		TypeId GetFinalType() const { return _finalType; }

		// Checks all registered event if occured and if occured +
		// condition is met, action is performed
		virtual void ProcessEvents()
		{
			for(auto eventIt = _events.begin(); eventIt != _events.end(); ++eventIt)
			{
				if(eventIt->second->size() > 0)
				{
					if( CheckIfEventOccured(eventIt->second) )
					{
						for(auto trigIt = eventIt->second->begin();
							trigIt != eventIt->second->end(); ++trigIt)
						{
							ProcessEventInteral((*trigIt));
						}
					}
				}
			}
		}
		// Adds trigger for Trigger's event type to be fired
		void RegisterTrigger(const RefPtr<ITrigger>& trigger)
		{
			FindTriggerList(trigger)->push_back(trigger);
		}
		// Removes trigger from Trigger's event type list
		void UnregisterTrigger(const RefPtr<ITrigger>& trigger)
		{
			FindTriggerList(trigger)->remove(trigger);
		}
	protected:

		RefPtr<TriggerList>& FindTriggerList(const RefPtr<ITrigger>& t) 
		{
			return _events.find(t->GetEventType())->second;
		}

		virtual bool CheckIfEventOccured(const RefPtr<ITrigger>& ev)
		{
			return ev->GetEvent()->CheckEventOccured();
		}

		virtual void ProcessEventInteral(const RefPtr<ITrigger>& trigger) = 0;
	};
}