#pragma once

#include "SceneObjectDefinition.h"
#include "GameObjectFactory.h"

namespace FlagRTS
{
	// FXs are all kinds of effects, placeable on scene including sounds, particles,
	// ligths etc. Most of them are by rule not independent and should be attached to
	// other SceneObjects ( with exception of FXEmitter )
	// 
	class FXObjectDefinition : public SceneObjectDefinition
	{
	protected:
		string _effectName;
		float _length;
		bool _playOnSpawn;
		bool _destroyOnEnd;
		bool _loop;

	public:
		// Sets default values
		FXObjectDefinition();
		// Reads values from xml node
		FXObjectDefinition(XmlNode* soDefNode);
		~FXObjectDefinition();

		void SetEffectName(const string& name) { _effectName = name; }
		const string& GetEffectName() const { return _effectName; }

		void SetLength(const float value) { _length = value; }
		float GetLength() const { return _length; }
		
		void SetLoop(const bool value) { _loop = value; }
		bool GetLoop() const { return _loop; }
		
		void SetPlayOnSpawn(const bool value) { _playOnSpawn = value; }
		bool GetPlayOnSpawn() const { return _playOnSpawn; }
		
		void SetDestroyOnEnd(const bool value) { _destroyOnEnd = value; }
		bool GetDestroyOnEnd() const { return _destroyOnEnd; }
	};

	class FXObjectDefinitionFactory : public SubClassXmlFactory
	{
	public:
		FXObjectDefinitionFactory();
	};
}