#include "TechRequirement.h"
#include <Exception.h>

namespace FlagRTS
{
	TechRequirement::TechRequirement(TechAvailability onTrue, TechAvailability onFalse, ICondition* condition) : 
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
		//SetHandleTypePart(PlayerTechs);
	}

	TechRequirement::TechRequirement(XmlNode* reqNode)
	{
		/*<Requirement name="PlayerHaveLogisticsCenter" type="HaveAtLeastUnits">
		<Params on_true="Available" on_false="Inavailable" unit_name="LogisticsCenter" count="1"/>
		<Hint locale="true" loc="Hint_HaveLogisticsCenter" sec="Game.ReqsHints"/>
		</Requirement>*/

		SetName(XmlUtility::XmlGetString(reqNode, "name"));

		XmlNode* reqParamsNode = reqNode->first_node("Params");
		if(reqParamsNode != 0)
		{
			_stateOnTrue = ParseTechAvailability(XmlUtility::XmlGetString(reqParamsNode, "on_true"));
			_stateOnFalse = ParseTechAvailability(XmlUtility::XmlGetString(reqParamsNode, "on_false"));
		}
		else
		{
			CastException(string("Error when creating TechRequirement with name: ") + _name + ". No Params node");
		}

		XmlNode* hintNode = reqNode->first_node("Hint");
		if( hintNode != 0 )
		{
			const char* hint = XmlUtility::XmlGetLocaleString(hintNode).c_str();
			SetInavailableHint(hint);
		}
		else
		{
			SetInavailableHint("");
		}
	}

	TechRequirement::~TechRequirement()
	{
		xDeleteSafe(_condition);
		sDeleteSafe(_inavailableHint);
		sDeleteSafe(_name);
	}

	bool TechRequirement::CheckRequirement()
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

	bool TechRequirement::CheckRequirementQuiet()
	{
		_isMet = _condition->CheckCondition();
		TechAvailability newState = _isMet == true ? _stateOnTrue : _stateOnFalse;
		_currentState = newState;

		return _isMet;
	}

	void TechRequirement::SetPlayer(uint8 player)
	{
		_player = player;
	}

	TechRequirement* TechRequirement::GetCopy() 
	{ 
		return xNew3(TechRequirement, _stateOnTrue, _stateOnFalse, _condition->GetCopy()); 
	}

	TechAvailability GetFinalAvailability(const Array<TechRequirement*>& reqs)
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