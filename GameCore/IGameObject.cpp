#include "IGameObject.h"
#include "IObjectComponent.h"

namespace FlagRTS
{
	IGameObject::IGameObject()
	{
		_handle.Object = reinterpret_cast<size_t>(this);
		_owner = NEUTRAL_PLAYERNUM;
	}

	IGameObject::~IGameObject()
	{
		for(auto it = _components.begin(); it != _components.end(); ++it)
		{
			xDelete(it->Value);
		}
	}

	void IGameObject::Update(float ms)
	{
		for(auto it = _components.begin(); it != _components.end(); ++it)
		{
			it->Value->Update(ms);
		}
	}

	void IGameObject::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		for(auto it = _components.begin(); it != _components.end(); ++it)
		{
			it->Value->LoadResources(ogreMgr);
		}
	}

	void IGameObject::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		for(auto it = _components.begin(); it != _components.end(); ++it)
		{
			it->Value->UnloadResources(ogreMgr);
		}
	}
}