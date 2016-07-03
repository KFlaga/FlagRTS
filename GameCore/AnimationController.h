#pragma once

#include "Animation.h"
#include <map>
#include <Array.h>


namespace Ogre
{
	class Entity;
}

namespace FlagRTS
{
	class SceneObject;

	// Animation controller should be created after other
	// resources ( neccessary for animation ) are loaded
	class AnimationControler
	{
	protected:
		SceneObject* _owner;
		AnimationMap _animations;
		Array<Animation*> _currentAnimations;
		float _fadingFactor;
		bool _changePending;

	public:
		// Creates controller and sets given animations
		AnimationControler(AnimationDefinitionMap* animDefs, 
			SceneObject* owner);

		AnimationControler(SceneObject* owner);
		// Destroys animations
		~AnimationControler();

		void SetAnimations(AnimationDefinitionMap* animDefs);

		void StartAnimation(int state, const string& action = "Default", bool looped = true);
		
		void AddAnimation(int state, const string& action = "Default", bool looped = true);
		void EndAnimation(int state, const string& action);
		void EndAllAnimations();

		// Ends all animations and starsts new one (foced)
		void ChangeAnimation(int state, const string& action, bool looped = true);
		
		Animation* FindAnimation(int state, const string& action);

		void Update(float ms);

		const Array<Animation*>& GetCurrentAnimations() const { return _currentAnimations; }
	
	protected:
		Animation* CreateNewAnimation(AnimationDefinition* animDef);
		void AddNewAnimation(Animation* toAnim, AnimationDefinition* animDef);
		Ogre::Entity* GetOwnerEntity();

		void FadeAnimation();
	};
}