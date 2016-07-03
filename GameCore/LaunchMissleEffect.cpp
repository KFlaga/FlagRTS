#include "LaunchMissleEffect.h"
#include "Missle.h"
#include "Unit.h"
#include "GameWorld.h"
#include "Map.h"
#include "MainGameObjectPool.h"

namespace FlagRTS
{
	LaunchMissleEffect::LaunchMissleEffect(const MissleHolder& storage) :
		ILaunchMissleEffect(),
		_missleStorage(storage),
		_spawnOffset(0.f,0.f,0.f),
		_targetOffset(0.f,0.f,0.f),
		_applySpawnOffset(false),
		_applyTargetOffset(false)
	{ 
		SetFinalType(GetTypeId<LaunchMissleEffect>());
		SetFinalTypeName("Effect"); // Type name for all effects is Effect
	}

	LaunchMissleEffect::LaunchMissleEffect(XmlNode* defNode) :
		ILaunchMissleEffect(defNode),
		_spawnOffset(0.f,0.f,0.f),
		_targetOffset(0.f,0.f,0.f),
		_applySpawnOffset(false),
		_applyTargetOffset(false)
	{
		SetFinalType(GetTypeId<LaunchMissleEffect>());
		SetFinalTypeName("Effect"); // Type name for all effects is Effect

		string missleDefName = XmlUtility::XmlGetString(defNode->first_node("Missle"), "definition");

		XmlNode* storageNode = defNode->first_node("MissleHolder");
		int baseSize = XmlUtility::XmlGetInt(storageNode->first_node("BaseSize"), "value");
		int freeMissles = XmlUtility::XmlGetInt(storageNode->first_node("FreeMissles"), "value");
		float cooldown = XmlUtility::XmlGetFloatIfExists(storageNode->first_node("Cooldown"), "value", 30000.f);

		XmlNode* allocNode = storageNode->first_node("AllocStrategy");
		MissleAllocationStrategy allocStrategy = MissleAllocationStrategies::ParseMissleAllocationStrategyType(
			XmlUtility::XmlGetString(allocNode, "resize"), 
			XmlUtility::XmlGetString(allocNode, "cooldown"), 
			XmlUtility::XmlGetString(allocNode, "user_resize"));

		_missleStorage.SetBaseSize(baseSize);
		_missleStorage.SetFreeMissles(freeMissles);
		_missleStorage.SetCooldown(cooldown);
		_missleStorage.SetAllocStartegy(allocStrategy);

		XmlNode* spawnOffsetNode = defNode->first_node("SpawnOffset");
		if(spawnOffsetNode != 0)
		{
			_spawnOffset = XmlUtility::XmlGetXYZ(spawnOffsetNode);
			_applySpawnOffset = true;
		}
		XmlNode* targetOffsetNode = defNode->first_node("TargetOffset");
		if(targetOffsetNode != 0)
		{
			_targetOffset = XmlUtility::XmlGetXYZ(targetOffsetNode);
			_applyTargetOffset = true;
		}

		auto getMissleDefinition = [this, missleDefName]() 
		{
			MissleDefinition* missleDef = static_cast<MissleDefinition*>(
				GameWorld::GlobalWorld->GetSceneObjectDefinition("Missle", missleDefName));
			_missleStorage.SetMissleDefinition(missleDef);
		};
		MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
			xNew1(DelegateEventHandler<decltype(getMissleDefinition)>,getMissleDefinition);
	}

	LaunchMissleEffect::~LaunchMissleEffect()
	{

	}

	void LaunchMissleEffect::AddUser()
	{
		_missleStorage.AddUser();
	}

	void LaunchMissleEffect::RemoveUser()
	{
		_missleStorage.RemoveUser();
	}

	// Gets new Missle from MissleHolder, spawns it on point defined by 'SpawnPosition',
	// and calls Lanuch on it
	// Missle is attached to caster if 'caster' != 0
	// If 'target' == 0, then it is treated as point-target
	// For LaunchMissleEffect 'caster' must be Unit or 0 for trigger-forced effect
	// 'source' is probably Weapon or Command
	// 'target' is PhysicalObject
	void LaunchMissleEffect::ApplyEffect(IGameObject* caster,
		IGameObject* source,
		IGameObject* target,
		const Vector3& targetPoint)
	{
		_ASSERT( caster->GetFinalType() == GetTypeId<Unit>() );
		Unit* casterUnit = static_cast<Unit*>(caster);

		Missle* newMissle = _missleStorage.GetMissle();
		// Set initial position/rotation of missle
		Quaternion spawnOrientation = casterUnit->GetOrientationAbsolute();
		Vector3 spawnPos;
		if( _applySpawnOffset )
			spawnPos = casterUnit->GetPositionAbsolute() + spawnOrientation*_spawnOffset;
		else
			spawnPos = casterUnit->GetPositionAbsolute();
		
		GameWorld::GlobalWorld->GetMap()->GetRootNode()->addChild( newMissle->GetSceneNode() );
		newMissle->SetPosition(spawnPos);
		newMissle->SetOrientation(spawnOrientation);
		newMissle->SetCaster(casterUnit);
		newMissle->SetSource(source);
		newMissle->SetSourceMissleHolder(&_missleStorage);
		// Missle should have scene node created, so we can attach it to Scene
		newMissle->Spawn();

		if( target != 0 )
		{
			if( _applyTargetOffset )
				newMissle->Launch(static_cast<PhysicalObject*>(target), spawnOrientation * _targetOffset);
			else
				newMissle->Launch(static_cast<PhysicalObject*>(target));
		}
		else
		{
			if( _applyTargetOffset )
				newMissle->Launch(targetPoint + spawnOrientation * _targetOffset);
			else
				newMissle->Launch(targetPoint);
		}
	}

	LaunchMissleMultiEffect::LaunchMissleMultiEffect()
	{
		SetFinalType(GetTypeId<LaunchMissleMultiEffect>());
		SetFinalTypeName("Effect");
	}

	LaunchMissleMultiEffect::LaunchMissleMultiEffect(XmlNode* defNode) :
		ILaunchMissleEffect(defNode)
	{
		SetFinalType(GetTypeId<LaunchMissleMultiEffect>());
		SetFinalTypeName("Effect");

		XmlNode* allEffectsNode = defNode->first_node("Effects");
		XmlNode* effectNode = allEffectsNode->first_node("Effect");
		int effCount = 0;
		while(effectNode != 0)
		{
			string effectName = XmlUtility::XmlGetString(effectNode, "name");
			++effCount;

			auto getEffect = [this, effectName]() 
			{
				ILaunchMissleEffect* effect = static_cast<ILaunchMissleEffect*>(
					GameWorld::GlobalWorld->GetGameObjectDefinition("Effect", effectName));
				AddEffect(effect);
			};
			MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
				xNew1(DelegateEventHandler<decltype(getEffect)>, getEffect);

			effectNode = effectNode->next_sibling();
		}
		_effects.reserve(effCount);
	}

	LaunchMissleMultiEffect::~LaunchMissleMultiEffect()
	{

	}
}