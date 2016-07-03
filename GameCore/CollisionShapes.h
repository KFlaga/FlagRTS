#pragma once

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
};