#pragma once

namespace FlagRTS
{
	class Unit;

	// Desribes unit behaviour when performing an activity
	// So if unit is idle IdleAIState may check surroundings for enemies to engage etc.
	// If engage / movement etc are caused by AI it may be overriden
	// if better action is found ( i.e. if pursuing enemy which appeared within unit sight
	// it may change target to closer / slower / weaker etc on its own will )
	// AI is overriden by commands -> unit will not switch target / stop movement etc
	// if it was ordered
	class UnitAIState
	{
	protected:
		Unit* _owner;

	public:
		UnitAIState(Unit* owner) :
			_owner(owner)
		{ }

		virtual void Begin() { }
		
		virtual void End() { }

		virtual void Update(float ms) = 0;
		
		virtual const char* GetName()
		{
			return "Unknown";
		}

		virtual size_t GetType()
		{
			return 0;
		}
	};

	namespace UnitAIStates
	{
		// Enum with common activities
		enum StateType : size_t
		{
			Unknown = 0,
			Disabled, // If unit behaviour is forced by commands or unit dont have AI / have is disabled
			Idle,
			IdleAggresive,
			IdleHoldGround,
			IdlePassive,
			IdleRetreat,

			FollowingUnit, // Follow object - unlike MovingToObject it will not end on first touch
			ChasingEnemy, // Chase enemy -> move to its predicted position ( from current velocity ) and attack when in range
			AidingUnit, // Aids friendly unit -> just copy its activities if possible or heal/repair etc if aplicable
			EngagingEnemy, // Attacks unit/units if have multiple weapons, but also uses offensive abilities if they're auto-cast
			TransportingResources, // Complex activity for resource transporters ( invloves gathering and returning resources )
			
			UnderConstruction, // Unit is under construction - it may be stopped or may do some fancy stuff like changing its model
			Other,
			Internal,
			StateTypesCount
		};
	}
	typedef UnitAIStates::StateType AIStateType;
}