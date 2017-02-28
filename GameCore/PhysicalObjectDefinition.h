#pragma once

#include "SceneObjectDefinition.h"
#include "DataTypes.h"
#include "AnimationDefinition.h"

namespace PathFinding
{
	class UniformGridObstacle;
}

namespace FlagRTS
{
	class PhysicalObject;

	class PhysicalObjectDefinition : public SceneObjectDefinition
	{
	protected:
		string _modelName;
		string _modelGroup;
		Vector3 _ingameSize;
		Vector3 _meshScale;
		AnimationDefinitionMap* _animations;

		PhysicalObjectDefinition* _portraitDef;
		Vector3 _portraitPosition;
		Quaternion _portraitOrientation;
		bool _useDefaultPortraitPosition;

		string _playerMaterialName; // Name of material (and so sub-entity) which uses player color as parameter
									// It will be colored according to owning player color ( it must be set if
									// mesh material support it or empty otherwise )
	public:
		// Sets default values
		PhysicalObjectDefinition();
		// Reads values from xml node
		PhysicalObjectDefinition(XmlNode* soDefNode);
		~PhysicalObjectDefinition();

		void SetModelName(const string& name) { _modelName = name; }
		const string& GetModelName() const { return _modelName; }
		
		void SetModelGroup(const string& group) { _modelGroup = group; }
		const string& GetModelGroup() const { return _modelGroup; }
		
		void SetScale(const Vector3& scale) { _meshScale = scale; }
		const Vector3& GetScale() const { return _meshScale; }
		
		void SetSize(const Vector3& size) { _ingameSize = size; }
		const Vector3& GetSize() const { return _ingameSize; }

		void SetAnimations(AnimationDefinitionMap* anims) { _animations = anims; }
		AnimationDefinitionMap* GetAnimations() const { return _animations; }

		PhysicalObjectDefinition* GetPortraitDefinition() const { return _portraitDef; }
		void SetPortraitDefinition(PhysicalObjectDefinition* object) { _portraitDef = object; }

		const Vector3& GetPortraitPosition() const { return _portraitPosition; }
		void SetPortraitPosition(const Vector3& pos) { _portraitPosition = pos; }
		
		bool GetUseDefaultPortraitPosition() const { return _useDefaultPortraitPosition; }
		void SetUseDefaultPortraitPosition(bool value) { _useDefaultPortraitPosition = value; }

		const Quaternion& GetPortraitOrientation() const { return _portraitOrientation; }
		void SetPortraitOrientation(const Quaternion& q) { _portraitOrientation = q; }

		void SetPlayerColorMaterial(const string& name) { _playerMaterialName = name; }
		const string& GetPlayerColorMaterial() const { return _playerMaterialName; }

	protected:
		void ParsePOFlags(XmlNode* flagsNode);
		void ParsePOProperties(XmlNode* propsNode);
		void ParsePOEvents(XmlNode* eventsNode);
		void ParsePOAnimations(XmlNode* animNode);
		void ParseDisplayObject(XmlNode* dobjNode);
	};
}