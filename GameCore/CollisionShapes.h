#pragma once

#include "BoundingShapes.h"
#include "CollisionFilter.h"

namespace FlagRTS
{
	namespace CollisionShapes
	{
		enum CollisionShapeType : int
		{
			None = 0,
			Box,
			Circle,
			Footprint,
			Box3d,
			Sphere,
			Cone2d,
			Cone3d,
			Hull2d,
			Hull3d,
			OOBox,
			OOBoxd3,
			OOCone2d,
			OOCone3d
		};

		CollisionShapeType ParseCollisionShapeType(const char* name);
	}
	typedef CollisionShapes::CollisionShapeType CollisionShapeType;

	class CollisionShape
	{
		BoundingBox _bbox;
		//Quaternion _orientation;
		Vector3 _center;
		Vector3 _halfExtends;
		CollisionFilter _collisionFilter;
		CollisionShapeType _shapeType;

	public:
		CollisionShape();
		CollisionShape(
			CollisionFilter collisionFilter,
			CollisionShapeType shape, 
			const Vector3& center, 
			const Vector3& halfExtends);

		BoundingBox& GetBoundingBox() { return _bbox; }

		void SetCenter(const Vector3& vec) 
		{ 
			_center = vec;
			UpdateBoundingBoxSize();
		}

		const Vector3& GetCenter() const { return _center; }

		void SetHalfExtends(const Vector3& vec) 
		{ 
			_halfExtends = vec; 
			UpdateBoundingBoxSize();
		}

		const Vector3& GetHalfExtends() const { return _halfExtends; }

		void SetExtends(const Vector3& vec) 
		{ 
			SetHalfExtends(vec * 0.5f); 
		}

		Vector3 GetExtends() const
		{
			return _halfExtends * 2.0f;
		}

		void UpdateBoundingBoxSize()
		{
			_bbox.TopLeft = Vector2(_center.x - _halfExtends.x,
				_center.z - _halfExtends.z);
			_bbox.BotRight = Vector2(_center.x + _halfExtends.x,
				_center.z + _halfExtends.z);
		}

		void SetShapeType(CollisionShapeType shape) { _shapeType = shape; } 
		CollisionShapeType GetShapeType() const { return _shapeType; }

		void SetCollisionFilter(int filter) { _collisionFilter = filter; }
		int GetCollisionFilter() const { return _collisionFilter; }

		bool CheckCollidesWith(const CollisionShape& otherShape) const;
		
		float DistanceTo(const CollisionShape& otherShape) const;
		float DistanceToSquared(const CollisionShape& otherShape) const;
	};

	struct ObjectOrientedBoundingBox
	{
		Vector2 V1, V2, V3, V4;

		ObjectOrientedBoundingBox();
		ObjectOrientedBoundingBox(Vector2* fourCorners);
		ObjectOrientedBoundingBox(BoundingBox aabb, Quaternion orientation);

		operator Vector2*() { return &V1; }
	};
}