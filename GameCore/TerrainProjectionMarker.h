#pragma once

#include "SceneMarker.h"
#include "TerrainProjectionMarkerDefinition.h"
#include <ArrayMap.h>

namespace Ogre
{
	class Terrain;
	class Frustum;
	class Pass;
}

namespace FlagRTS
{
	class TerrainProjectionMarker : public SceneMarker
	{
	protected:
		Vector2 _halfSize;
		Ogre::Frustum* _projectionFrustum;
		Ogre::Terrain* _lastProjectedTerrains[4];
		
		// info about materials that are receiving the decal : passes mapped by material names
		ArrayMaps<Ogre::Pass*>::KeyString _targetMaterials;

	public:
		TerrainProjectionMarker(TerrainProjectionMarkerDefinition* tpmDef);
		~TerrainProjectionMarker();

		// Creates projection frustum and loads texture
		void LoadResources(Ogre::SceneManager* ogreMgr);
		// Deletes projection frustum
		void UnloadResources(Ogre::SceneManager* ogreMgr);

		void Spawn();

		// Undo changes in terrain materials
		void Despawn();

		// Sets new size of marker ( scaling node doesnt work here as frustum must change )
		void UpdateSize(const Ogre::Vector2& size);

		TerrainProjectionMarkerDefinition* GetTerrainProjectionMarkerDefinition() const
		{
			return static_cast<TerrainProjectionMarkerDefinition*>(_definition);
		}

		const Vector2& GetHalfSize() const { return _halfSize; }
		
		const string& GetTextureName() const { return GetTerrainProjectionMarkerDefinition()->GetTextureName(); }

	protected:
		// Returns material
		void AddMaterial(const string& matName);
		void RemoveMaterial(const string& matName);

		// Called after scene node changed position/rotated -> updates terrain materials if needed
		// ( if marker changed terrain cells )
		void UpdateProjection( SceneObject* thisObject );

		DEFINE_DELEGATE1(MarkerMovedDelegate, TerrainProjectionMarker,
			UpdateProjection, SceneObject*);
		MarkerMovedDelegate _updateProjection;

		void FindProjectedTerrainMaterials(Array<string>& matNames);
	};
}