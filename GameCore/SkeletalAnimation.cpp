#include "SkeletalAnimation.h"
#include <OgreEntity.h>
#include <Exception.h>

namespace FlagRTS
{

	SkeletalAnimation::SkeletalAnimation(
		SkeletalAnimationDefinition* saDef) :
			Animation(saDef)
	{

	}

	void SkeletalAnimation::AddOgreAnimation(Ogre::AnimationState* anim) 
	{ 
		if(anim == 0)
		{
			CastException(string("Ogre animation not found (for action :" + _animDef->GetAction() + ")").c_str());
		}
		_ogreAnimations.push_back(anim);
		_animLength = std::max(_animLength, (float)(anim->getLength())*1000.0f) / _animDef->GetSpeed();
	}

	void SkeletalAnimation::RemoveOgreAnimation(Ogre::AnimationState* anim) 
	{ 
		_ogreAnimations.remove(anim); 
		if(!(anim->getLength() < _animLength))
		{
			for(auto animIt = _ogreAnimations.begin(); animIt != _ogreAnimations.end(); animIt++)
			{
				_animLength = std::max(_animLength, (float)(*animIt).Value->getLength());
			}
		}
	}

	void SkeletalAnimation::Start()
	{
		_animProgress = 0.0f;
		for(auto animIt = _ogreAnimations.begin(); animIt != _ogreAnimations.end(); animIt++)
		{
			(*animIt).Value->setTimePosition(0.0f);
			(*animIt).Value->setEnabled(true);
		}
	}

	void SkeletalAnimation::End()
	{
		for(auto animIt = _ogreAnimations.begin(); animIt != _ogreAnimations.end(); animIt++)
		{
			(*animIt).Value->setEnabled(false);
		}
	}

	void SkeletalAnimation::Update(float ms)
	{
		_animProgress += ms/_animLength;
		if(_animProgress > 1.0f)
		{
			if(GetLoop())
			{
				// animation looped
				_animProgress -= (int)(_animProgress * _animDef->GetSpeed());
			}
			else 
				return;
		}
		for(auto animIt = _ogreAnimations.begin(); animIt != _ogreAnimations.end(); animIt++)
		{
			(*animIt).Value->addTime((ms*0.001f) * _animDef->GetSpeed());
		}
	}
}