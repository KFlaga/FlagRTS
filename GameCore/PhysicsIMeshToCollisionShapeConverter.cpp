#include "PhysicsIMeshToCollisionShapeConverter.h"

#include <Ogre\OgreVector4.h>
#include <Ogre\OgreMatrix4.h>

#include <Memory.h>
#include <Exception.h>

#include <Bullet\BulletCollision\CollisionShapes\btCapsuleShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btBoxShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btConvexHullShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btCompoundShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btTriangleMeshShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btBvhTriangleMeshShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btSphereShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btCylinderShape.h>
#include <Bullet\BulletCollision\CollisionShapes\btTriangleMesh.h>

namespace ORTS
{
	using namespace Physics;

	IMeshToCollisionShapeConverter::IMeshToCollisionShapeConverter() :
		_bounds(xNew0(Ogre::Vector3))
	{

	}

	IMeshToCollisionShapeConverter::~IMeshToCollisionShapeConverter()
	{
		xDelete(_bounds);
	}

	void IMeshToCollisionShapeConverter::SetVertexTransform(Ogre::Matrix4* _transform)
	{
		for(unsigned int v = 0; v < _vertexBuffer.GetVertexCount(); ++v)
		{
			Ogre::Vector4 vertex(
				_vertexBuffer.GetVertex(v).x,
				_vertexBuffer.GetVertex(v).y,
				_vertexBuffer.GetVertex(v).z,
				1.0f);
			vertex = (*_transform) * vertex;
			_vertexBuffer.GetVertex(v) = Ogre::Vector3(vertex.x, vertex.y, vertex.z);
		}
	}

	void IMeshToCollisionShapeConverter::UpdateBounds()
	{
		const unsigned int vertexCount = _vertexBuffer.GetVertexCount();
		if (vertexCount > 0u)
		{
			const Ogre::Vector3 * const vertex = _vertexBuffer.GetData().data();

			Ogre::Vector3 vmin(vertex[0]);
			Ogre::Vector3 vmax(vertex[0]);

			for (unsigned int j = 1; j < vertexCount; ++j)
			{
				vmin.x = std::min(vmin.x, vertex[j].x);
				vmin.y = std::min(vmin.y, vertex[j].y);
				vmin.z = std::min(vmin.z, vertex[j].z);

				vmax.x = std::max(vmax.x, vertex[j].x);
				vmax.y = std::max(vmax.y, vertex[j].y);
				vmax.z = std::max(vmax.z, vertex[j].z);
			}

			_bounds->x = vmax.x - vmin.x;
			_bounds->y = vmax.y - vmin.y;
			_bounds->z = vmax.z - vmin.z;
			_boundRadius = (std::max(_bounds->x, std::max(_bounds->y, _bounds->z)) * 0.5f);
		}
	}

	void IMeshToCollisionShapeConverter::AddMesh(Ogre::Mesh* mesh)
	{

	}

	void IMeshToCollisionShapeConverter::AddEntity(Ogre::Entity* entity)
	{

	}

	void IMeshToCollisionShapeConverter::Clear()
	{
		_vertexBuffer.Clear();
		_indexBuffer.Clear();
		_transform = 0;
	}

	btSphereShape* IMeshToCollisionShapeConverter::CreateSphere()
	{
		return new btSphereShape(_boundRadius);
	}

	btBoxShape* IMeshToCollisionShapeConverter::CreateBox()
	{
		return new btBoxShape(btVector3(
			_bounds->x/2.0f,
			_bounds->y/2.0f,
			_bounds->z/2.0f));
	}

	btTriangleMeshShape* IMeshToCollisionShapeConverter::CreateTrimesh()
	{
		return 0;
	}

	btCylinderShape* IMeshToCollisionShapeConverter::CreateCylinder()
	{
		return new btCylinderShape(btVector3(
			_bounds->x/2.0f,
			_bounds->y/2.0f,
			_bounds->z/2.0f));
		return 0;
	}

	btCapsuleShape* IMeshToCollisionShapeConverter::CreateCapsule()
	{
		float d = std::max(_bounds->x, _bounds->z);
		return new btCapsuleShape(d*0.5f, _bounds->y - d);
	}

	btConvexHullShape* IMeshToCollisionShapeConverter::CreateConvexHull()
	{
		return new btConvexHullShape(
			(btScalar *)_vertexBuffer.GetData().data(),
			_vertexBuffer.GetVertexCount(),
			sizeof(Ogre::Vector3));
	}

	btCompoundShape* IMeshToCollisionShapeConverter::CreateBoxDecomposition()
	{
		return 0;
	}

	btCompoundShape* IMeshToCollisionShapeConverter::CreateCapsuleDecomposition()
	{
		return 0;
	}
}