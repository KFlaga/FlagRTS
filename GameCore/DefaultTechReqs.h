#pragma once

#include "TechRequirement.h"
#include <IFactory.h>
#include <Xml.h>

namespace FlagRTS
{
	// Some base derives of TechRequirement with predefined conditions
	class Unit;

	class PlayerHaveAtLeastUnitsOfKindRequirement : public TechRequirement
	{
		class Condition : public IParametrizedCondition<uint8>
		{
			size_t _unitKind;
			int _atLeastCount;
			uint8 _player;
			friend PlayerHaveAtLeastUnitsOfKindRequirement;

		public:
			Condition(size_t kind, int count);
			void SetParameters(uint8 player)
			{
				_player = player;
			}
			bool CheckCondition();
			ICondition* GetCopy() { return xNew2(Condition, _unitKind, _atLeastCount); }
		};
		
	public:
		PlayerHaveAtLeastUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse,
			size_t unitKind, int count);
		~PlayerHaveAtLeastUnitsOfKindRequirement();

		void SetPlayer(uint8 player);
		void SetUnitKind(size_t uk) { static_cast<Condition*>(_condition)->_unitKind = uk; }

		TechRequirement* GetCopy();

		class Factory : public IFactoryCast<TechRequirement, XmlNode*>
		{
		public:
			Factory();
			IGameObject* Create(XmlNode* node);
		};

	private:
		void UnitStatsChanged(Unit* unit, uint8 player);
		DEFINE_DELEGATE2(UnitStatsDelegate, PlayerHaveAtLeastUnitsOfKindRequirement,
			UnitStatsChanged, Unit*, uint8);
		UnitStatsDelegate _onUnitStatsChanged;
	};

	class PlayerHaveNoMoreThanUnitsOfKindRequirement : public TechRequirement
	{
		class Condition : public IParametrizedCondition<uint8>
		{
			size_t _unitKind;
			int _atMostCount;
			uint8 _player;
			friend PlayerHaveNoMoreThanUnitsOfKindRequirement;

		public:
			Condition(size_t kind, int count);
			void SetParameters(uint8 player)
			{
				_player = player;
			}
			bool CheckCondition();
			ICondition* GetCopy() { return xNew2(Condition, _unitKind, _atMostCount); }
		};

	public:
		PlayerHaveNoMoreThanUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse,
			size_t unitKind, int count);
		~PlayerHaveNoMoreThanUnitsOfKindRequirement();
		
		void SetPlayer(uint8 player);
		void SetUnitKind(size_t uk) { static_cast<Condition*>(_condition)->_unitKind = uk; }

		TechRequirement* GetCopy();

		class Factory : public IFactoryCast<TechRequirement, XmlNode*>
		{
		public:
			Factory();
			IGameObject* Create(XmlNode* node);
		};

	private:
		void UnitStatsChanged(Unit* unit, uint8 player);
		DEFINE_DELEGATE2(UnitStatsDelegate, PlayerHaveNoMoreThanUnitsOfKindRequirement,
			UnitStatsChanged, Unit*, uint8);
		UnitStatsDelegate _onUnitStatsChanged;
	};

	class PlayerHaveExaclyUnitsOfKindRequirement : public TechRequirement
	{
		class Condition : public IParametrizedCondition<uint8>
		{
			size_t _unitKind;
			int _count;
			uint8 _player;
			friend PlayerHaveExaclyUnitsOfKindRequirement;

		public:
			Condition(size_t kind, int count);
			void SetParameters(uint8 player)
			{
				_player = player;
			}
			bool CheckCondition();
			ICondition* GetCopy() { return xNew2(Condition, _unitKind, _count); }
		};

	public:
		PlayerHaveExaclyUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse,
			size_t unitKind, int count);
		~PlayerHaveExaclyUnitsOfKindRequirement();
		
		void SetPlayer(uint8 player);
		void SetUnitKind(size_t uk) { static_cast<Condition*>(_condition)->_unitKind = uk; }

		TechRequirement* GetCopy();

		class Factory : public IFactoryCast<TechRequirement, XmlNode*>
		{
		public:
			Factory();
			IGameObject* Create(XmlNode* node);
		};

	private:
		void UnitStatsChanged(Unit* unit, uint8 player);
		DEFINE_DELEGATE2(UnitStatsDelegate, PlayerHaveExaclyUnitsOfKindRequirement,
			UnitStatsChanged, Unit*, uint8);
		UnitStatsDelegate _onUnitStatsChanged;
	};

	class PlayerSpawnedAtLeastUnitsOfKindRequirement : public TechRequirement
	{
		class Condition : public IParametrizedCondition<uint8>
		{
			size_t _unitKind;
			int _atLeastCount;
			uint8 _player;
			friend PlayerSpawnedAtLeastUnitsOfKindRequirement;

		public:
			Condition(size_t kind, int count);
			void SetParameters(uint8 player)
			{
				_player = player;
			}
			bool CheckCondition();
			ICondition* GetCopy() { return xNew2(Condition, _unitKind, _atLeastCount); }
		};

	public:
		PlayerSpawnedAtLeastUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse,
			size_t unitKind, int count);
		~PlayerSpawnedAtLeastUnitsOfKindRequirement();
		
		void SetPlayer(uint8 player);
		void SetUnitKind(size_t uk) { static_cast<Condition*>(_condition)->_unitKind = uk; }

		TechRequirement* GetCopy();

		class Factory : public IFactoryCast<TechRequirement, XmlNode*>
		{
		public:
			Factory();
			IGameObject* Create(XmlNode* node);
		};

	private:
		void UnitStatsChanged(Unit* unit, uint8 player);
		DEFINE_DELEGATE2(UnitStatsDelegate, PlayerSpawnedAtLeastUnitsOfKindRequirement,
			UnitStatsChanged, Unit*, uint8);
		UnitStatsDelegate _onUnitStatsChanged;
	};

	class PlayerSpawnedNoMoreThanUnitsOfKindRequirement : public TechRequirement
	{
		class Condition : public IParametrizedCondition<uint8>
		{
			size_t _unitKind;
			int _atMostCount;
			uint8 _player;
			friend PlayerSpawnedNoMoreThanUnitsOfKindRequirement;

		public:
			Condition(size_t kind, int count);
			void SetParameters(uint8 player)
			{
				_player = player;
			}
			bool CheckCondition();
			ICondition* GetCopy() { return xNew2(Condition, _unitKind, _atMostCount); }
		};

	public:
		PlayerSpawnedNoMoreThanUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse,
			size_t unitKind, int count);
		~PlayerSpawnedNoMoreThanUnitsOfKindRequirement();

		void SetPlayer(uint8 player);
		void SetUnitKind(size_t uk) { static_cast<Condition*>(_condition)->_unitKind = uk; }

		TechRequirement* GetCopy();

		class Factory : public IFactoryCast<TechRequirement, XmlNode*>
		{
		public:
			Factory();
			IGameObject* Create(XmlNode* node);
		};

	private:
		void UnitStatsChanged(Unit* unit, uint8 player);
		DEFINE_DELEGATE2(UnitStatsDelegate, PlayerSpawnedNoMoreThanUnitsOfKindRequirement,
			UnitStatsChanged, Unit*, uint8);
		UnitStatsDelegate _onUnitStatsChanged;
	};

	class PlayerSpawnedExaclyUnitsOfKindRequirement : public TechRequirement
	{
		class Condition : public IParametrizedCondition<uint8>
		{
			size_t _unitKind;
			int _count;
			uint8 _player;
			friend PlayerSpawnedExaclyUnitsOfKindRequirement;

		public:
			Condition(size_t kind, int count);
			void SetParameters(uint8 player)
			{
				_player = player;
			}
			bool CheckCondition();
			ICondition* GetCopy() { return xNew2(Condition, _unitKind, _count); }
		};

	public:
		PlayerSpawnedExaclyUnitsOfKindRequirement(TechAvailability onTrue, TechAvailability onFalse,
			size_t unitKind, int count);
		~PlayerSpawnedExaclyUnitsOfKindRequirement();
		
		void SetPlayer(uint8 player);
		void SetUnitKind(size_t uk) { static_cast<Condition*>(_condition)->_unitKind = uk; }

		TechRequirement* GetCopy();

		class Factory : public IFactoryCast<TechRequirement, XmlNode*>
		{
		public:
			Factory();
			IGameObject* Create(XmlNode* node);
		};

	private:
		void UnitStatsChanged(Unit* unit, uint8 player);
		DEFINE_DELEGATE2(UnitStatsDelegate, PlayerSpawnedExaclyUnitsOfKindRequirement,
			UnitStatsChanged, Unit*, uint8);
		UnitStatsDelegate _onUnitStatsChanged;
	};
}