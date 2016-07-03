#pragma once

#include "VertexBuffer.h"
#include "IndexBuffer.h"

namespace Ogre
{
	class Matrix4;
	class Vector3;
	class Mesh;
	class Entity;
}

class btBoxShape;
class btSphereShape;
class btCapsuleShape;
class btCylinderShape;
class btConvexHullShape;
class btTriangleMeshShape;
class btCompoundShape;


namespace ORTS
{
	namespace Physics
	{
		// Base class for converters of ogre meshes to bullet collision shapes
		// Loads vertex/index buffers and transforms them to different collision shapes
		class IMeshToCollisionShapeConverter
		{
		protected:
			VertexBuffer _vertexBuffer;
			IndexBuffer  _indexBuffer;

			Ogre::Matrix4* _transform;
			float _boundRadius;
			Ogre::Vector3* _bounds;

		public:
			IMeshToCollisionShapeConverter();
			virtual ~IMeshToCollisionShapeConverter();

			const VertexBuffer& GetVertexBuffer() const { return _vertexBuffer; }

			unsigned int GetVertexCount() const { return _vertexBuffer.GetVertexCount(); }

			const IndexBuffer& GetIndexBuffer() const { return _indexBuffer; }

			unsigned int GetIndexCount() const { return _indexBuffer.GetIndexCount(); }

			// Transforms all vertices of mesh by given transform matrix
			// ( if default scale/orientation of shape should be different than mesh )
			void SetVertexTransform(Ogre::Matrix4* _transform);

			// Recalculates bounding box/sphere radius
			void UpdateBounds();

			// Adds mesh info to be converted
			virtual void AddMesh(Ogre::Mesh* mesh) = 0;
			
			// Adds mesh info of entity to be converted
			virtual void AddEntity(Ogre::Entity* entity) = 0;

			// Clears mesh data of converter
			virtual void Clear();

			// Creates shpere shape that contains mesh
			btSphereShape* CreateSphere();
			// Creates box shape that contains mesh
			btBoxShape* CreateBox();

			btTriangleMeshShape* CreateTrimesh();
			// Creates cylinder shape that contains mesh
			btCylinderShape* CreateCylinder();
			// Creates capsule shape that contains mesh
			btCapsuleShape* CreateCapsule();
			// Creates convex hull shape that contains mesh
			btConvexHullShape* CreateConvexHull();
			// Creates shape made of boxes - each contains one bone of skeleton
			// ( mesh must have skeleton )
			virtual btCompoundShape* CreateBoxDecomposition();
			// Creates shape made of capsules - each contains one bone of skeleton
			// ( mesh must have skeleton )
			virtual btCompoundShape* CreateCapsuleDecomposition();
		};
	}
}