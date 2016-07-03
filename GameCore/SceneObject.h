#pragma once

#include "SceneObjectDefinition.h"

#include <OgreSceneNode.h>
#include "SceneObjectStateMachine.h"

#include <Array.h>

namespace FlagRTS
{
	// Created via SceneObjectFactory
	class SceneObject : public IGameObject
	{
	protected:
		SceneObjectDefinition* _definition;
		SceneNode* _sceneNode;				
		SceneObjectStateMachine _stateMachine;

		typedef Array<std::pair<SceneObject*, std::pair<Vector3, Quaternion>>> ChildObjectSpawnArray;
		// Spawn array conatins objects that are spawned together with this object and are
		// created based on info on child objects from definition
		// They shouldn't be detached from this object if object is to be spawned multiple times
		// Those objects are also stored in child objects array
		ChildObjectSpawnArray _spawnObjects;

		Array<SceneObject*> _childObjects;
		SceneObject* _parentObject;

		Event<SceneObject*> _onSpawn;
		Event<SceneObject*> _onDespawn;
		Event<SceneObject*> _onHoverBegin;
		Event<SceneObject*> _onHoverEnd;

		Event<SceneObject*> _onMoved;
		Event<SceneObject*> _onRotated;

		uint32 _selectionFlags; // Flags used for scene queries to determine if object should accept hover/selection
		unsigned int _owner; // Owning player ( for objects that have owener )

		bool _loaded;
		bool _spawned;
		
		size_t _objectSpecificDataHandle; // Some additional data specific to the kind of object
		size_t _minimapHandle; // Handle to MinimapUnit
		int _minimapFlags;

	public:
		// Creates empty scene object ( as definiton cannot be added later
		SceneObject();
		// Creates detached SceneObject with properties stored in
		// SODefinition, also recives handle to 'this'
		SceneObject(SceneObjectDefinition* soDef);
		virtual ~SceneObject();

		// Loads resources / creates Ogre stuff etc
		// SceneObject must be loaded prior to attaching to SceneNode
		// By default all child objects from definition are created
		virtual void LoadResources(Ogre::SceneManager* ogreMgr);

		// Unloads resources / deletes Ogre stuff etc
		// SceneObject must be detached from scene
		// By default all child objects are destroyed
		virtual void UnloadResources(Ogre::SceneManager* ogreMgr);

		bool IsLoaded() const { return _loaded; }

		// Adds child to 'this' object
		// If attachToSceneNode is true, also attaches child's scene node to 'this''s one,
		// also creates node if child don't have one
		// ( should only be set to false if object have no physical representation )
		// Returns index in children vector or negative value on failure ( -2 = child already added )
		int AttachChild(SceneObject* child,	bool attachToSceneNode = true);
	
		// Removes child at position idx
		// If detachFromSceneNode is true, then child's node remains unconnected after this call
		// If detachFromSceneNode is false, it node connection remains unchanged
		// If destroy scene node = true node is also destroyed
		// (for all this child need to have a scene node)
		void DetachChildAt(int idx, bool detachFromSceneNode = true, bool destroySceneNode = false);

		// Removes child from 'this' object
		// If detachFromSceneNode is true, then child's node remains unconnected after this call
		// If detachFromSceneNode is false, it node connection remains unchanged
		// If destroy scene node = true node is also destroyed
		// (for all this child need to have a scene node)
		void DetachChild(SceneObject* child, bool detachFromSceneNode = true, bool destroySceneNode = false);

		// Removes all children
		// If detachFromSceneNode is true, then child's node remains unconnected after this call
		// If detachFromSceneNode is false, it node connection remains unchanged
		// If destroy scene node = true node is also destroyed
		// (for all this child need to have a scene node)
		void DetachAllChildren(bool detachFromSceneNode = true, bool destroySceneNode = true);

		// Returns child at given index or invalid pointer on failure
		SceneObject* GetChildAt(int idx) const { return _childObjects[idx]; }

		SceneNode* GetSceneNode() const { return _sceneNode; }
		void SetSceneNode(SceneNode* node) { _sceneNode = node; }

		bool HaveChildren() const { return _childObjects.size() > 0; }

		bool HaveParent() const { return _parentObject != 0; }
		void SetParent(SceneObject* parent) { _parentObject = parent; }
		SceneObject* GetParent() const { return _parentObject; }

		size_t GetCurrentState() const { return _stateMachine.GetCurrentState(); }

		SceneObjectState* GetCurrentStatePtr() const { return _stateMachine.GetCurrentStatePtr(); }
		
		void ChangeState(size_t state) { _stateMachine.ChangeState(state); }

		void AddState(size_t type, SceneObjectState* state)
		{
			_stateMachine.AddState(type, state);
		}

		SceneObjectState* FindState(size_t state)
		{
			return _stateMachine.FindState(state);
		}

		uint32 GetSelectionFlags() const { return _selectionFlags; }
		virtual void SetSelectionFlags(uint32 flags) { _selectionFlags = flags; }
		void AddSelectionFlags(uint32 flags) { SetSelectionFlags(_selectionFlags | flags); }
		void RemoveSelectionFlags(uint32 flags) { SetSelectionFlags(_selectionFlags & (~flags)); }
	
	public:

		// Updates state of object ( by default delegates update to state machine , updates mover )
		virtual void Update(float ms);

		// Create actor and place it in scene -> SceneNode must be set before call
		// Should be called after subclass spawn
		// Also spawns children -> if child doesn't have SceneNode it 
		// creates one as this object's child node 
		virtual void Spawn();

		bool IsSpawned() const { return _spawned; }

		// Get event that is fired when so is spawned
		Event<SceneObject*>& Spawned() { return _onSpawn; }

		// Removes from scene, also despawns children and destroyes their nodes if
		// they are attached to this object's node
		virtual void Despawn();
		// Get event that is fired when so is despawned
		// Should be called after subclass despawn
		Event<SceneObject*>& Despawned()  { return _onDespawn; }

		// To be called if mouse hovers over scene object
		virtual void SetMouseHoverBegin()
		{
			_onHoverBegin.Fire(this);
		}
		
		// To be called if mouse leaves scene object
		virtual void SetMouseHoverEnd()
		{
			_onHoverEnd.Fire(this);
		}

		// Get event that is fired when mouse is hovering over object
		Event<SceneObject*>& MouseHoverBegin()  { return _onHoverBegin; }
		
		// Get event that is fired when mouse is hovering over object
		Event<SceneObject*>& MouseHoverEnd()  { return _onHoverEnd; }

		Event<SceneObject*>& Moved() { return _onMoved; }
		Event<SceneObject*>& Rotated() { return _onRotated; }

		const SceneObjectDefinition* GetSceneObjectDefinition() const { return _definition; }
		void SetSceneObjectDefinition(SceneObjectDefinition* soDef);
		
		int GetMinimapFlags() const { return _minimapFlags; }
		void SetMinimapFlags(int flags) { _minimapFlags = flags; }

		size_t GetMinimapHandle() const { return _minimapHandle; }
		void SetMinimapHandle(const size_t handle) { _minimapHandle = handle; }
		
		size_t GetMinimapIconHandle() const { return GetSceneObjectDefinition()->GetMinimapIconHandle(); }
		
		const string& GetMinimapIconMaterialName() const { return GetSceneObjectDefinition()->GetMinimapIconMaterialName(); }
		
		const Vector2& GetMinimapSize() const { return GetSceneObjectDefinition()->GetMinimapSize(); }

	public:
		// Properties accessors
		string GetName() const { return _definition->GetName(); }
		TypeId GetFinalType() const { return _definition->GetFinalType(); }
		bool IsIndependent() const { return _definition->IsIndependent(); }
		bool InheritsScale() const { return _definition->InheritsScale(); }
		bool HavePhysics() const { return _definition->HavePhysics(); }
		bool HaveFootprint() const { return _definition->HaveFootprint(); }
		
		size_t GetObjectSpecificDataHandle() const { return _objectSpecificDataHandle; }
		void SetObjectSpecificDataHandle(size_t handle) { _objectSpecificDataHandle = handle; }
		template<typename DataType>
		DataType* GetObjectSpecificData() const 
		{ 
			return reinterpret_cast<DataType*>(_objectSpecificDataHandle); 
		}

		size_t GetKindSpecificDataHandle() const { return _definition->GetKindSpecificDataHandle(); }
		template<typename DataType>
		DataType* GetKindSpecificData() const 
		{ 
			return _definition->GetKindSpecificData<DataType>(); 
		}

		unsigned int GetOwner() const { return _owner; }
		virtual void SetOwner(unsigned int owner) { _owner = owner; }

	public:
		// Wrapper around Ogre SceneNode :
		// Methods to access / change position in space ( SO's position = scene node position )

		// Returns orientation of object relative to parent
		const Quaternion& GetOrientation() const { return _sceneNode->getOrientation(); }
		// Returns absolute orientation of object
		const Quaternion& GetOrientationAbsolute() const { return _sceneNode->_getDerivedOrientation(); }

		// Returns position of object relative to parent
		const Vector3& GetPosition() const { return _sceneNode->getPosition(); }
		// Returns ansolute position of object
		const Vector3& GetPositionAbsolute() const { return _sceneNode->_getDerivedPosition(); }

		// Returns direction of node relative to parent ( computed from orientation )
		Vector3 GetDirection() const
		{
			// Direction is local unit -z vector in global space 
			return GetOrientation() * Vector3(0.f,0.f,1.f);
		}
		// Returns ansolute direction of node ( computed from orientation )
		Vector3 GetDirectionAbsolute() const
		{
			// Direction is local unit -z vector in global space 
			return GetOrientationAbsolute() * Vector3(0.f,0.f,1.f);
		}

		// Returns scale of object relative to parent
		const Vector3& GetScale() const { return _sceneNode->getScale(); }
		// Returns scale of object 
		const Vector3& GetScaleAbsolute() const { return _sceneNode->_getDerivedScale(); }

		// Returns value indicating wether bbox should be rendered around object
		bool GetShowBoundingBox() const { return _sceneNode->getShowBoundingBox(); }
		
		// Set value indicating wether bbox should be rendered around object
		void SetShowBoundingBox(bool show) { return _sceneNode->showBoundingBox(show); }

		//Rotate the object around an arbitrary axis. 
		void Rotate (const Vector3 &axis, const Radian &angle, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_LOCAL)
		{
			_sceneNode->rotate(axis, angle, relativeTo);
			_onRotated.Fire(this);
		}

		// Rotate the object around an aritrary axis using a Quarternion.
		void Rotate (const Quaternion &q, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_LOCAL)
		{
			_sceneNode->rotate(q, relativeTo);
			_onRotated.Fire(this);
		}

		void RotateQuiet(const Quaternion &q, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_LOCAL)
		{
			_sceneNode->rotate(q, relativeTo);
		}

		//Scales the object, combining it's current scale with the passed in scaling factor.
		void Scale (const Vector3 &scale)
		{
			_sceneNode->scale(scale);
		}

		//	Scales the object, combining it's current scale with the passed in scaling factor.
		void Scale (float x, float y, float z)
		{
			_sceneNode->scale(x,y,z);
		}
		
		// Changes direction of object to given value
		void SetDirection (float x, float y, float z, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT, const Vector3 &localDirectionVector = Vector3::UNIT_Z)
		{
			_sceneNode->setDirection(x,y,z,relativeTo,localDirectionVector);
			_onRotated.Fire(this);
		}

		// Changes direction of node to given value
		void SetDirection (const Vector3 &vec, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT, const Vector3 &localDirectionVector = Vector3::UNIT_Z)
		{
			_sceneNode->setDirection(vec,relativeTo,localDirectionVector);
			_onRotated.Fire(this);
		}

		void SetDirectionQuiet(const Vector3 &vec, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT, const Vector3 &localDirectionVector = Vector3::UNIT_Z)
		{
			_sceneNode->setDirection(vec,relativeTo,localDirectionVector);
		}

		//	Tells the object whether it should inherit orientation from it's parent node.
		void SetInheritOrientation (bool inherit)
		{
			_sceneNode->setInheritOrientation(inherit);
		}

		//Tells the object whether it should inherit scaling factors from it's parent node. 
		void SetInheritScale (bool inherit)
		{
			_sceneNode->setInheritScale(inherit);
		}

		// Changes orientation of object to given one
		void SetOrientation (const Quaternion &q)
		{
			_sceneNode->setOrientation(q);
			_onRotated.Fire(this);
		}

		//Sets the orientation of this object via quaternion parameters. 
		void SetOrientation (float w, float x, float y, float z)
		{
			_sceneNode->setOrientation(w,x,y,z);
			_onRotated.Fire(this);
		}

		void SetOrientationQuiet(const Quaternion &q)
		{
			_sceneNode->setOrientation(q);
		}

		// Sets the position of the object relative to it's parent.
		void SetPosition (const Vector3 &pos)
		{
			_sceneNode->setPosition(pos);
			_onMoved.Fire(this);
		}

		//Sets the position of the object relative to it's parent.
		void SetPosition (float x, float y, float z)
		{
			_sceneNode->setPosition(x,y,z);
			_onMoved.Fire(this);
		}

		void SetPositionQuiet(const Vector3 &pos)
		{
			_sceneNode->setPosition(pos);
		}

		//	Sets the scaling factor applied to this object.
		void SetScale (const Vector3 &scale)
		{
			_sceneNode->setScale(scale);
		}

		//	Sets the scaling factor applied to this object.
		void SetScale (float x, float y, float z)
		{
			_sceneNode->setScale(x,y,z);
		}

		//Makes all objects attached to this node become visible / invisible.
		void SetVisible (bool visible, bool cascade=true)
		{
			_sceneNode->setVisible(visible, cascade);
		}

		//Moves the node along the Cartesian axes. 
		void Move (const Vector3 &d, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT)
		{
			_sceneNode->translate(d, relativeTo);
			_onMoved.Fire(this);
		}

		//Moves the node along the Cartesian axes.
		void Move (float x, float y, float z, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT)
		{
			_sceneNode->translate(x,y,z, relativeTo);
			_onMoved.Fire(this);
		}

		//Moves the node along arbitrary axes. 
		void Move (const Matrix3 &axes, const Vector3 &move, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT)
		{
			_sceneNode->translate(axes, move, relativeTo);
			_onMoved.Fire(this);
		}

		//Moves the node along arbitrary axes.
		void Move (const Matrix3 &axes, float x, float y, float z, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT)
		{
			_sceneNode->translate(axes, x, y, z, relativeTo);
			_onMoved.Fire(this);
		}

		// Do not fire moved event
		void MoveQuiet(const Vector3 &d, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT)
		{
			_sceneNode->translate(d, relativeTo);
		}

		// Moves this object ignoring scales of SceneNodes, that is scales movement by
		// inverse of scale of this node ( so assumed child is moved in exact world units )
		void MoveNonScaled( const Vector3& move, Ogre::Node::TransformSpace relativeTo = Ogre::Node::TS_PARENT )
		{
			Vector3 scale(GetScaleAbsolute());
			_sceneNode->translate(move / scale, relativeTo);
		}
	};

}