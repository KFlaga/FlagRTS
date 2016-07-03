#include "PhysicsStaticMeshToCollisionShapeConverter.h"

#include <Ogre\OgreEntity.h>
#include <Ogre\OgreMesh.h>
#include <Ogre\OgreSubMesh.h>

namespace ORTS
{
	using namespace Physics;

	StaticMeshToCollisionShapeConverter::StaticMeshToCollisionShapeConverter()
	{

	}

	StaticMeshToCollisionShapeConverter::~StaticMeshToCollisionShapeConverter()
	{

	}

	void StaticMeshToCollisionShapeConverter::AddMesh(Ogre::Mesh* mesh)
	{
		// First reserve buffers for all vertices/indices from all submeshes
		int vertexCount = 0;
		int indexCount = 0;

		if (mesh->sharedVertexData != 0)
		{
			vertexCount += mesh->sharedVertexData->vertexCount;
		}

		for (unsigned int i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			Ogre::SubMesh* subMesh = mesh->getSubMesh(i);

			if (!subMesh->useSharedVertices)
			{
				indexCount += subMesh->indexData->indexCount;
				vertexCount += subMesh->vertexData->vertexCount;
			}
			else 
			{
				indexCount += subMesh->indexData->indexCount;
			}
		}

		_vertexBuffer.Reserve(_vertexBuffer.GetVertexCount() + vertexCount);
		_indexBuffer.Reserve(_indexBuffer.GetIndexCount() + indexCount);
		
		// Fill reserved buffers with actual data
		if (mesh->sharedVertexData != 0)
		{
			_vertexBuffer.AddFromVertexData(mesh->sharedVertexData);
		}

		for (unsigned int i = 0; i < mesh->getNumSubMeshes(); ++i)
		{
			Ogre::SubMesh* subMesh = mesh->getSubMesh(i);

			if (!subMesh->useSharedVertices)
			{
				_indexBuffer.AddFromIndexData(subMesh->indexData, _vertexBuffer.GetVertexCount());
				_vertexBuffer.AddFromVertexData(subMesh->vertexData);
			}
			else 
			{
				_indexBuffer.AddFromIndexData(subMesh->indexData, 0u);
			}
		}

		UpdateBounds();
	}

	void StaticMeshToCollisionShapeConverter::AddEntity(Ogre::Entity* entity)
	{
		AddMesh(entity->getMesh().getPointer());
	}
}