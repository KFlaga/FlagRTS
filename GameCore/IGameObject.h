#pragma once

#include <ObjectHandle.h>
#include <TypeId.h>
#include <TypeDefs.h>

namespace Ogre
{
	class SceneManager;
}

namespace FlagRTS
{
	/// Base class for objects created in game
	/**
		Every object stores its handle and TypeId of its final type.
		Final TypeId should be set in constructors of sub classes.
		Handle's object part is set in IGameObject constructor and cannot be
		overriden and serves as object's unique id.
		Handle's type part is used for saving ObjectData slot.
		IGameObject have empty Update and Load/Unload methods, sub class is not
		required to support those operations (placed here for convenience)
	*/
	class IGameObject
	{
		DISALLOW_COPY(IGameObject);

	private:
		ObjectHandle _handle;
		TypeId _finalType;

	public:
		IGameObject()
		{	
			_handle.Object = reinterpret_cast<size_t>(this);
		}

		virtual ~IGameObject() { }

		ObjectHandle GetHandle() const { return _handle; }

		void SetFinalType(TypeId type) { _finalType; }
		TypeId GetFinalType() const { return _finalType; }

		virtual void Update(float ms) { }
		virtual void LoadResources(Ogre::SceneManager* ogreMgr) { }
		virtual void UnloadResources(Ogre::SceneManager* ogreMgr) { }
	};
}