#include "SceneObject.h"
#include <Exception.h>
#include "GameWorld.h"
#include "Player.h"
#include <OgreSceneManager.h>
#include "Minimap.h"

namespace FlagRTS
{
	SceneObject::SceneObject() :
		IGameObject(),
		_sceneNode(0),
		_parentObject(0),
		_definition(0),
		_stateMachine(),
		_loaded(false),
		_spawned(false),
		_selectionFlags(0),
		_owner(NEUTRAL_PLAYERNUM),
		_minimapHandle(0),
		_minimapFlags(MinimapFlags::NeverVisible)
	{ }

	void SceneObject::SetSceneObjectDefinition(SceneObjectDefinition* soDef)
	{
		_definition = soDef;
		_onSpawn = soDef->GetOnSpawnEvent();
		_onDespawn = soDef->GetOnDespawnEvent();
		_onHoverBegin = soDef->GetOnHoverBeginEvent();
		_onHoverEnd = soDef->GetOnHoverEndEvent();
		_objectSpecificDataHandle = soDef->RequestObjectDataHandle();
		_minimapFlags = soDef->GetMinimapFlags();
	}

	SceneObject::SceneObject(SceneObjectDefinition* soDef) :
		IGameObject(),
		_sceneNode(0),
		_parentObject(0),
		_definition(soDef),
		_stateMachine(),
		_loaded(false),
		_spawned(false),
		_selectionFlags(soDef->GetSelectionFlags()),
		_minimapHandle(0),
		_minimapFlags(soDef->GetMinimapFlags()),
		_owner(NEUTRAL_PLAYERNUM)
	{
		SetFinalType( soDef->GetFinalType() );

		_onSpawn = soDef->GetOnSpawnEvent();
		_onDespawn = soDef->GetOnDespawnEvent();
		_onHoverBegin = soDef->GetOnHoverBeginEvent();
		_onHoverEnd = soDef->GetOnHoverEndEvent();

		_objectSpecificDataHandle = soDef->RequestObjectDataHandle();
	}

	SceneObject::~SceneObject()
	{
		if( GetParent() != 0 )
			GetParent()->DetachChild(this, false, false);
		for(unsigned int i = 0; i < _childObjects.size(); ++i)
		{
			GameWorld::GlobalWorld->QueueDestroySceneObject(_childObjects[i]);
		}
		DetachAllChildren(false, false);

		if(_objectSpecificDataHandle != 0)
		{
			_definition->DeleteObjectDataHandle(_objectSpecificDataHandle);
		}
	}

	void SceneObject::Update(float ms) 
	{ 
		_stateMachine.Update(ms);
	}

	int SceneObject::AttachChild(SceneObject* child, bool attachToSceneNode)
	{
		// Check if child is already in list
		if(child->GetParent() == this)
			return -2;
		else if(child->GetParent() != 0)
		{
			// Detach from old parent, dont destroy its node if it is to attached
			child->GetParent()->DetachChild(child, attachToSceneNode, !attachToSceneNode);
		}

		_childObjects.push_back(child);
		if( attachToSceneNode )
		{
			if(child->GetSceneNode() == 0)
			{
				SceneNode* snode = _sceneNode->createChildSceneNode();
				child->SetSceneNode(snode);
				child->Spawn(); // If there was no node, child wasn't spawned yet, so spawn it
			}
			else
			{
				// SceneNode should be detached at this point
				try
				{
					SceneNode* snode = child->GetSceneNode();
					_sceneNode->addChild(snode);
				}
				catch(Ogre::Exception& e)
				{
					CastException_d((string("Trying to attach already attached SceneNode or node == 0. Parent: ") +
						GetName() + ", child: " + child->GetName() + ". Reason: " + e.getFullDescription()).c_str());
				}
			}
		}
		child->SetParent(this);
		return _childObjects.size() - 1;
	}

	void SceneObject::DetachChildAt(int idx, bool detachFromSceneNode, bool destroySceneNode)
	{
		// Bad index - return
		if(idx > (int)_childObjects.size())
			return;

		// Get child and remove it from vector
		SceneObject* child = _childObjects[idx];
		for(uint32 i = idx; i < _childObjects.size() - 1; i++)
		{
			std::swap(_childObjects[i], _childObjects[i+1]);
		}
		_childObjects.pop_back();

		// Detach scene node if requested
		if(detachFromSceneNode)
		{
			if(destroySceneNode)
			{
				child->GetSceneNode()->getCreator()->destroySceneNode(child->GetSceneNode());
				child->SetSceneNode(0);
			}
			else
				_sceneNode->removeChild(child->GetSceneNode());
		}
		child->SetParent(0);
	}

	void SceneObject::DetachChild(SceneObject* child, bool detachFromSceneNode, bool destroySceneNode)
	{
		// Find index of child
		int idx = -1;
		for(uint32 i = 0; i < _childObjects.size(); i++)
		{
			if(child == _childObjects[i])
			{
				idx = i;
				break;
			}
		}
		if(idx == -1) // No such child - return
			return;

		// Remove child from list
		for(uint32 i = idx; i < _childObjects.size() - 1; i++)
		{
			std::swap(_childObjects[i], _childObjects[i+1]);
		}
		_childObjects.pop_back();

		// Detach from scene node if requested
		if(detachFromSceneNode)
		{
			_sceneNode->removeChild(child->GetSceneNode());
		}
		child->SetParent(0);
	}

	void SceneObject::DetachAllChildren(bool detachFromSceneNode, bool destroySceneNode)
	{
		for(uint32 i = 0; i < _childObjects.size(); i++)
		{
			if(detachFromSceneNode)
			{
				_sceneNode->removeChild(_childObjects[i]->GetSceneNode());
			}
			_childObjects[i]->SetParent(0);
		}
		_childObjects.clear();
	}

	void SceneObject::Spawn()
	{
		if(_sceneNode != 0)
			_sceneNode->setInheritScale(InheritsScale());

		// Spawn all object from spawn array ( so ones that are read from definition )
		// Dont spawn other objects ( actually there shouldnt be any )
		for(unsigned int i = 0; i < _spawnObjects.size(); ++i)
		{
			GameWorld::GlobalWorld->SpawnSceneObject(_spawnObjects[i].first, 
				SpawnInfo(_spawnObjects[i].second.second, _spawnObjects[i].second.first, false));
			AttachChild(_spawnObjects[i].first, true);
		}
	}

	void SceneObject::Despawn()
	{
		if( GetParent() != 0 )
			GetParent()->DetachChild(this, true, false);
		for(unsigned int i = 0; i < _spawnObjects.size(); ++i)
		{
			if(_spawnObjects[i].first->IsSpawned())
				GameWorld::GlobalWorld->DespawnSceneObject(_spawnObjects[i].first);
		}
		DetachAllChildren(false, false);
	}

	void SceneObject::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		_spawnObjects.reserve(_definition->GetChildObjects().size());
		_childObjects.reserve(_definition->GetChildObjects().size());
		for(unsigned int i = 0; i < _definition->GetChildObjects().size(); ++i)
		{
			auto object = GameWorld::GlobalWorld->CreateSceneObject(
				_definition->GetChildObjects()[i].first, _owner);
			_spawnObjects.push_back(std::make_pair(object, _definition->GetChildObjects()[i].second));
		}
	}

	void SceneObject::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		for(unsigned int i = 0; i < _childObjects.size(); ++i)
		{
			if( _childObjects[i]->IsSpawned() )
			{
				// this means that object was attached after Despawn, which should not happen
				_ASSERT(false); 
				GameWorld::GlobalWorld->DespawnSceneObject(_childObjects[i]);
			}
			GameWorld::GlobalWorld->DestroySceneObject(_childObjects[i]);
		}
		_childObjects.clear();
	}
}