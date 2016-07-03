#include "SceneMarkerPools.h"
#include "DefaultStateSuppliers.h"
#include <Exception.h>


namespace FlagRTS
{
	TerrainProjectionMarkerPool::TerrainProjectionMarkerPool()
	{
		// Register default state suppliers
		RegisterObjectStateSupplier("Classless", xNew0(SceneMarkerDefaultStateSupplier));
	}

	TerrainProjectionMarkerPool::~TerrainProjectionMarkerPool()
	{

	}

	TerrainProjectionMarker* TerrainProjectionMarkerPool::CreateCast(TerrainProjectionMarkerDefinition* objDef, int owner)
	{
		TerrainProjectionMarker* newObj(xNew1(TerrainProjectionMarker, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);
		AddStatesToObject(newObj, objDef);

		return newObj;
	}


	void TerrainProjectionMarkerPool::DestroyCast(TerrainProjectionMarker* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}

	ModelMarkerPool::ModelMarkerPool()
	{
		// Register default state suppliers
		RegisterObjectStateSupplier("Classless", xNew0(SceneMarkerDefaultStateSupplier));
	}

	ModelMarkerPool::~ModelMarkerPool()
	{

	}

	ModelMarker* ModelMarkerPool::CreateCast(ModelMarkerDefinition* objDef, int owner)
	{
		ModelMarker* newObj(xNew1(ModelMarker, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);
		AddStatesToObject(newObj, objDef);
		return newObj;
	}

	void ModelMarkerPool::DestroyCast(ModelMarker* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}

	BillboardMarkerPool::BillboardMarkerPool()
	{
		// Register default state suppliers
		RegisterObjectStateSupplier("Classless", xNew0(SceneMarkerDefaultStateSupplier));
	}

	BillboardMarkerPool::~BillboardMarkerPool()
	{

	}

	BillboardMarker* BillboardMarkerPool::CreateCast(BillboardMarkerDefinition* objDef, int owner)
	{
		BillboardMarker* newObj(xNew1(BillboardMarker, objDef));
		_objects.insert(newObj->GetHandle().Object, newObj);
		newObj->SetOwner(owner);
		AddStatesToObject(newObj, objDef);
		return newObj;
	}

	void BillboardMarkerPool::DestroyCast(BillboardMarker* object)
	{
		_objects.erase(object->GetHandle().Object);
		xDelete(object);
	}
}