#include "TrainingComponent.h"
#include "Unit.h"
#include "GameWorld.h"
#include "Resources.h"
#include "IGameObjectPool.h"
#include "IObjectDefinitionManager.h"
#include "ISceneObjectSpawner.h"
#include "NoticeMessages.h"
#include "TrainingStates.h"
#include "AnimationController.h"

namespace FlagRTS
{
	struct SpawnBehaviors
	{
		static bool TrySpawnTrainedUnit_SpawnOutsideFar(TrainingComponent* component, Unit* trainer, Unit* trainee);
		static bool TrySpawnTrainedUnit_SpawnOutsideNear(TrainingComponent* component, Unit* trainer, Unit* trainee);
		static bool TrySpawnTrainedUnit_SpawnInside(TrainingComponent* component, Unit* trainer, Unit* trainee);		
	};

	TrainingComponentDefinition::TrainingComponentDefinition(XmlNode* compNode)
	{
		/*
		<Component type="TrainingComponent">
		<Unit definition="Transporter" id="0"/> 
		<!-- num="x" means trained-command is bind to train{x} command -->

		<PaymentMethod value="PaidInAdvance"/>
		<SpawnBehavior value="SpawnOutsideFar"/>
		<RefundMethod on_cancel="FullRefund" on_died="NoRefund"/>
		<StartTrainMethod value="StartTrainingOnPrepare"/>
		</Component>
		*/

		SetFinalType(GetTypeId<TrainingComponent>());
		SetFinalTypeName("TrainingComponent");

		XmlNode* unitNode = compNode->first_node("Unit");
		while(unitNode != 0)
		{
			const char* defName = CopyChar(XmlUtility::XmlGetString(unitNode, "definition"));
			int num = XmlUtility::XmlGetInt(unitNode, "num");

			TrainableUnit unit;
			unit.Number = num;
			int index = _trainableUnitsDefs.size();

			auto getDefinition = [defName, index, this] (IObjectDefinitionManager* mgr)
			{
				ObjectDefinition* def = mgr->GetObjectDefinitionByName(defName, "Unit");
				this->GetTrainableUnitsArray()[index].Definition = static_cast<UnitDefinition*>(def);
			};
			typedef DelegateEventHandler<decltype(getDefinition), IObjectDefinitionManager*> DefinitionsLoadedHandler;
			GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() += 
				xNew1(DefinitionsLoadedHandler, getDefinition);


			XmlNode* allReqsNode = unitNode->first_node("Requirements");
			if( allReqsNode != 0 )
			{
				XmlNode* reqNode = allReqsNode->first_node("Requirement");
				while( reqNode != 0 )
				{
					unit.RequirementNames.push_back(CopyChar(
						XmlUtility::XmlGetString(reqNode, "name")));
				}
			}

			_trainableUnitsDefs.push_back(unit);
			unitNode = unitNode->next_sibling();
		}
	}

	TrainingComponentDefinition::~TrainingComponentDefinition()
	{
		//Definieion delete
		for(unsigned int i = 0; i < _trainableUnitsDefs.size(); ++i)
		{
			for(unsigned int req = 0; req < _trainableUnitsDefs[i].RequirementNames.size(); ++req)
			{
				xDelete( _trainableUnitsDefs[i].RequirementNames[req] );
			}
		}
	}

	TrainingComponent::TrainingComponent(ObjectDefinition* def, IGameObject* owner) :
		IObjectComponent(def, owner),
		_currentUnit(0),
		_maxSize(5),
		_paused(false),
		_loopQueue(false)
	{
		_ASSERT(dynamic_cast<Unit*>(owner) != 0);

		TrainingComponentDefinition* compDef = GetDefinition<TrainingComponentDefinition>();
		SetSpawnBehavior(compDef->GetSpawnBehavior());

		UpdateInternal = &TrainingComponent::Update_Paused;
	}

	TrainingComponent::~TrainingComponent()
	{
		_ASSERT( _currentUnit == 0 );
	}

	void TrainingComponent::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		// Add states to unit
		GetOwner<Unit>()->GetStates().AddState(xNew1(TrainingState, GetOwner<Unit>()));
		GetOwner<Unit>()->GetStates().AddState(xNew1(TrainingPrepareState, GetOwner<Unit>()));
		GetOwner<Unit>()->GetStates().AddState(xNew1(TrainingFinishedState, GetOwner<Unit>()));

		_isReadyToTrainNext = true;
		UpdateInternal = &TrainingComponent::Update_WaitForNext;
	}

	void TrainingComponent::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		UpdateInternal = &TrainingComponent::Update_Paused;

		if(_currentUnit != 0)
		{
			// Unit was still training when it was removed -> abort training
			Clear();
		}
	}

	void TrainingComponent::Update_Paused(float ms)
	{

	}

	void TrainingComponent::Update_WaitForNext(float ms)
	{
		if(_queue.size() > 0 && _isReadyToTrainNext)
		{
			// Wait for resources for next unit
			int lackingResource = TryTakeResourcesForStartTraining(_queue[0]);
			if(lackingResource >= 0)
				return; // Have no resources

			// We have enough resources, so push next states
			GetOwner<Unit>()->GetStates().PushState(SceneObjectStates::TrainingPrepare);
			GetOwner<Unit>()->GetStates().PushState(SceneObjectStates::Training);

			// create new unit and set is as currently training
			_currentUnit = static_cast<Unit*>(GameInterfaces::GetGameObjectPool()->
				Create(_queue[0], _owner->GetOwner()));
			_currentUnit->SetConstructionProgress(0.f);
			_queue.removeAt(0);

			UpdateInternal = &TrainingComponent::Update_WaitForProgress;
		}
	}

	void TrainingComponent::Update_WaitForProgress(float ms)
	{
		if( _startTrainingProgress )
		{
			UpdateInternal = &TrainingComponent::Update_IsTraining;
		}
	}

	void TrainingComponent::Update_IsTraining(float ms)
	{
		float trainTime = _currentUnit->GetCurrentStats()->BuildTime;
		float tick = ms / trainTime;

		int lackingResource = TryTakeResourcesForTick(_currentUnit, tick);
		if(lackingResource >= 0)
			return; // Have no resources

		// Update progress
		_currentUnit->SetConstructionProgress(
			_currentUnit->GetConstructionProgress() + tick);

		if( _currentUnit->GetConstructionProgress() >= 1.f )
		{
			UpdateInternal = &TrainingComponent::Update_TriesToSpawn;
			Update(ms);
		}
	}

	void TrainingComponent::Update_TriesToSpawn(float ms)
	{
		if( TrySpawnTrainedUnit( this, GetOwner<Unit>(), _currentUnit ) )
		{
			GetOwner<Unit>()->GetStates().PushState(SceneObjectStates::TrainingFinished);

			UpdateInternal = &TrainingComponent::Update_WaitForNext;
			_onUnitTrained.Fire(GetOwner<Unit>(), _currentUnit);
		}
	}

	void TrainingComponent::AddToQueue(UnitDefinition* definition)
	{
		if( GetQueueSize() >= GetMaxSize() )
		{
			GameInterfaces::GetNoticeMessageSender()->ShowNotice(NoticeMessage(
				Notices::GameError, (string("Queue full")).c_str(), _owner->GetOwner()));
			return;
		}

		int lackingResource = TryTakeResourcesForQueue(definition);
		if(lackingResource >= 0) 
		{
			// Have no resources
			GameInterfaces::GetNoticeMessageSender()->ShowNotice(NoticeMessage(
				Notices::GameError, (string("Insuficient resource: ") + 
				GameWorld::GlobalWorld->GetResources()->
				GetResourceName(lackingResource)).c_str(),
				_owner->GetOwner()));
			return; 
		}

		_queue.push_back(definition);
		_onAddedToQueue.Fire(GetOwner<Unit>(), definition);
	}

	int TrainingComponent::TryTakeResourcesForStartTraining(UnitDefinition* unit)
	{
		// For wait for resources take all resources
		// For paid in advance take nothing
		// For other take supply only

		PaymentMethod method = GetPaymentMethod();
		if(method == PaymentMethod::PaidInAdvance)
			return -1;

		Resources* res = GameWorld::GlobalWorld->GetResources();
		auto& cost = unit->GetStats()->Cost;

		for(int i = 0; i < cost.size(); +i)
		{
			// For each non-supply resource check if player have enough of it
			if((res->IsSupplyResource(i) == true || method == PaymentMethod::WaitForResources) &&
				cost[i].ResAmount > res->GetPlayerResource(_owner->GetOwner(), i).Amount)
			{
				return i; // Return lacking resource
			}
		}

		// Player have sufficient resources, so take them
		for(int i = 0; i < cost.size(); +i)
		{
			if(res->IsSupplyResource(i) == true || method == PaymentMethod::WaitForResources)
			{
				res->TakeResourceFromPlayer(_owner->GetOwner(), i, cost[i].ResAmount);
			}
		}

		return -1;
	}

	int TrainingComponent::TryTakeResourcesForTick(Unit* unit, float tickPart)
	{
		if(GetPaymentMethod() != PaymentMethod::PaidContinually)
			return -1;

		// Take 'tickPart' part of unit's resource costs excluding supply type resources
		Resources* res = GameWorld::GlobalWorld->GetResources();
		auto& cost = unit->GetCurrentStats()->Cost;

		for(int i = 0; i < cost.size(); +i)
		{
			// For each non-supply resource check if player have enough of it
			if(res->IsSupplyResource(i) == false &&
				cost[i].ResAmount * tickPart > res->GetPlayerResource(_owner->GetOwner(), i).Amount)
			{
				return i; // Return lacking resource
			}
		}

		// Player have sufficient resources, so take them
		for(int i = 0; i < cost.size(); +i)
		{
			if(res->IsSupplyResource(i) == false)
			{
				res->TakeResourceFromPlayer(_owner->GetOwner(), i, cost[i].ResAmount * tickPart);
			}
		}

		return -1;
	}

	int TrainingComponent::TryTakeResourcesForQueue(UnitDefinition* unit)
	{
		// Take/check resources only if paid in advance
		PaymentMethod method = GetPaymentMethod();
		if(method != PaymentMethod::PaidInAdvance_NoSupply ||
			method != PaymentMethod::PaidInAdvance)
			return -1;

		Resources* res = GameWorld::GlobalWorld->GetResources();
		auto& cost = unit->GetStats()->Cost;

		for(int i = 0; i < cost.size(); +i)
		{
			if(((method == PaymentMethod::PaidInAdvance_NoSupply &&
				res->IsSupplyResource(i) == false) || 
				method == PaymentMethod::PaidInAdvance) &&
				cost[i].ResAmount > res->GetPlayerResource(_owner->GetOwner(), i).Amount)
			{
				return i;
			}
		}

		// Player have sufficient resources, so take them
		for(int i = 0; i < cost.size(); +i)
		{
			if((GetPaymentMethod() == PaymentMethod::PaidInAdvance_NoSupply &&
				res->IsSupplyResource(i) == false) || 
				GetPaymentMethod() == PaymentMethod::PaidInAdvance)
			{
				res->TakeResourceFromPlayer(_owner->GetOwner(), i, cost[i].ResAmount);
			}
		}
	}

	void TrainingComponent::RemoveFirstOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent)
	{
		if( _currentUnit == 0 && _queue.size() == 0)
			return; // No units to remove

		// If we can remove trained unit check for it first
		if( removeCurrent && (definition->GetUnitKind() == _currentUnit->GetUnitKind()))
		{
			AbortTraining();
			return;
		}

		for(int pos = 0; pos < _queue.size(); ++pos)
		{
			if( definition->GetUnitKind() == _queue[pos]->GetUnitKind())
			{
				RemoveAt(pos);
			}
		}
	}

	void TrainingComponent::RemoveLastOfTypeFromQueue(UnitDefinition* definition, bool removeCurrent)
	{
		if( _currentUnit == 0 && _queue.size() == 0)
			return; // No units to remove

		for(int pos = _queue.size() - 1; pos >= 0; --pos)
		{
			if( definition->GetUnitKind() == _queue[pos]->GetUnitKind())
			{
				RemoveAt(pos);
			}
		}

		// If we can remove trained unit check for it at the end
		if( removeCurrent && (definition->GetUnitKind() == _currentUnit->GetUnitKind()))
		{
			AbortTraining();
		}
	}

	void TrainingComponent::RemoveAt(int pos)
	{
		_ASSERT( pos < _queue.size() );

		RefundResourcesForQueued(_queue[pos]);

		UnitDefinition* def = _queue[pos];
		_queue.removeAt(pos);
		_onQueuedUnitRemoved.Fire(GetOwner<Unit>(), def, pos);
	}

	void TrainingComponent::Clear(bool removeCurrent)
	{
		for(int pos = 0; pos < _queue.size(); ++pos)
		{
			RefundResourcesForQueued(_queue[pos]);
		}
		_queue.clear();

		if(removeCurrent)
		{
			AbortTraining();
		}
	}

	void TrainingComponent::Pause()
	{
		if(!_paused)
		{
			_paused = true;
			UpdateInternal_Paused = UpdateInternal;
			UpdateInternal = &TrainingComponent::Update_Paused;
		}
	}

	void TrainingComponent::Resume()
	{
		if(_paused)
		{
			_paused = false;
			UpdateInternal = UpdateInternal_Paused;
		}
	}

	void TrainingComponent::AbortTraining(bool ownerDied)
	{
		// Destory current unit, return resources
		if( _currentUnit != 0 )
		{
			GameWorld::GlobalWorld->GetResources()->ReturnAllResourcesToPlayer(
				_owner->GetOwner(), _currentUnit->GetCurrentStats()->Cost);

			UnitDefinition* def = _currentUnit->GetUnitDefinition();
			GameInterfaces::GetSceneObjectSpawner()->DestroySceneObject(_currentUnit);
			_currentUnit = 0;
			_onQueuedUnitRemoved.Fire(GetOwner<Unit>(), def, 0);

			RefundMethod refund = ownerDied ? GetRefundMethodOnDeath() : GetRefundMethodOnCancel();

			// !!!! Refund supply-type resources !!!!
			// Refund non-supply resources
			if(refund == RefundMethod::FullRefund)
			{
				if(GetPaymentMethod() == PaymentMethod::PaidContinually)
				{
					// Refund all non-supply resources used as far
				}
				else
				{
					// Refund all non-supply resources
				}
			}
			else if(refund == RefundMethod::PartialRefund)
			{
				if(GetPaymentMethod() == PaymentMethod::PaidContinually)
				{
					// Refund part of all non-supply resources used as far
				}
				else
				{
					// Refund part of all non-supply resources
				}
			}

			if(_paused)
			{
				UpdateInternal = &TrainingComponent::Update_WaitForNext;
			}
			else
			{
				UpdateInternal_Paused = &TrainingComponent::Update_WaitForNext;
			}
		}
	}

	void TrainingComponent::RefundResourcesForQueued(UnitDefinition* unit)
	{
		Resources* res = GameWorld::GlobalWorld->GetResources();
		auto& cost = unit->GetStats()->Cost;

		for(int i = 0; i < cost.size(); +i)
		{
			if((GetPaymentMethod() == PaymentMethod::PaidInAdvance_NoSupply &&
				res->IsSupplyResource(i) == false) || 
				GetPaymentMethod() == PaymentMethod::PaidInAdvance)
			{
				res->AddResourceToPlayer(_owner->GetOwner(), i, cost[i].ResAmount);
			}
		}
	}

	void TrainingComponent::SetSpawnBehavior(SpawnBehavior method)
	{
		switch(method)
		{
		case SpawnBehavior::SpawnInside :
			TrySpawnTrainedUnit = SpawnBehaviors::TrySpawnTrainedUnit_SpawnInside;
			break;
		case SpawnBehavior::SpawnOutsideNear :
			TrySpawnTrainedUnit = SpawnBehaviors::TrySpawnTrainedUnit_SpawnOutsideNear;
			break;
		case SpawnBehavior::SpawnOutsideFar :
		default:
			TrySpawnTrainedUnit = SpawnBehaviors::TrySpawnTrainedUnit_SpawnOutsideFar;
		};
	}

	TrainingFinishedState::TrainingFinishedState(Unit* owner) :
		UnitState(owner, SceneObjectStates::TrainingFinished, "TrainingFinished")
	{

	}

	void TrainingFinishedState::Begin()
	{
		TrainingComponent* comp = _owner->FindComponent<TrainingComponent>();
		TrainingComponent::StartTrainMethod method = comp->GetStartTrainMethod();

		if(method == TrainingComponent::StartTrainingOnFinished)
		{
			comp->SetIsReadyToTrainNext(true);
		}

		Animation* anim = _owner->GetAnimController().FindAnimation(
			SceneObjectStates::Training, "Finished");

		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(anim, false);
			_status = StateStatus::RunningCritical;
		}
		else
		{
			_status = StateStatus::Finished;
		}
	}

	void TrainingFinishedState::End()
	{
		_status = StateStatus::Ready;

		TrainingComponent* comp = _owner->FindComponent<TrainingComponent>();
		TrainingComponent::StartTrainMethod method = comp->GetStartTrainMethod();

		if(method == TrainingComponent::StartTrainingOnFinished)
		{
			comp->SetIsStartTrainingProgress(true);
		}
		else
		{
			comp->SetIsReadyToTrainNext(true);
		}

		_owner->GetAnimController().EndAllAnimations();
	}

	void TrainingFinishedState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void TrainingFinishedState::Resume()
	{
		Animation* anim = _owner->GetAnimController().FindAnimation(
			SceneObjectStates::Training, "Finished");

		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(anim, false);
			_status = StateStatus::RunningCritical;
		}
		else
		{
			_status = StateStatus::Finished;
		}
	}

	void TrainingFinishedState::Update(float ms)
	{
		UnitState::Update(ms);
		if(StateStatuses::IsRunning(_status))
		{
			if(_owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished())
			{
				_status == StateStatus::Finished;
			}
		}
	}

	TrainingPrepareState::TrainingPrepareState(Unit* owner) :
		UnitState(owner, SceneObjectStates::TrainingPrepare, "TrainingPrepare")
	{

	}

	void TrainingPrepareState::Begin()
	{
		TrainingComponent* comp = _owner->FindComponent<TrainingComponent>();
		TrainingComponent::StartTrainMethod method = comp->GetStartTrainMethod();

		if(method == TrainingComponent::StartTrainingOnPrepare)
		{
			comp->SetIsStartTrainingProgress(true);
		}

		Animation* anim = _owner->GetAnimController().FindAnimation(
			SceneObjectStates::Training, "Prepare");

		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(anim, false);
			_status = StateStatus::RunningCritical;
		}
		else
		{
			_status = StateStatus::Finished;
		}
	}

	void TrainingPrepareState::End()
	{
		_status = StateStatus::Ready;
		_owner->GetAnimController().EndAllAnimations();
	}

	
	void TrainingPrepareState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void TrainingPrepareState::Resume()
	{
		Animation* anim = _owner->GetAnimController().FindAnimation(
			SceneObjectStates::Training, "Prepare");

		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(anim, false);
			_status = StateStatus::RunningCritical;
		}
		else
		{
			_status = StateStatus::Finished;
		}
	}

	void TrainingPrepareState::Update(float ms)
	{
		UnitState::Update(ms);
		if(StateStatuses::IsRunning(_status))
		{
			if(_owner->GetAnimController().GetCurrentAnimations()[0]->CheckIsFinished())
			{
				_status == StateStatus::Finished;
			}
		}
	}

	TrainingState::TrainingState(Unit* owner) :
		UnitState(owner, SceneObjectStates::Training, "Training")
	{

	}

	void TrainingState::Begin()
	{
		TrainingComponent* comp = _owner->FindComponent<TrainingComponent>();
		TrainingComponent::StartTrainMethod method = comp->GetStartTrainMethod();

		if(method == TrainingComponent::StartTrainingOnTraining)
		{
			comp->SetIsStartTrainingProgress(true);
		}

		Animation* anim = _owner->GetAnimController().FindAnimation(
			SceneObjectStates::Training, "Prepare");

		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(anim, false);
			_status = StateStatus::RunningNoncritical;
		}
		else
		{
			_status = StateStatus::Finished;
		}
	}

	void TrainingState::End()
	{
		_status = StateStatus::Ready;
		_owner->GetAnimController().EndAllAnimations();
	}

	void TrainingState::Interrupt()
	{
		_status = StateStatus::Paused;
		_owner->GetAnimController().EndAllAnimations();
	}

	void TrainingState::Resume()
	{
		Animation* anim = _owner->GetAnimController().FindAnimation(
			SceneObjectStates::Training, "Prepare");

		if(anim != 0)
		{
			_owner->GetAnimController().ChangeAnimation(anim, false);
			_status = StateStatus::RunningNoncritical;
		}
		else
		{
			_status = StateStatus::Finished;
		}
	}

	void TrainingState::Update(float ms)
	{
		UnitState::Update(ms);
	}

	bool SpawnBehaviors::TrySpawnTrainedUnit_SpawnOutsideFar(
		TrainingComponent* component, Unit* trainer, Unit* trainee)
	{

	}

	bool SpawnBehaviors::TrySpawnTrainedUnit_SpawnOutsideNear(
		TrainingComponent* component, Unit* trainer, Unit* trainee)
	{

	}

	bool SpawnBehaviors::TrySpawnTrainedUnit_SpawnInside(
		TrainingComponent* component, Unit* trainer, Unit* trainee)
	{

	}
}