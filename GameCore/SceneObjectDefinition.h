#pragma once

#include "ObjectDefinition.h"
#include <Event.h>
#include "SceneObjectProperties.h"
#include <Array.h>
#include "GameObjectFactory.h"

namespace FlagRTS
{
	class SceneObject;


	// Contains all constant properties of SO specific to
	// type of SO ( like constants that applies to all objects of given type )
	// 
	class SceneObjectDefinition : public ObjectDefinition
	{
	protected:
		// Objectclasses defines object available behaviours ( states )
		Array<char*> _objectClasses;
		// Array of child objects to be spawned and attached to this object and objects node
		// They are created / spawned / despawned / destroyed together with this object
		// ( created on LoadResources and destroyed on UnloadResources )
		// Intendet to use for FXs like attach beam-track effect to missle or 
		// objects extensions (auto-turrets?)
		// Object should be set as not Independent in their definitions
		// 2nd and 3rd params are initial offset from this object
		typedef Array<std::pair<SceneObjectDefinition*, std::pair<Vector3, Quaternion>>> ChildObjectDefsArray;
		ChildObjectDefsArray _childObjects;

		// Independents objects have their scene nodes created when spawned. Non-independent
		// objects need not to be spawned, but need to be attached to parent -> their nodes
		// are created when attaching and destroyed when detaching
		bool _isIndependent;
		bool _inhertisScale; // If object inherits scale its model scale will be multiplied by parent's
		int _fogVisibility;

		// Self-event handlers common for every object of this type
		// Copied to SceneObject and called from there
		//
		Event<SceneObject*> _onSpawn; 
		Event<SceneObject*> _onDespawn;
		Event<SceneObject*> _onHoverBegin;
		Event<SceneObject*> _onHoverEnd;

		uint32 _selectionFlags;

		int _minimapFlags;
		string _minimapIconMaterialName;
		size_t _minimapIconHandle; // Pointer to ogre material for icon
		Vector2 _minimapSize; // Size of minimap icon

	public:
		// Sets default values
		SceneObjectDefinition();
		// Reads values from xml node
		SceneObjectDefinition(XmlNode* soDefNode); 

		~SceneObjectDefinition();

		bool IsIndependent() const { return _isIndependent; }
		void SetIsIndependent(bool value) { _isIndependent = value; }
		
		bool InheritsScale() const { return _inhertisScale; }
		void SetInheritsScale(bool value) { _inhertisScale = value; }

		virtual bool HavePhysics() const { return false; }
		virtual bool HaveFootprint() const { return false; }

		Event<SceneObject*>& GetOnSpawnEvent() { return _onSpawn; }
		Event<SceneObject*>& GetOnDespawnEvent() { return _onDespawn; }
		Event<SceneObject*>& GetOnHoverBeginEvent() { return _onHoverBegin; }
		Event<SceneObject*>& GetOnHoverEndEvent() { return _onHoverEnd; }

		Array<char*>& GetObjectClasses() { return _objectClasses; }
		const Array<char*>& GetObjectClasses() const { return _objectClasses; }
		
		ChildObjectDefsArray& GetChildObjects() { return _childObjects; }
		const ChildObjectDefsArray& GetChildObjects() const { return _childObjects; }

		uint32 GetSelectionFlags() const { return _selectionFlags; }
		void SetSelectionFlags(uint32 flags) { _selectionFlags = flags; }
		void AddSelectionFlags(uint32 flags) { _selectionFlags |= flags; }
		void RemoveSelectionFlags(uint32 flags) { _selectionFlags &= ~flags; }
		
		int GetMinimapFlags() const { return _minimapFlags; }
		void SetMinimapFlags(int value) { _minimapFlags = value; }
		
		size_t GetMinimapIconHandle() const { return _minimapIconHandle; }
		void SetMinimapIconHandle(size_t value) { _minimapIconHandle = value; }
		
		const string& GetMinimapIconMaterialName() const { return _minimapIconMaterialName; }
		void SetMinimapIconMaterialName(const string& value) { _minimapIconMaterialName = value; }
		
		const Vector2& GetMinimapSize() const { return _minimapSize; }
		void SetMinimapSize(const Vector2& value) { _minimapSize = value; }

	protected:
		void ParseSOFlags(XmlNode* flagsNode);
		void ParseSOProperties(XmlNode* propsNode);
		void ParseSOClasses(XmlNode* classesNode);
		void ParseSOChildObjects(XmlNode* childrenNode);
		void ParseMinimap(XmlNode* minimapNode);
	};

	class SceneObjectDefinitionFactory : public SubClassXmlFactory
	{
	public:
		SceneObjectDefinitionFactory();
	};
}