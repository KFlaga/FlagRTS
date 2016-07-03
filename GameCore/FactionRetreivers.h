#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class FactionRetrieverFactory
	{
	public:
		static IObjectRetriever<string>* Create(XmlNode* vectorNode);
	};

	class FactionFixedRetriever : public IObjectRetriever<string>
	{
	private:
		string _faction;

	public:
		FactionFixedRetriever(const string& faction) :
			_faction(faction)
		{ }

		operator string() const
		{
			return _faction;
		}
		
		IObjectRetriever<string>* GetCopy() 
		{ 
			return xNew1(FactionFixedRetriever, _faction); 
		}
	};

	class FactionVariableRetriever : public IObjectRetriever<string>
	{
		string _varName;
	public:
		FactionVariableRetriever(const string& value) :
			_varName(value)
		{ }

		operator string() const;
		
		IObjectRetriever<string>* GetCopy() 
		{ 
			return xNew1(FactionVariableRetriever, _varName); 
		}
	};

	class FactionOfPlayerRetriever : public IObjectRetriever<string>
	{
		IObjectRetriever<int>* _player;
	public:
		FactionOfPlayerRetriever(IObjectRetriever<int>* playerRetriever) :
			_player(playerRetriever)
		{ }
		~FactionOfPlayerRetriever();

		operator string() const;
		
		IObjectRetriever<string>* GetCopy() 
		{ 
			return xNew1(FactionOfPlayerRetriever, _player->GetCopy()); 
		}
	};
}