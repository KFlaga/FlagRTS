#include "DefaultTechReqs.h"
#include "GameWorld.h"
#include "MainGameObjectPool.h"
#include "Unit.h"
#include "PlayerStatsManager.h"

namespace FlagRTS
{
	template<typename ReqT>
	ReqT* CreateFromXml(XmlNode* node)
	{
		TechAvailability onTrue = ParseTechAvailability(XmlUtility::XmlGetString(node, "on_true"));
		TechAvailability onFalse = ParseTechAvailability(XmlUtility::XmlGetString(node, "on_false"));
		string unitName = XmlUtility::XmlGetString(node, "unit_name");

		int count = XmlUtility::XmlGetInt(node, "count");
		ReqT* req = xNew4(ReqT, onTrue, onFalse, 0, count);

		auto getUnitKind = [unitName, req]() 
		{	
			size_t unitKind = GameWorld::GlobalWorld->GetObjectPool()->
				GetObjectDefinitionByNameCast<UnitDefinition>(unitName, "Unit")->GetUnitKind();
			req->SetUnitKind(unitKind);
		};
		MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
			xNew1(DelegateEventHandler<decltype(getUnitKind)>,getUnitKind);

		return req;
	}


	PlayerHaveAtLeastUnitsOfKindRequirement::Condition::Condition(size_t kind, int count) :
		_unitKind(kind), _atLeastCount(count), _player(0u)
	{ }

	bool PlayerHaveAtLeastUnitsOfKindRequirement::Condition::CheckCondition()
	{
		PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
		return playerStats->GetUnitKindCount(_unitKind) >= _atLeastCount;
	}

	PlayerHaveAtLeastUnitsOfKindRequirement::PlayerHaveAtLeastUnitsOfKindRequirement(
		TechAvailability onTrue, TechAvailability onFalse, size_t unitKind, int count) :
	TechRequirement( onTrue, onFalse, xNew2(Condition, unitKind, count) ),
		_onUnitStatsChanged(this)
	{ }

	PlayerHaveAtLeastUnitsOfKindRequirement::~PlayerHaveAtLeastUnitsOfKindRequirement()
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}
	}

	void PlayerHaveAtLeastUnitsOfKindRequirement::SetPlayer(uint8 player)
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}

		TechRequirement::SetPlayer(player);
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitConstructedEvent() += &_onUnitStatsChanged;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitKilledEvent() += &_onUnitStatsChanged;
	}

	void PlayerHaveAtLeastUnitsOfKindRequirement::UnitStatsChanged(Unit* unit, uint8 player)
	{
		if(_player == player &&
			unit->GetUnitKind() == static_cast<Condition*>(_condition)->_unitKind )
		{
			CheckRequirement();
		}
	}

	TechRequirement* PlayerHaveAtLeastUnitsOfKindRequirement::GetCopy()
	{
		return xNew4(PlayerHaveAtLeastUnitsOfKindRequirement, 
			_stateOnTrue, _stateOnFalse,
			static_cast<Condition*>(_condition)->_unitKind, 
			static_cast<Condition*>(_condition)->_atLeastCount);
	}

	PlayerHaveAtLeastUnitsOfKindRequirement::Factory::Factory() :
		IFactoryCast("TechRequirement")
	{ }

	IGameObject* PlayerHaveAtLeastUnitsOfKindRequirement::Factory::Create(XmlNode* node)
	{
		return CreateFromXml<PlayerHaveAtLeastUnitsOfKindRequirement>(node);
	}




	PlayerHaveNoMoreThanUnitsOfKindRequirement::Condition::Condition(size_t kind, int count) :
		_unitKind(kind), _atMostCount(count), _player(0u)
	{ }

	bool PlayerHaveNoMoreThanUnitsOfKindRequirement::Condition::CheckCondition()
	{
		PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
		return playerStats->GetUnitKindCount(_unitKind) <= _atMostCount;
	}

	PlayerHaveNoMoreThanUnitsOfKindRequirement::PlayerHaveNoMoreThanUnitsOfKindRequirement(
		TechAvailability onTrue, TechAvailability onFalse,
		size_t unitKind, int count) :
	TechRequirement( onTrue, onFalse, xNew2(Condition, unitKind, count) ),
		_onUnitStatsChanged(this)
	{ }

	PlayerHaveNoMoreThanUnitsOfKindRequirement::~PlayerHaveNoMoreThanUnitsOfKindRequirement()
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}
	}

	void PlayerHaveNoMoreThanUnitsOfKindRequirement::SetPlayer(uint8 player)
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}

		TechRequirement::SetPlayer(player);
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitConstructedEvent() += &_onUnitStatsChanged;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitKilledEvent() += &_onUnitStatsChanged;
	}

	void PlayerHaveNoMoreThanUnitsOfKindRequirement::UnitStatsChanged(Unit* unit, uint8 player)
	{
		if(_player == player &&
			unit->GetUnitKind() == static_cast<Condition*>(_condition)->_unitKind )
		{
			CheckRequirement();
		}
	}

	TechRequirement* PlayerHaveNoMoreThanUnitsOfKindRequirement::GetCopy()
	{
		return xNew4(PlayerHaveNoMoreThanUnitsOfKindRequirement, 
			_stateOnTrue, _stateOnFalse,
			static_cast<Condition*>(_condition)->_unitKind, 
			static_cast<Condition*>(_condition)->_atMostCount);
	}

	PlayerHaveNoMoreThanUnitsOfKindRequirement::Factory::Factory() :
		IFactoryCast("TechRequirement")
	{ }

	IGameObject* PlayerHaveNoMoreThanUnitsOfKindRequirement::Factory::Create(XmlNode* node)
	{
		return CreateFromXml<PlayerHaveNoMoreThanUnitsOfKindRequirement>(node);
	}



	PlayerHaveExaclyUnitsOfKindRequirement::Condition::Condition(size_t kind, int count) :
		_unitKind(kind), _count(count), _player(0u)
	{ }

	bool PlayerHaveExaclyUnitsOfKindRequirement::Condition::CheckCondition()
	{
		PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
		return playerStats->GetUnitKindCount(_unitKind) == _count;
	}

	PlayerHaveExaclyUnitsOfKindRequirement::PlayerHaveExaclyUnitsOfKindRequirement(
		TechAvailability onTrue, TechAvailability onFalse,
		size_t unitKind, int count) :
	TechRequirement( onTrue, onFalse, xNew2(Condition, unitKind, count) ),
		_onUnitStatsChanged(this)
	{ }

	PlayerHaveExaclyUnitsOfKindRequirement::~PlayerHaveExaclyUnitsOfKindRequirement()
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}
	}

	void PlayerHaveExaclyUnitsOfKindRequirement::SetPlayer(uint8 player)
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}

		TechRequirement::SetPlayer(player);
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitConstructedEvent() += &_onUnitStatsChanged;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitKilledEvent() += &_onUnitStatsChanged;
	}

	void PlayerHaveExaclyUnitsOfKindRequirement::UnitStatsChanged(Unit* unit, uint8 player)
	{
		if(_player == player &&
			unit->GetUnitKind() == static_cast<Condition*>(_condition)->_unitKind )
		{
			CheckRequirement();
		}
	}

	TechRequirement* PlayerHaveExaclyUnitsOfKindRequirement::GetCopy()
	{
		return xNew4(PlayerHaveAtLeastUnitsOfKindRequirement, 
			_stateOnTrue, _stateOnFalse,
			static_cast<Condition*>(_condition)->_unitKind, 
			static_cast<Condition*>(_condition)->_count);
	}

	PlayerHaveExaclyUnitsOfKindRequirement::Factory::Factory() :
		IFactoryCast("TechRequirement")
	{ }

	IGameObject* PlayerHaveExaclyUnitsOfKindRequirement::Factory::Create(XmlNode* node)
	{
		return CreateFromXml<PlayerHaveExaclyUnitsOfKindRequirement>(node);
	}



	PlayerSpawnedAtLeastUnitsOfKindRequirement::Condition::Condition(size_t kind, int count) :
		_unitKind(kind), _atLeastCount(count), _player(0u)
	{ }

	bool PlayerSpawnedAtLeastUnitsOfKindRequirement::Condition::CheckCondition()
	{
		PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
		return (int)playerStats->GetAllUnitsOfKind(_unitKind)->size() >= _atLeastCount;
	}

	PlayerSpawnedAtLeastUnitsOfKindRequirement::PlayerSpawnedAtLeastUnitsOfKindRequirement(
		TechAvailability onTrue, TechAvailability onFalse,
		size_t unitKind, int count) :
	TechRequirement( onTrue, onFalse, xNew2(Condition, unitKind, count) ),
		_onUnitStatsChanged(this)
	{ }

	PlayerSpawnedAtLeastUnitsOfKindRequirement::~PlayerSpawnedAtLeastUnitsOfKindRequirement()
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitSpawnedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}
	}

	void PlayerSpawnedAtLeastUnitsOfKindRequirement::SetPlayer(uint8 player)
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitSpawnedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}

		TechRequirement::SetPlayer(player);
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitSpawnedEvent() += &_onUnitStatsChanged;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitKilledEvent() += &_onUnitStatsChanged;
	}

	void PlayerSpawnedAtLeastUnitsOfKindRequirement::UnitStatsChanged(Unit* unit, uint8 player)
	{
		if(_player == player &&
			unit->GetUnitKind() == static_cast<Condition*>(_condition)->_unitKind )
		{
			CheckRequirement();
		}
	}

	TechRequirement* PlayerSpawnedAtLeastUnitsOfKindRequirement::GetCopy()
	{
		return xNew4(PlayerSpawnedAtLeastUnitsOfKindRequirement, 
			_stateOnTrue, _stateOnFalse,
			static_cast<Condition*>(_condition)->_unitKind, 
			static_cast<Condition*>(_condition)->_atLeastCount);
	}

	PlayerSpawnedAtLeastUnitsOfKindRequirement::Factory::Factory() :
		IFactoryCast("TechRequirement")
	{ }

	IGameObject* PlayerSpawnedAtLeastUnitsOfKindRequirement::Factory::Create(XmlNode* node)
	{
		return CreateFromXml<PlayerSpawnedAtLeastUnitsOfKindRequirement>(node);
	}



	PlayerSpawnedNoMoreThanUnitsOfKindRequirement::Condition::Condition(size_t kind, int count) :
		_unitKind(kind), _atMostCount(count), _player(0u)
	{ }

	bool PlayerSpawnedNoMoreThanUnitsOfKindRequirement::Condition::CheckCondition()
	{
		PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
		return (int)playerStats->GetAllUnitsOfKind(_unitKind)->size() <= _atMostCount;
	}

	PlayerSpawnedNoMoreThanUnitsOfKindRequirement::PlayerSpawnedNoMoreThanUnitsOfKindRequirement(
		TechAvailability onTrue, TechAvailability onFalse,
		size_t unitKind, int count) :
	TechRequirement( onTrue, onFalse, xNew2(Condition, unitKind, count) ),
		_onUnitStatsChanged(this)
	{ }

	PlayerSpawnedNoMoreThanUnitsOfKindRequirement::~PlayerSpawnedNoMoreThanUnitsOfKindRequirement()
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitSpawnedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}
	}

	void PlayerSpawnedNoMoreThanUnitsOfKindRequirement::SetPlayer(uint8 player)
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitSpawnedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}

		TechRequirement::SetPlayer(player);
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitSpawnedEvent() += &_onUnitStatsChanged;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitKilledEvent() += &_onUnitStatsChanged;
	}

	void PlayerSpawnedNoMoreThanUnitsOfKindRequirement::UnitStatsChanged(Unit* unit, uint8 player)
	{
		if(_player == player &&
			unit->GetUnitKind() == static_cast<Condition*>(_condition)->_unitKind )
		{
			CheckRequirement();
		}
	}

	TechRequirement* PlayerSpawnedNoMoreThanUnitsOfKindRequirement::GetCopy()
	{
		return xNew4(PlayerSpawnedNoMoreThanUnitsOfKindRequirement, 
			_stateOnTrue, _stateOnFalse,
			static_cast<Condition*>(_condition)->_unitKind, 
			static_cast<Condition*>(_condition)->_atMostCount);
	}

	PlayerSpawnedNoMoreThanUnitsOfKindRequirement::Factory::Factory() :
		IFactoryCast("TechRequirement")
	{ }

	IGameObject* PlayerSpawnedNoMoreThanUnitsOfKindRequirement::Factory::Create(XmlNode* node)
	{
		return CreateFromXml<PlayerSpawnedNoMoreThanUnitsOfKindRequirement>(node);
	}



	PlayerSpawnedExaclyUnitsOfKindRequirement::Condition::Condition(size_t kind, int count) :
		_unitKind(kind), _count(count), _player(0u)
	{ }

	bool PlayerSpawnedExaclyUnitsOfKindRequirement::Condition::CheckCondition()
	{
		PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
		return playerStats->GetAllUnitsOfKind(_unitKind)->size() == _count;
	}

	PlayerSpawnedExaclyUnitsOfKindRequirement::PlayerSpawnedExaclyUnitsOfKindRequirement(
		TechAvailability onTrue, TechAvailability onFalse,
		size_t unitKind, int count) :
	TechRequirement( onTrue, onFalse, xNew2(Condition, unitKind, count) ),
		_onUnitStatsChanged(this)
	{ }

	PlayerSpawnedExaclyUnitsOfKindRequirement::~PlayerSpawnedExaclyUnitsOfKindRequirement()
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitSpawnedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}
	}

	void PlayerSpawnedExaclyUnitsOfKindRequirement::SetPlayer(uint8 player)
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitSpawnedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}

		TechRequirement::SetPlayer(player);
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitSpawnedEvent() += &_onUnitStatsChanged;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitKilledEvent() += &_onUnitStatsChanged;
	}

	void PlayerSpawnedExaclyUnitsOfKindRequirement::UnitStatsChanged(Unit* unit, uint8 player)
	{
		if(_player == player &&
			unit->GetUnitKind() == static_cast<Condition*>(_condition)->_unitKind )
		{
			CheckRequirement();
		}
	}

	TechRequirement* PlayerSpawnedExaclyUnitsOfKindRequirement::GetCopy()
	{
		return xNew4(PlayerSpawnedExaclyUnitsOfKindRequirement, 
			_stateOnTrue, _stateOnFalse,
			static_cast<Condition*>(_condition)->_unitKind, 
			static_cast<Condition*>(_condition)->_count);
	}

	PlayerSpawnedExaclyUnitsOfKindRequirement::Factory::Factory() :
		IFactoryCast("TechRequirement")
	{ }

	IGameObject* PlayerSpawnedExaclyUnitsOfKindRequirement::Factory::Create(XmlNode* node)
	{
		return CreateFromXml<PlayerSpawnedExaclyUnitsOfKindRequirement>(node);
	}

}