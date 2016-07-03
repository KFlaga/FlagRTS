#include "FXObjectPools.h"
#include <Exception.h>


namespace FlagRTS
{
	FXParticlesPool::FXParticlesPool()
	{

	}

	FXParticlesPool::~FXParticlesPool()
	{

	}

	FXParticles* FXParticlesPool::CreateCast(FXParticlesDefinition* objDef, int owner)
	{
		FXParticles* newObj(xNew1(FXParticles, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);

		return newObj;
	}


	void FXParticlesPool::DestroyCast(FXParticles* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}

	FXSoundPool::FXSoundPool()
	{
	}

	FXSoundPool::~FXSoundPool()
	{

	}

	FXSound* FXSoundPool::CreateCast(FXSoundDefinition* objDef, int owner)
	{
		FXSound* newObj(xNew1(FXSound, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);

		return newObj;
	}

	void FXSoundPool::DestroyCast(FXSound* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}
}