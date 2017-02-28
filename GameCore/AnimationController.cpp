#include "AnimationController.h"
#include "SceneObjectStateMachine.h"
#include <OgreEntity.h>
#include "Animation.h"
#include "SkeletalAnimation.h"
#include "TransformAnimation.h"
#include "EmptyAnimation.h"
#include <Exception.h>
#include "PhysicalObject.h"
#include "ModelMarker.h"

namespace FlagRTS
{
	AnimationControler::AnimationControler(
		AnimationDefinitionMap* animDefs,
		SceneObject* owner) :
	_owner(owner),
		_changePending(false),
		_fadingFactor(10.0f)
	{ 
		SetAnimations(animDefs);
	}

	AnimationControler::AnimationControler(
		SceneObject* owner) :
	_owner(owner),
		_changePending(false),
		_fadingFactor(10.0f)
	{ 
	}

	AnimationControler::~AnimationControler()
	{
		for(auto animSetIt = _animations.begin(), end = _animations.end(); 
			animSetIt != end; ++animSetIt)
		{
			for(auto animIt = animSetIt->Value->begin(); animIt != animSetIt->Value->end(); ++animIt)
			{
				xDelete(animIt->Value);
			}
			xDelete(animSetIt->Value);
		}
	}

	void AnimationControler::SetAnimations(
		AnimationDefinitionMap* animDefs)
	{
		for(auto animSetIt = animDefs->begin(), end = animDefs->end(); 
			animSetIt != end; ++animSetIt)
		{
			// animSetIt contain definitions for all animations for one state
			AnimationSet* newSet(xNew0(AnimationSet));
			for(auto animIt = animSetIt->second->begin(); animIt != animSetIt->second->end(); ++animIt)
			{
				auto newAnimIt =  newSet->find((*animIt).Value->GetAction());
				if( newAnimIt != newSet->end() )
				{
					// if action already have animation created only add new 
					// animation to it
					AddNewAnimation(newAnimIt->Value, animIt->Value);
				}
				else
				{
					// Create new animation for this state
					Animation* newAnim = CreateNewAnimation(animIt->Value);
					newSet->insert(std::make_pair(
						newAnim->GetAction(), newAnim));
				}
			}
			_animations.insert(std::make_pair(
				animSetIt->first, newSet));
		}
	}

	void AnimationControler::StartAnimation(int state, const string& action, bool looped)
	{
		Animation* anim = FindAnimation(state, action);
		anim->SetLoop(looped);
		for(unsigned int i = 0; i < _currentAnimations.size(); ++i)
		{
			if(_currentAnimations[i] == anim)
			{
				anim->End();
				anim->Start();
				return;
			}
		}

		_currentAnimations.push_back(anim);
		anim->Start();
	}

	void AnimationControler::AddAnimation(int state, const string& action, bool looped)
	{
		Animation* anim = FindAnimation(state, action);
		anim->SetLoop(looped);
		_currentAnimations.push_back(anim);
		anim->Start();
	}

	void AnimationControler::EndAnimation(int state, const string& action)
	{
		Animation* anim = FindAnimation(state, action);
		for(unsigned int i = 0; i < _currentAnimations.size(); ++i)
		{
			if(_currentAnimations[i] == anim)
			{
				anim->End();
				_currentAnimations[i] = _currentAnimations.back();
				_currentAnimations.pop_back();
				break;
			}
		}
	}

	void AnimationControler::EndAllAnimations()
	{
		for(unsigned int i = 0; i < _currentAnimations.size(); ++i)
		{
			_currentAnimations[i]->End();
		}
		_currentAnimations.clear();
	}

	void AnimationControler::ChangeAnimation(int state, const string& action, bool looped)
	{
		EndAllAnimations();
		// Find animation
		auto anim = FindAnimation(state, action);
		anim->SetLoop(looped);

		anim->Start();
		_currentAnimations.push_back(anim);

	}

	void AnimationControler::ChangeAnimation(Animation* anim, bool looped)
	{
		_ASSERT(anim != 0);
		EndAllAnimations();
		anim->SetLoop(looped);
		anim->Start();
		_currentAnimations.push_back(anim);
	}

	Animation* AnimationControler::FindAnimation(int state, const string& action)
	{
		auto animSetIt = _animations.find(state);
		if(animSetIt != _animations.end())
		{
			auto animIt = animSetIt->Value->find(action);
			if(animIt != animSetIt->Value->end())
			{
				return animIt->Value;
			}
		}
		return 0;
	}


	void AnimationControler::Update(float ms)
	{
		for(unsigned int i = 0; i < _currentAnimations.size(); ++i)
		{
			_currentAnimations[i]->Update(ms);
		}
	}

	void AnimationControler::FadeAnimation()
	{

	}

	Animation* AnimationControler::CreateNewAnimation(AnimationDefinition* animDef)
	{
		Animation* newAnim = 0;
		if(animDef->GetFinalType() == GetTypeId<SkeletalAnimation>())
		{
			auto saDef = static_cast<SkeletalAnimationDefinition*>(animDef);
			newAnim = xNew1(SkeletalAnimation,saDef);
			Ogre::Entity* entity = GetOwnerEntity();
			static_cast<SkeletalAnimation*>(newAnim)->
				AddOgreAnimation(entity->getAnimationState(newAnim->GetName()));
		}
		else if(animDef->GetFinalType() == GetTypeId<TransformAnimation>())
		{
			auto taDef = static_cast<TransformAnimationDefinition*>(animDef);
			newAnim = xNew2(TransformAnimation,taDef, _owner);
		}
		else if(animDef->GetFinalType() == GetTypeId<EmptyAnimation>())
		{
			auto eaDef = static_cast<EmptyAnimationDefinition*>(animDef);
			newAnim = xNew1(EmptyAnimation,eaDef);
		}
		else
		{
			string msg = "Bad animation type for anim : " + (animDef)->GetName() +
				"\r\nUnit name: " + _owner->GetName();
			CastException(msg.c_str());
		}
		return newAnim;
	}

	void AnimationControler::AddNewAnimation(Animation* toAnim, AnimationDefinition* animDef)
	{
		if(animDef->GetFinalType() == GetTypeId<SkeletalAnimation>())
		{
			Ogre::Entity* entity = GetOwnerEntity();
			static_cast<SkeletalAnimation*>(toAnim)->
				AddOgreAnimation(entity->getAnimationState(animDef->GetName()));
		}
		else
		{
			// Translation can have only one animation pre action
			string msg = "Multiple animations per action for non-skeletal Animation, for anim : " 
				+ animDef->GetName() + "\r\nUnit name: " + _owner->GetName();
			CastException(msg.c_str());
		}
	}

	Ogre::Entity* AnimationControler::GetOwnerEntity()
	{
		PhysicalObject* pobj = dynamic_cast<PhysicalObject*>(_owner);
		if( pobj != 0 )
		{
			return pobj->GetEntity();
		}
		else
		{
			SceneMarker* sm = dynamic_cast<ModelMarker*>(_owner);
			if( sm == 0 )
			{
				string msg = "Request entity for object without one ( for skeletal animation ), for object : " 
					+ _owner->GetName();
				CastException(msg.c_str());
			}
		}
		return 0;
	}
}