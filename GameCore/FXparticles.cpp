#include "FXParticles.h"
#include <OgreParticleSystem.h>
#include <OgreParticleSystemManager.h>
#include <OgreSceneManager.h>
#include <OgreResourceManager.h>

namespace FlagRTS
{
	FXParticles::FXParticles(FXParticlesDefinition* sfxDef) :
		FXObject(sfxDef)
	{

	}

	FXParticles::~FXParticles()
	{

	}

	void FXParticles::Update(float ms)
	{

	}

	void FXParticles::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		if(!_loaded)
		{
			// Create partice system from script and some unique name
			_particles = ogreMgr->createParticleSystem(
				std::to_string( this->GetHandle().Object ) + "fxp", 
				_fxDef->GetEffectName());
			_particles->setCastShadows(false);
			_particles->setNonVisibleUpdateTimeout(2.f);
			_particles->setQueryFlags(0);
			_particles->setRenderingDistance(2000.f);
			_particles->setSpeedFactor(_playSpeed);

			_particles->getUserObjectBindings().setUserAny(Ogre::Any((SceneObject*)this));

			FXObject::LoadResources(ogreMgr);
			_loaded = true;
		}
	}

	void FXParticles::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		if(_loaded)
		{
			_loaded = false;

			ogreMgr->destroyParticleSystem(_particles);
			_particles = 0;

			FXObject::UnloadResources(ogreMgr);
		}
	}

	void FXParticles::Spawn()
	{
		if(!_spawned)
		{
			_sceneNode->attachObject(_particles);
			_particles->setVisible(false);
			_particles->setEmitting(false);
			FXObject::Spawn();
			_spawned = true;
		}
	}

	void FXParticles::Despawn()
	{
		if(_spawned)
		{
			FXObject::Despawn();
			_sceneNode->detachObject(_particles);
			_spawned = false;
		}
	}

	void FXParticles::Start()
	{
		_particles->setVisible(true);
		_particles->setEmitting(true);
		_particles->setSpeedFactor(_playSpeed);
		_playState = PlayState::Playing;
	}

	void FXParticles::Stop()
	{
		_particles->setVisible(false);
		_particles->setEmitting(false);
		_playState = PlayState::Stopped;
	}

	void FXParticles::Pause()
	{
		_particles->setSpeedFactor(0.f);
		_playState = PlayState::Paused;
	}

	void FXParticles::SetPlaySpeed(float value)
	{
		_particles->setSpeedFactor(value);
		FXObject::SetPlaySpeed(value);
	}
}