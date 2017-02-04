#pragma once

#include "TechRequirement.h"
#include <Xml.h>

namespace FlagRTS
{
	// Some base derives of TechRequirement with predefined conditions
	class Unit;

	class PlayerUnitKindRequirement  : public TechRequirement
	{
	public:
		class PlayerUnitKindCondition : public ICondition
		{
		protected:
			size_t _unitKind;
			int _unitCount;
			uint8 _player;
			friend PlayerUnitKindRequirement;
		};

	public:
		PlayerUnitKindRequirement(XmlNode* reqNode);
		PlayerUnitKindRequirement(TechAvailability onTrue, TechAvailability onFalse, PlayerUnitKindCondition* condition);
		~PlayerUnitKindRequirement();

		void SetPlayer(uint8 player);
		void SetUnitKind(size_t uk);
		void SetUnitCount(int count);
		
		size_t GetUnitKind() const;
		int GetUnitCount() const;

	protected:
		void UnitStatsChanged(Unit* unit, uint8 player);
		DEFINE_DELEGATE2(UnitStatsDelegate, PlayerUnitKindRequirement,
			UnitStatsChanged, Unit*, uint8);
		UnitStatsDelegate _onUnitStatsChanged;
	};

	class PlayerHaveAtLeastUnitsOfKindRequirement : public PlayerUnitKindRequirement
	{
	public:
		PlayerHaveAtLeastUnitsOfKindRequirement(XmlNode* reqNode);
		PlayerHaveAtLeastUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse);

		TechRequirement* GetCopy();
	};

	class PlayerHaveNoMoreThanUnitsOfKindRequirement : public PlayerUnitKindRequirement
	{
	public:
		PlayerHaveNoMoreThanUnitsOfKindRequirement(XmlNode* reqNode);
		PlayerHaveNoMoreThanUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse);
		
		TechRequirement* GetCopy();
	};

	class PlayerHaveExaclyUnitsOfKindRequirement : public PlayerUnitKindRequirement
	{
	public:
		PlayerHaveExaclyUnitsOfKindRequirement(XmlNode* reqNode);
		PlayerHaveExaclyUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse);

		TechRequirement* GetCopy();
	};

	class PlayerSpawnedAtLeastUnitsOfKindRequirement : public PlayerUnitKindRequirement
	{
	public:
		PlayerSpawnedAtLeastUnitsOfKindRequirement(XmlNode* reqNode);
		PlayerSpawnedAtLeastUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse);

		TechRequirement* GetCopy();
	};

	class PlayerSpawnedNoMoreThanUnitsOfKindRequirement : public PlayerUnitKindRequirement
	{
	public:
		PlayerSpawnedNoMoreThanUnitsOfKindRequirement(XmlNode* reqNode);
		PlayerSpawnedNoMoreThanUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse);

		TechRequirement* GetCopy();
	};

	class PlayerSpawnedExaclyUnitsOfKindRequirement : public PlayerUnitKindRequirement
	{
	public:
		PlayerSpawnedExaclyUnitsOfKindRequirement(XmlNode* reqNode);
		PlayerSpawnedExaclyUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse);

		TechRequirement* GetCopy();
	};
}