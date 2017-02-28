#include "SelectionComponent.h"
#include "SceneMarker.h"
#include "GameWorld.h"
#include "IObjectDefinitionManager.h"
#include "IGameObjectPool.h"
#include "SceneObjectSpawner.h"

namespace FlagRTS
{
	SelectionComponentDefinition::SelectionComponentDefinition(XmlNode* compNode)
	{
		SetFinalType(GetTypeId<SelectionComponent>());
		SetFinalTypeName("SelectionComponent");
		/*
		<Component type="SelectionComponent">

		<IsSelectable value="true"/> <!-- is selectable by default -->

		<SelectionMarker definition="MMarker_SelectionRectMedium" type="ModelMarker">
		<Size x="120" y="1" z="220"/>
		<Position x="0" y="5" z="0"/> 
		</SelectionMarker>

		</Component>
		*/

		XmlNode* markerNode = compNode->first_node("SelectionMarker");
		if(markerNode != 0)
		{
			string markerName = XmlUtility::XmlGetString(markerNode, "definition");
			string markerType = XmlUtility::XmlGetString(markerNode, "type");
			_selectionMarkerSize = XmlUtility::XmlGetXYZ(markerNode->first_node("Size"));
			_selectionMarkerOffset = XmlUtility::XmlGetXYZ(markerNode->first_node("Position"));

			auto getDefinition = [this, markerName, markerType](IObjectDefinitionManager* mgr) 
			{
				_selectionMarkerDef = static_cast<SceneMarkerDefinition*>(
					mgr->GetObjectDefinitionByName(markerType, markerName));
			};
			typedef DelegateEventHandler<decltype(getDefinition), IObjectDefinitionManager*> DefinitionsLoadedHandler;
			GameInterfaces::GetObjectDefinitionManager()->OnAllDefinitionsLoaded() +=
				xNew1(DefinitionsLoadedHandler, getDefinition);
		}
	}

	SelectionComponent::SelectionComponent(ObjectDefinition* def, IGameObject* owner) :
		IObjectComponent(def, owner),
		_onOwnerSpawned(this),
		_onOwnerDespawned(this)
	{
		_ASSERT(dynamic_cast<SceneObject*>(owner) != 0);
		SetFinalType(GetTypeId<SelectionComponent>());

		SceneObject* object = static_cast<SceneObject*>(owner);
		SelectionComponentDefinition* scDef = GetDefinition<SelectionComponentDefinition>();

		object->Spawned() += &_onOwnerSpawned;
		object->Despawned() += &_onOwnerDespawned;
		_isSelectable = scDef->IsSelectable();
		_isSelected = false;
	}


	void SelectionComponent::OnOwnerSpawned(SceneObject* owner)
	{
		//Spawn:
		if( _selectionMarker != 0 )
		{
			// Create node for marker, attach it (it will create child node) + set not visible
			owner->AttachChild(_selectionMarker, true);
			_selectionMarker->Spawn();
			_selectionMarker->GetOgreMovable()->setVisible(false);
			// Set initial values for marker
			_selectionMarker->SetSize(GetDefinition<SelectionComponentDefinition>()->
				GetSelectionMarkerSize());
			_selectionMarker->SetPosition(GetDefinition<SelectionComponentDefinition>()->
				GetSelectionMarkerOffset());
		}
	}

	void SelectionComponent::OnOwnerDespawned(SceneObject* owner)
	{
		if( _selectionMarker != 0 )
		{
			_selectionMarker->Despawn();
			owner->DetachChild(_selectionMarker, true, true);
		}
	}

	void SelectionComponent::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if( GetDefinition<SelectionComponentDefinition>()->GetSelectionMarkerDefinition() != 0 )
		{
			// Create selection marker
			_selectionMarker = static_cast<SceneMarker*>(
				GameInterfaces::GetGameObjectPool()->Create(
				GetDefinition<SelectionComponentDefinition>()->
				GetSelectionMarkerDefinition(), NEUTRAL_PLAYERNUM));

			_selectionMarker->SetOwner(GetOwner()->GetOwner());
		}
	}

	void SelectionComponent::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if( _selectionMarker != 0)
		{
			GameInterfaces::GetSceneObjectSpawner()->
				DestroySceneObject(_selectionMarker);
			_selectionMarker = 0;
		}
	}

	// SetOwner	
	/*
	RemoveSelectionFlags(SelectionFlags::GetPlayerFlag(_owner));
	if(_selectionMarker != 0)
		_selectionMarker->SetOwner(owner);
	AddSelectionFlags(SelectionFlags::GetPlayerFlag(owner));
	*/

	void SelectionComponent::SetIsSelected(bool value)
	{
		if( value != _isSelected )
		{
			_isSelected = (value);
			if(_selectionMarker != 0)
			{
				_selectionMarker->GetOgreMovable()->setVisible(_isSelected);
			}
			_onIsSelectedChanged.Fire(GetOwner<SceneObject>(), value);
		}
	}
}