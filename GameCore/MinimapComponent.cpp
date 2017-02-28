#include "MinimapComponent.h"
#include "Minimap.h"
#include <OgreMaterialManager.h>
#include "GameWorld.h"
#include "Player.h"

#include "SceneObject.h"

namespace FlagRTS
{
	MinimapComponentDefinition::MinimapComponentDefinition(XmlNode* compNode)
	{
		SetFinalType(GetTypeId<MinimapComponent>());
		SetFinalTypeName("MinimapComponent");

		// <Component type="MinimapComponent">
		//   <Flags>
		//		<RevealedVisible/>
		//		<UseCustomIcon/>
		//		<MinimapSelectable/>
		//   </Flags>
		//   <Size x="80.f" y="160.f"/> <!-- Size of minimap icon -> whole minimap is 1000x1000 * aspect -->
		//   <CustomIcon name="icon.png"/>
		//</Component>

		XmlNode* flagsNode = compNode->first_node("Flags");
		_minimapFlags = 0;
		XmlNode* flagNode = flagsNode->first_node();
		while(flagNode != 0)
		{
			_minimapFlags |= MinimapFlags::ParseMinimapFlags(flagNode->name());
			flagNode = flagNode->next_sibling();
		}

		if( (_minimapFlags & MinimapFlags::UseCustomIcon) != 0 )
		{
			_minimapMaterialName = XmlUtility::XmlGetString(compNode->first_node("CustomIcon"), "name");
			// Get material and set its handle
			try
			{
				auto mat = Ogre::MaterialManager::getSingleton().getByName(_minimapMaterialName, "Materials");
				_minimapIconHandle = reinterpret_cast<size_t>(mat.getPointer());
			}
			catch(Ogre::Exception& e)
			{ 
				_ASSERT(false);
			}
		}

		_minimapSize = XmlUtility::XmlGetXY(compNode->first_node("Size"));
	}

	MinimapComponent::MinimapComponent(ObjectDefinition* def, IGameObject* owner) :
		IObjectComponent(def, owner),
		_minimapFlags(GetDefinition<MinimapComponentDefinition>()->GetMinimapFlags()),
		_billboardMarkerHandle(0),
		_normalBillboard(0),
		_selectedBillboard(0),
		_onObjectSpawned(this),
		_onObjectDespawned(this)
	{
		_ASSERT(dynamic_cast<SceneObject*>(owner) != 0);
		SetFinalType(GetTypeId<MinimapComponent>());
	}

	void MinimapComponent::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		auto players = GameWorld::GlobalWorld->GetPlayers();
		int ownerPlayer = GetOwner()->GetOwner();
		_minimapFlags &= ~(MinimapFlags::OwnerFlags);

		if(ownerPlayer == NEUTRAL_PLAYERNUM)
		{
			_minimapFlags |= MinimapFlags::EnvironmentUnit;
		}
		else if(players->IsOwned(ownerPlayer))
		{
			_minimapFlags |= MinimapFlags::OwnedUnit;
		}
		else if(players->IsAlly(players->GetClientPlayer(), ownerPlayer))
		{
			_minimapFlags |= MinimapFlags::AllyUnit;
		}
		else if(players->IsEnemy(players->GetClientPlayer(),ownerPlayer))
		{
			_minimapFlags |= MinimapFlags::EnemyUnit;
		}
		else if(players->IsNeutral(players->GetClientPlayer(),ownerPlayer))
		{
			_minimapFlags |= MinimapFlags::NeutralUnit;
		}

		GetOwner<SceneObject>()->Spawned() += &_onObjectSpawned;
		GetOwner<SceneObject>()->Spawned() += &_onObjectDespawned;
	}

	void MinimapComponent::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		GetOwner<SceneObject>()->Spawned() -= &_onObjectSpawned;
		GetOwner<SceneObject>()->Spawned() -= &_onObjectDespawned;
	}

	void MinimapComponent::ObjectSpawned(SceneObject* obj)
	{
		GameWorld::GlobalWorld->GetMinimap()->AddObject(
			GetOwner<SceneObject>());
	}

	void MinimapComponent::ObjectDespawned(SceneObject* obj)
	{
		GameWorld::GlobalWorld->GetMinimap()->RemoveObject(
			GetOwner<SceneObject>());
	}
}