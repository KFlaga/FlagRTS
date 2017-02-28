#pragma once

#include "Technology.h"
#include "ICondition.h"
#include <Event.h>
#include <StringUtils.h>

namespace FlagRTS
{
	// TechRequirement checks arbitraru condition and based on that sets 
	// availability of some technology. Requirements are player specific and player must
	// be set before checking requirement
	class TechRequirement
	{
	protected:
		static const uint8 _noPlayer = (uint8)-1;
		const char* _name;
		TechAvailability _stateOnTrue;
		TechAvailability _stateOnFalse;
		TechAvailability _currentState;
		ICondition* _condition;
		const char* _inavailableHint; // Short text shown to player if tech is inavailable becouse of this requirement
									  // Probably not met requirements will be enlisted, so it should be something like:
									  // - "Structure X constructed" or - "Technology Y researched"

		Event<TechRequirement*> _onStateChanged;
		bool _isMet;
		uint8 _player;

	public:
		
		TechRequirement(XmlNode* requirementNode);
		// ICondition is owned by TechRequirement and deleted in destructor
		TechRequirement(TechAvailability onTrue, TechAvailability onFalse, ICondition* condition);

		virtual ~TechRequirement();

		const char* GetName() const { return _name; }
		// Name is copied and deleted in destructor
		void SetName(const char* name)
		{
			xDeleteSafe(_name);
			_name = CopyChar(name);
		}

		// Returns availability of target unit/research on current requirement state
		TechAvailability GetTargetAvailability() 
		{ 
			return _currentState; 
		}

		bool IsRequirementMet() const { return _isMet; }

		// Before checking requirement, player should be set
		bool CheckRequirement();

		// Do not fire event ( used when checking all reqs of single tech )
		bool CheckRequirementQuiet();

		// Sets player for this requirement - must be called before checking.
		// In dervied classes it allows also for doing player specific preparations.
		virtual void SetPlayer(uint8 player);

		uint8 GetPlayer() const { return _player; }

		const char* GetInavailableHint() const 
		{
			return _inavailableHint;
		}

		// Text set will be copied here and deleted in destructor, so temporary pointer can be passed
		void SetInavailableHint(const char* hint)
		{
			xDeleteSafe(_inavailableHint);
			_inavailableHint = CopyChar(hint);
		}

		Event<TechRequirement*>& GetStateChangedEvent() { return _onStateChanged; }

		virtual TechRequirement* GetCopy();
	};

	class TechRequirementFactory
	{
	//	DISALLOW_COPY(TechRequirementFactory);

	//private:
	//	typedef ArrayMaps<IObjectDefinitionFactory*>::KeyCCString FactoryMap;
	//	FactoryMap _subFactories;

	//public:
	//	SubObjectDefinitionFactory();
	//	~SubObjectDefinitionFactory();
	//	ObjectDefinition* Create(XmlNode* objNode);

	//	// Adds factory to available subclass factories
	//	// If factory with given key already exists, then old one is deleted and replaced
	//	void RegisterFactory(const string& typeName, IObjectDefinitionFactory* factory);
	//	
	//	// Removes factory from available subclass factories
	//	// If factory with given key doesn't exist - nothing happens
	//	void UnregisterFactory(const string& typeName);
	//	
	//	// Returns factory of given type
	//	// If factory with given key doesn't exist returns 0
	//	IObjectDefinitionFactory* GetFactoryOfType(const string& typeName);
	};

	// Returns combined availability of Technology which have given set of Requirements
	// Rules are simple : if any req causes it to be hidden, it is hidden, otherwise if
	// and req causes to be inavailable it is inavailable, otherwise it is available
	TechAvailability GetFinalAvailability(const Array<TechRequirement*>& reqs);
}