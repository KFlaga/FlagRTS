#pragma once

#include "SceneObjectDefaultPool.h"
#include "FXParticles.h"
#include "FXSound.h"

namespace FlagRTS
{
	class FXParticlesPool : public SceneObjectDefaultPool<FXParticles, FXParticlesDefinition>
	{
	public:
		FXParticlesPool();
		~FXParticlesPool();

		IGameObject* FindByHandle(ObjectHandle handle)
		{
			return static_cast<IGameObject*>(FindByHandleCast(handle));
		}
		
		FXParticles* CreateCast(FXParticlesDefinition* objDef, int owner);

		void DestroyCast(FXParticles* object);
	};

	class FXSoundPool : public SceneObjectDefaultPool<FXSound, FXSoundDefinition>
	{
	public:
		FXSoundPool();
		~FXSoundPool();

		IGameObject* FindByHandle(ObjectHandle handle)
		{
			return static_cast<IGameObject*>(FindByHandleCast(handle));
		}
		
		FXSound* CreateCast(FXSoundDefinition* objDef, int owner);

		void DestroyCast(FXSound* object);
	};
}