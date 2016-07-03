#pragma once

#include "Animation.h"
#include "SkeletalAnimationDefinition.h"
#include <OgreAnimationState.h>
#include <List.h>


namespace FlagRTS
{
	class SkeletalAnimation : public Animation
	{
	private:
		// List of animations as many ogre animation may be applied for single action
		List<Ogre::AnimationState*> _ogreAnimations;

	public:
		SkeletalAnimation(SkeletalAnimationDefinition* saDef);
	
		void AddOgreAnimation(Ogre::AnimationState* anim);
		void RemoveOgreAnimation(Ogre::AnimationState* anim);

		void Start();
		void End();
		void Update(float ms);

		void SetLoop(bool value) 
		{ 
			for(auto animIt = _ogreAnimations.begin(); animIt != _ogreAnimations.end(); animIt++)
			{
				(*animIt).Value->setLoop(value);
			}
			_loop = value;
		}
	};
}