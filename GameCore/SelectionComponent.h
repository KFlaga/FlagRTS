#pragma once

#include "IObjectComponent.h"
#include "ObjectDefinition.h"
#include <Event.h>

namespace Ogre
{
	class Billboard;
}

namespace FlagRTS
{
	class SceneObject;
	class SceneMarkerDefinition;
	class SceneMarker;

	/**
		Expected layout of node:
		<Component type="MinimapComponent">
		   <Flags>
				<RevealedVisible/>
				<UseCustomIcon/>
				<MinimapSelectable/>
		   </Flags>
		   <Size x="80.f" y="160.f"/> <!-- Size of minimap icon -> whole minimap is 1000x1000 * aspect -->
		   <CustomIcon name="icon.png"/> <!-- Must be provided if flag UseCustomIcon is set -->
		</Component>
	*/
	class SelectionComponentDefinition : public ObjectDefinition
	{
	protected:
		// Event occurs when unit is selected/deselected ( param is true if selected )
		SceneMarkerDefinition* _selectionMarkerDef;
		Vector3 _selectionMarkerSize;
		Vector3 _selectionMarkerOffset;
		bool _isSelectable; // Is it selectable by default?

	public:
		SelectionComponentDefinition(XmlNode* compNode);

		SceneMarkerDefinition* GetSelectionMarkerDefinition() const { return _selectionMarkerDef; }
		void SetSelectionMarkerDefinition(SceneMarkerDefinition* markerDef) { _selectionMarkerDef = markerDef; }
		
		const Vector3& GetSelectionMarkerSize() const { return _selectionMarkerSize; }
		void SetSelectionMarkerSize(const Vector3& value) { _selectionMarkerSize = value; }
		
		const Vector3& GetSelectionMarkerOffset() const { return _selectionMarkerOffset; }
		void SetSelectionMarkerOffset(const Vector3& value) { _selectionMarkerOffset = value; }
		
		bool IsSelectable() const { return _isSelectable; }
		void SetIsSelectable(bool& value) { _isSelectable = value; }
	};

	/**
		Component that allows selection of object.
		If object is selected SceneMarker may be shown around unit (like bbox)
	*/
	class SelectionComponent : public IObjectComponent
	{
		Event<SceneObject*, bool> _onIsSelectedChanged;
		SceneMarker* _selectionMarker; // Marker to show when unit is selected
		int _selectionFlags;
		bool _isSelected;
		bool _isSelectable; 

	public:
		SelectionComponent(ObjectDefinition* def, IGameObject* owner);

		void Update(float ms) { }
		void LoadResources(Ogre::SceneManager* ogreMgr);
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		Event<SceneObject*, bool>& IsSelectedChanged() { return _onIsSelectedChanged; }
		bool IsSelected() { return _isSelected; }
		void SetIsSelected(bool value);

		SceneMarker* GetSelectionMarker() const { _selectionMarker; }
		void SetSelectionMarker(SceneMarker* marker) { _selectionMarker = marker; }

		const Vector3& GetSelectionMarkerSize() const { return GetDefinition<SelectionComponentDefinition>()->GetSelectionMarkerSize(); }
		const Vector3& GetSelectionMarkerOffset() const { return GetDefinition<SelectionComponentDefinition>()->GetSelectionMarkerOffset(); }
	
		int GetSelectionFlags() const { return _selectionFlags; }
		virtual void SetSelectionFlags(int flags) { _selectionFlags = flags; }
		void AddSelectionFlags(int flags) { SetSelectionFlags(_selectionFlags | flags); }
		void RemoveSelectionFlags(int flags) { SetSelectionFlags(_selectionFlags & (~flags)); }
	
		bool IsSelectable() const { return _isSelectable; }
		void SetIsSelectable(bool& value) { _isSelectable = value; }
	protected:
		void OnOwnerSpawned(SceneObject*);
		DEFINE_DELEGATE1(OwnerSpawnedDelegate, SelectionComponent,
			OnOwnerSpawned, SceneObject*);
		OwnerSpawnedDelegate _onOwnerSpawned;
		
		void OnOwnerDespawned(SceneObject*);
		DEFINE_DELEGATE1(OwnerDespawnedDelegate, SelectionComponent,
			OnOwnerDespawned, SceneObject*);
		OwnerDespawnedDelegate _onOwnerDespawned;
	};
}