#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class KillReasonFixedRetriever : public IObjectRetriever<int>
	{
		int _value;
	public:
		KillReasonFixedRetriever(int value) :
			_value(value)
		{ }

		operator int() const
		{
			return _value;
		}
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(KillReasonFixedRetriever, _value); 
		}
	};

	class KillReasonKilledByPlayerRetriever : public IObjectRetriever<int>
	{
		IObjectRetriever<int>* _player;

	public:
		KillReasonKilledByPlayerRetriever(IObjectRetriever<int>* player):
			_player(player)
		{ }
		~KillReasonKilledByPlayerRetriever();

		operator int() const;
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(KillReasonKilledByPlayerRetriever, _player->GetCopy()); 
		}
	};
	
	class KillReasonRetrieverFactory
	{
	public:
		static IObjectRetriever<int>* Create(XmlNode* valueNode);
	};
}