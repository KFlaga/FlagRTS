#pragma once

#include <IGameObject.h>
#include "Technology.h"
#include "ICondition.h"
#include <Event.h>
#include <StringUtils.h>

namespace FlagRTS
{
	// TechRequirement checks arbitraru condition and based on that sets 
	// availability of some technology. Requirements are player specific and player must
	// be set before checking requirement
	class TechRequirement : public IGameObject
	{
	public:
		enum RequirementType : size_t
		{
			PlayerTechs, // Requirement on player owned technology
			CommandState, // Requirement on command/command owner state ( for commands )
		};

	protected:
		static const uint8 _noPlayer = (uint8)-1;
		const char* _name;
		TechAvailability _stateOnTrue;
		TechAvailability _stateOnFalse;
		TechAvailability _currentState;
		IParametrizedCondition<uint8>* _condition; // Condition depending on player
		const char* _inavailableHint; // Short text shown to player if tech is inavailable becouse of this requirement
									  // Probably not met requirements will be enlisted, so it should be something like:
									  // - "Structure X constructed" or - "Technology Y researched"

		Event<TechRequirement*> _onStateChanged;
		bool _isMet;
		uint8 _player;

	public:
		// ICondition is owned by TechRequirement and deleted in destructor
		TechRequirement(TechAvailability onTrue, TechAvailability onFalse, IParametrizedCondition<uint8>* condition) : 
			_stateOnTrue(onTrue),
			_stateOnFalse(onFalse),
			_currentState(Unchecked),
			_isMet(false),
			_condition(condition),
			_inavailableHint(0),
			_name(0),
			_player(_noPlayer)
		{ 
			// Handle Type is used to distinguish TechRequirement subtypes
			SetHandleTypePart(PlayerTechs);
		}

		virtual ~TechRequirement()
		{
			xDeleteSafe(_condition);
			sDeleteSafe(_inavailableHint);
			sDeleteSafe(_name);
		}

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
		bool CheckRequirement()
		{
			_isMet = _condition->CheckCondition();
			TechAvailability newState = _isMet == true ? _stateOnTrue : _stateOnFalse;
			if( newState != _currentState )
			{
				_currentState = newState;
				_onStateChanged.Fire(this);
			}

			return _isMet;
		}

		// Do not fire event ( used when checking all reqs of single tech )
		bool CheckRequirementQuiet()
		{
			_isMet = _condition->CheckCondition();
			TechAvailability newState = _isMet == true ? _stateOnTrue : _stateOnFalse;
			_currentState = newState;

			return _isMet;
		}

		// Sets player for this requirement - must be called before checking.
		// In dervied classes it allows also for doing player specific preparations.
		virtual void SetPlayer(uint8 player)
		{
			_player = player;
			_condition->SetParameters(player);
		}

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

		virtual TechRequirement* GetCopy() 
		{ 
			return xNew3(TechRequirement, _stateOnTrue, _stateOnFalse, 
				(IParametrizedCondition<uint8>*)_condition->GetCopy()); 
		}
	};

	// Returns combined availability of Technology which have given set of Requirements
	// Rules are simple : if any req causes it to be hidden, it is hidden, otherwise if
	// and req causes to be inavailable it is inavailable, otherwise it is available
	inline TechAvailability GetFinalAvailability(const Array<TechRequirement*>& reqs)
	{
		TechAvailability availability = TechAvailability::Available;
		for(unsigned int req = 0; req < reqs.size(); ++req)
		{
			TechAvailability reqAv = reqs[req]->GetTargetAvailability();
			if(reqAv == TechAvailability::Hidden)
			{
				availability = TechAvailability::Hidden;
				break;
			}
			else if(reqAv == TechAvailability::Inavailable)
			{
				availability = TechAvailability::Inavailable;
			}
		}
		return availability;
	}
}