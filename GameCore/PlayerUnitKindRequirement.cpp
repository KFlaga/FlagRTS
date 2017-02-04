#include "PlayerUnitKindRequirement.h"
#include "GameWorld.h"
#include "Unit.h"
#include "PlayerStatsManager.h"
#include "IObjectDefinitionManager.h"

namespace FlagRTS
{
	template<typename ReqT>
	ReqT* CreateFromXml(XmlNode* node)
	{
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


	PlayerUnitKindRequirement::PlayerUnitKindRequirement(XmlNode* reqNode) :
		TechRequirement(reqNode),
		_onUnitStatsChanged(this)
	{
		string unitName = XmlUtility::XmlGetString(reqNode, "unit_name");
		int count = XmlUtility::XmlGetInt(reqNode, "count");

		auto getUnitKind = [unitName, this](IObjectDefinitionManager* mgr) 
		{	
			size_t unitKind = static_cast<UnitDefinition*>(
				mgr->GetObjectDefinitionByName(unitName, "Unit"))->GetUnitKind();
			this->SetUnitKind(unitKind);
		};
		typedef DelegateEventHandler<decltype(getUnitKind), IObjectDefinitionManager*> DefinitionsLoadedHandler;	
		GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
			xNew1(DefinitionsLoadedHandler,getUnitKind);
	}

	PlayerUnitKindRequirement::PlayerUnitKindRequirement(TechAvailability onTrue, TechAvailability onFalse, PlayerUnitKindCondition* condition) :
		TechRequirement(onTrue, onFalse, condition),
		_onUnitStatsChanged(this)
	{

	}

	PlayerUnitKindRequirement::~PlayerUnitKindRequirement()
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}
	}

	void PlayerUnitKindRequirement::SetPlayer(uint8 player)
	{
		if(_player != _noPlayer)
		{
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitConstructedEvent() -= &_onUnitStatsChanged;
			GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
				GetUnitKilledEvent() -= &_onUnitStatsChanged;
		}

		TechRequirement::SetPlayer(player);
		static_cast<PlayerUnitKindCondition*>(_condition)->_player = player;

		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitConstructedEvent() += &_onUnitStatsChanged;
		GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player)->
			GetUnitKilledEvent() += &_onUnitStatsChanged;
	}

	void PlayerUnitKindRequirement::SetUnitKind(size_t uk)
	{
		static_cast<PlayerUnitKindCondition*>(_condition)->_unitKind = uk;
	}

	void PlayerUnitKindRequirement::SetUnitCount(int count)
	{
		static_cast<PlayerUnitKindCondition*>(_condition)->_unitCount = count;
	}
	
	size_t PlayerUnitKindRequirement::GetUnitKind() const
	{
		return static_cast<PlayerUnitKindCondition*>(_condition)->_unitCount;
	}
	
	int PlayerUnitKindRequirement::GetUnitCount() const
	{
		return static_cast<PlayerUnitKindCondition*>(_condition)->_unitCount;
	}

	void PlayerUnitKindRequirement::UnitStatsChanged(Unit* unit, uint8 player)
	{
		if(_player == player &&
			unit->GetUnitKind() == static_cast<PlayerUnitKindCondition*>(_condition)->_unitKind )
		{
			CheckRequirement();
		}
	}

#define DEFINE_PlayerUnitKindRequirement(ClassName, ConditionFunction) \
	class ClassName##Condition : public PlayerUnitKindRequirement::PlayerUnitKindCondition \
	{ \
	public: \
		bool CheckCondition() \
		{ \
			ConditionFunction \
		} \
		ICondition* GetCopy() \
		{ \
			return xNew1(ClassName##Condition, *this); \
		} \
	}; \
	ClassName::ClassName(XmlNode* reqNode) : \
		PlayerUnitKindRequirement(reqNode) { } \
	ClassName::ClassName(TechAvailability onTrue, TechAvailability onFalse) : \
		PlayerUnitKindRequirement(onTrue, onFalse, xNew0(ClassName##Condition)) { } \
	TechRequirement* ClassName::GetCopy() \
	{ \
		ClassName* req = xNew2(ClassName, _stateOnTrue, _stateOnFalse); \
		req->SetPlayer(GetPlayer()); \
		req->SetUnitKind(GetUnitKind()); \
		req->SetUnitCount(GetUnitCount()); \
		return xNew2(ClassName, _stateOnTrue, _stateOnFalse); \
	}

	DEFINE_PlayerUnitKindRequirement(PlayerHaveAtLeastUnitsOfKindRequirement, 
		{
			PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
			return playerStats->GetUnitKindCount(_unitKind) >= _unitCount;
		}
	);

	DEFINE_PlayerUnitKindRequirement(PlayerHaveNoMoreThanUnitsOfKindRequirement, 
		{
			PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
			return playerStats->GetUnitKindCount(_unitKind) <= _unitCount;
		}
	);

	DEFINE_PlayerUnitKindRequirement(PlayerHaveExaclyUnitsOfKindRequirement, 
		{
			PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
			return playerStats->GetUnitKindCount(_unitKind) == _unitCount;
		}
	);

	DEFINE_PlayerUnitKindRequirement(PlayerSpawnedAtLeastUnitsOfKindRequirement, 
		{
			PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
			return playerStats->GetAllUnitsOfKind(_unitKind)->size() >= _unitCount;
		}
	);

	DEFINE_PlayerUnitKindRequirement(PlayerSpawnedNoMoreThanUnitsOfKindRequirement, 
		{
			PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
			return playerStats->GetAllUnitsOfKind(_unitKind)->size() <= _unitCount;
		}
	);

	DEFINE_PlayerUnitKindRequirement(PlayerSpawnedExaclyUnitsOfKindRequirement, 
		{
			PlayerStatsManager* playerStats = GameWorld::GlobalWorld->GetPlayerStatsManager()->GetPlayerStats(_player);
			return playerStats->GetAllUnitsOfKind(_unitKind)->size() == _unitCount;
		}
	);
}