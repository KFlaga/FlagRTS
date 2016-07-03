#pragma once

#include "ICondition.h"
#include <Array.h>
#include <Xml.h>
#include "IObjectRetriever.h"

namespace FlagRTS
{
	class PlayerFamilyConditionFactory : public IConditionFactory
	{
		std::map<string, IConditionFactory*> _factories;

	public:
		PlayerFamilyConditionFactory();
		~PlayerFamilyConditionFactory();

		ICondition* Create(XmlNode* condNode);
	};

	// Condition true if given player IsUsed
	class PlayerIsInGameCondition : public IParametrizedCondition<IObjectRetriever<int>*>
	{
	protected:
		IObjectRetriever<int>* _player;

	public:
		bool CheckCondition();

		PlayerIsInGameCondition* GetCopy();

		void SetParameters(IObjectRetriever<int>* player)
		{
			_player = player;
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};

	// Condition true if given player have given faction
	class PlayerHaveFactionCondition : public IParametrizedCondition<IObjectRetriever<int>*, IObjectRetriever<string>*>
	{
	protected:
		IObjectRetriever<int>* _player;
		IObjectRetriever<string>* _faction;

	public:
		bool CheckCondition();

		PlayerHaveFactionCondition* GetCopy();

		void SetParameters(IObjectRetriever<int>* player, IObjectRetriever<string>* faction)
		{
			_player = player;
			_faction = faction;
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};
}