#pragma once

#include "DataTypes.h"
#include "ObjectDefinition.h"
#include <List.h>
#include <map>
#include "GameObjectFactory.h"

namespace FlagRTS
{
	class AnimationDefinition : public ObjectDefinition
	{
	protected:
		string _animName; // Name of animation ( for ogre its ogre name )
		string _action; // Action animation is dedicated for
		bool _fadeIn; // If true animation will fade in on begin ( slowly gain weight )
		bool _fadeOut; // If true animation will fade out on end ( slowly loose weight )
		bool _loop;
		float _speed; // Animation speed multiplier

	public:
		// animDefNode is named "Animation"
		AnimationDefinition(XmlNode* animDefNode); 

		AnimationDefinition();

		void SetName(const string& name) { _animName = name; }
		const string& GetName() const { return _animName; }

		void SetAction(const string& action) { _action = action; }
		const string& GetAction() const { return _action; }
		
		void SetSpeed(const float speed) { _speed = speed; }
		float GetSpeed() const { return _speed; }

		bool GetFadeIn() const { return _fadeIn; }
		void SetFadeIn(bool value) { _fadeIn = value; }
		
		bool GetFadeOut() const { return _fadeOut; }
		void SetFadeOut(bool value) { _fadeOut = value; }

		virtual void SetLoop(bool value) { _loop = value; }
		virtual bool GetLoop() const { return _loop; }
	};

	class AnimationDefinitionFactory : public SubClassXmlFactory
	{
	public:
		AnimationDefinitionFactory();
	};

	// List of animations
	typedef List<AnimationDefinition*> AnimationDefinitionsSet;
	// Maps state enum with animations sets for this state
	typedef std::map<int, AnimationDefinitionsSet*> AnimationDefinitionMap;
}