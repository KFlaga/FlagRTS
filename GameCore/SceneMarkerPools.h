#pragma once

#include "SceneObjectDefaultPool.h"
#include "ModelMarker.h"
#include "TerrainProjectionMarker.h"
#include "BillboardMarker.h"

namespace FlagRTS
{
	class TerrainProjectionMarkerPool : public SceneObjectDefaultPool<TerrainProjectionMarker, TerrainProjectionMarkerDefinition>
	{
	public:
		TerrainProjectionMarkerPool();
		~TerrainProjectionMarkerPool();

		IGameObject* FindByHandle(ObjectHandle handle)
		{
			return static_cast<IGameObject*>(FindByHandleCast(handle));
		}
		
		TerrainProjectionMarker* CreateCast(TerrainProjectionMarkerDefinition* objDef, int owner);

		void DestroyCast(TerrainProjectionMarker* object);
	};

	class ModelMarkerPool : public SceneObjectDefaultPool<ModelMarker, ModelMarkerDefinition>
	{
	public:
		ModelMarkerPool();
		~ModelMarkerPool();

		IGameObject* FindByHandle(ObjectHandle handle)
		{
			return static_cast<IGameObject*>(FindByHandleCast(handle));
		}
		
		ModelMarker* CreateCast(ModelMarkerDefinition* objDef, int owner);

		void DestroyCast(ModelMarker* object);
	};

	class BillboardMarkerPool : public SceneObjectDefaultPool<BillboardMarker, BillboardMarkerDefinition>
	{
	public:
		BillboardMarkerPool();
		~BillboardMarkerPool();

		IGameObject* FindByHandle(ObjectHandle handle)
		{
			return static_cast<IGameObject*>(FindByHandleCast(handle));
		}
		
		BillboardMarker* CreateCast(BillboardMarkerDefinition* objDef, int owner);

		void DestroyCast(BillboardMarker* object);
	};
}