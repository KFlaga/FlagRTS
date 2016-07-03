#pragma once

#include "FXObject.h"
#include "FXParticlesDefinition.h"

namespace Ogre
{
	class ParticleSystem;
}

namespace FlagRTS
{
	
	class FXParticles : public FXObject
	{
	private:
		Ogre::ParticleSystem* _particles;

	public:
		FXParticles(FXParticlesDefinition* sfxDef);
		~FXParticles();

		FXParticlesDefinition* GetFXParticlesDefinition() const
		{ 
			return static_cast<FXParticlesDefinition*>(_fxDef); 
		}

		
		void Update(float ms);

		// Creates ogre particle system
		void LoadResources(Ogre::SceneManager* ogreMgr);
		// Destroys ogre particle system
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		// Attaches particle system to node, but makes it invisible
		void Spawn();
		// Detaches particle system from node
		void Despawn();
		// Makes particle system visible
		void Start();
		// Makes particle system invisible
		void Stop();
		// Sets speed to 0, so particle system wont be updated
		void Pause();

		void SetPlaySpeed(float value);
	};
}