#pragma once

#include "SceneMarkerDefinition.h"
#include "DataTypes.h"

namespace FlagRTS
{
	class TerrainProjectionMarkerDefinition : public SceneMarkerDefinition
	{
	protected:
		string _textureName;
		Vector2 _baseHalfSize;

	public:
		// Sets default values
		TerrainProjectionMarkerDefinition();
		// Reads values from xml node
		TerrainProjectionMarkerDefinition(XmlNode* soDefNode);

		const string& GetTextureName() const { return _textureName; }
		void SetTextureName(const string& tex) { _textureName = tex; }
		
		const Vector2& GetBaseHalfSize() const { return _baseHalfSize; }
		void SetBaseHalfSize(const Vector2& size) { _baseHalfSize = size; }
	};
}