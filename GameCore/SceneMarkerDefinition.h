#pragma once

#include "SceneObjectDefinition.h"
#include "DataTypes.h"
#include "AnimationDefinition.h"

namespace FlagRTS
{
	class SceneMarkerDefinition : public SceneObjectDefinition
	{
	protected:
		AnimationDefinitionMap* _animations;

		// Interactable markers can serve as small, unit/ability specific guis
		bool _isInteractable;

		// If timed, despawns after set time
		bool _isTimed; 
		float _timeout;

	public:
		// Sets default values
		SceneMarkerDefinition();
		// Reads values from xml node
		SceneMarkerDefinition(XmlNode* soDefNode);
		~SceneMarkerDefinition();

		void SetAnimations(AnimationDefinitionMap* anims) { _animations = anims; }
		AnimationDefinitionMap* GetAnimations() const { return _animations; }

		bool GetIsInteractable() const { return _isInteractable; }
		void SetIsInteractable(bool value) { _isInteractable = value; }

		bool GetIsTimed() const { return _isTimed; }
		void SetIsTimed(bool value) { _isTimed = value; }
		
		float GetTimeout() const { return _timeout; }
		void SetTimeout(float value) { _timeout = value; }

	protected:
		void ParseAnimations(XmlNode* animNode);
	};
}