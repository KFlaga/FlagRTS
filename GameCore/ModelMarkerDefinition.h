#pragma once

#include "SceneMarkerDefinition.h"
#include "DataTypes.h"

namespace FlagRTS
{
	class ModelMarkerDefinition : public SceneMarkerDefinition
	{
	protected:
		string _modelName;
		string _modelGroup;
		Vector3 _ingameSize;
		Vector3 _meshScale;

	public:
		// Sets default values
		ModelMarkerDefinition();
		// Reads values from xml node
		ModelMarkerDefinition(XmlNode* soDefNode);

		void SetModelName(const string& name) { _modelName = name; }
		const string& GetModelName() const { return _modelName; }
		
		void SetModelGroup(const string& group) { _modelGroup = group; }
		const string& GetModelGroup() const { return _modelGroup; }
		
		void SetScale(const Vector3& scale) { _meshScale = scale; }
		const Vector3& GetScale() const { return _meshScale; }
		
		void SetSize(const Vector3& size) { _ingameSize = size; }
		const Vector3& GetSize() const { return _ingameSize; }
	};
}