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

	/**
		Definition of minimap component.
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
	class MinimapComponentDefinition : public ObjectDefinition
	{
	protected:
		Vector2 _minimapSize; /** Size of object's icon on minimap in minimap units */
		string _minimapMaterialName; /** Name of Ogre::Material resource for custom icon */
		size_t _minimapIconHandle; /** Handle to Ogre::Material resource for custom icon */
		int _minimapFlags; /** Default set of MinimapFlags for this object */

	public:
		MinimapComponentDefinition(XmlNode* compNode);
		
		int GetMinimapFlags() const { return _minimapFlags; }
		void SetMinimapFlags(int value) { _minimapFlags = value; }
		
		size_t GetMinimapIconHandle() const { return _minimapIconHandle; }
		void SetMinimapIconHandle(size_t value) { _minimapIconHandle = value; }
		
		const string& GetMinimapIconMaterialName() const { return _minimapMaterialName; }
		void SetMinimapIconMaterialName(const string& value) { _minimapMaterialName = value; }
		
		const Vector2& GetMinimapSize() const { return _minimapSize; }
		void SetMinimapSize(const Vector2& value) { _minimapSize = value; }
	};

	/**
		Component responsible for attaching object to minimap.
		On LoadResources() adds object to Minimap and removes it on UnloadResources().
		Owner must be a SceneObject.
	*/
	class MinimapComponent : public IObjectComponent
	{
		Ogre::Billboard* _normalBillboard; /** Billboard with object's icon */
		Ogre::Billboard* _selectedBillboard; /** Billboard with object's icon in selected state */
		size_t _billboardMarkerHandle; // Current BillMarker id
		int _minimapFlags; /** Instance MinimapFlags (may differ from definition's flags) */
	
	public:
		MinimapComponent(ObjectDefinition* def, IGameObject* owner);

		/**
			Adds object to Minimap.
			Sets ownership MinimapFlag.
		*/
		void LoadResources(Ogre::SceneManager* ogreMgr);
		
		/**
			Removes object from Minimap
		*/
		void UnloadResources(Ogre::SceneManager* ogreMgr);
		
		void Update(float ms) { }

		Ogre::Billboard* GetBillboard() const { return _normalBillboard; }
		void SetBillboard(Ogre::Billboard* billboard) { _normalBillboard = billboard; }
		
		Ogre::Billboard* GetSelectedBillboard() const { return _selectedBillboard; }
		void SetSelectedBillboard(Ogre::Billboard* billboard) { _selectedBillboard = billboard; }

		size_t GetBillboardMarkerHandle() const { return _billboardMarkerHandle; }
		void SetBillboardMarkerHandle(size_t handle) { _billboardMarkerHandle = handle; }
		
		int GetMinimapFlags() const { return _minimapFlags; }
		void SetMinimapFlags(int value) { _minimapFlags = value; }
		
		size_t GetMinimapIconHandle() const 
		{ 
			return GetDefinition<MinimapComponentDefinition>()->GetMinimapIconHandle(); 
		}
		
		const string& GetMinimapIconMaterialName() const 
		{ 
			return GetDefinition<MinimapComponentDefinition>()->GetMinimapIconMaterialName(); 
		}
		
		const Vector2& GetMinimapSize() const 
		{ 
			return GetDefinition<MinimapComponentDefinition>()->GetMinimapSize(); 
		}

	protected:
		void ObjectSpawned(SceneObject* obj);
		DEFINE_DELEGATE1(ObjectSpawnedDelegate, MinimapComponent,
			ObjectSpawned, SceneObject*);
		ObjectSpawnedDelegate _onObjectSpawned;
		
		void ObjectDespawned(SceneObject* obj);
		DEFINE_DELEGATE1(ObjectDespawnedDelegate, MinimapComponent,
			ObjectDespawned, SceneObject*);
		ObjectDespawnedDelegate _onObjectDespawned;
	};
}