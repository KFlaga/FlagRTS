#pragma once

#include "DataTypes.h"
#include <PathFindingIPathUnit.h>
#include "PathingSystem.h"
#include "PhysicalObject.h"
#include "CollisionShapes.h"

namespace FlagRTS
{
	class PathUnit
	{
	public:
		PathFinding::IPathUnit<Vector2>* Unit;
		PhysicalObject* Owner;
		int ShapeType;
		Vector2 FinalGoal;
		Array<Vector2> Path;
		IntVector2 CellSize; // Used for GPF only
		int NextPathIdx; 
		bool HavePathRequestPending;
		bool FinishedMove;

		PathUnit(PathFinding::IPathUnit<Vector2>* ipathUnit) :
			Unit(ipathUnit),
			NextPathIdx(0),
			HavePathRequestPending(false),
			FinishedMove(true),
			FinalGoal(0.f,0.f)
		{ }
	};

	class BoxPathUnit : public PathUnit
	{
	public:
		typedef PathFinding::BoundedPathUnit2d<PathFinding::Box, PathFinding::Box> UnitT;
		PathFinding::Box Shape;

		BoxPathUnit(const int filter) :
			PathUnit(new PathFinding::BoundedPathUnit2d<>(filter))
		{ 
			ShapeType = CollisionShapes::Box;
		}

		UnitT* GetBoundedUnit()
		{
			return static_cast<UnitT*>(Unit);
		}

		void SetShape(PhysicalObject* object)
		{
			auto& pos = object->GetPositionAbsolute();
			auto& hsize = object->GetHalfSize();
			Shape = PathFinding::Box(PFVector2(pos.x - hsize.x, pos.z - hsize.z), 
				PFVector2(pos.x + hsize.x, pos.z + hsize.z));
			PathFinding::Box LookShape = Shape;
			LookShape.SetExtends( LookShape.GetExtends() * 2.f );
			GetBoundedUnit()->SetCollisionShape(Shape);
			GetBoundedUnit()->SetLookUpShape(LookShape);
		}

		void UpdateShape(PhysicalObject* object)
		{
			auto& min = object->GetBoudningBox().getMinimum();
			auto& max = object->GetBoudningBox().getMaximum();
			Shape.TopLeft = PFVector2(min.x, min.z);
			Shape.BotRight = PFVector2(max.x, max.z);
			PathFinding::Box LookShape = Shape;
			LookShape.SetExtends( LookShape.GetExtends() * 2.f );
			GetBoundedUnit()->SetCollisionShape(Shape);
			GetBoundedUnit()->SetLookUpShape(LookShape);
		}

		PathFinding::Box& GetShape() { return Shape; }
	};

	class CirclePathUnit : public PathUnit
	{
	public:
		typedef PathFinding::BoundedPathUnit2d<PathFinding::Circle, PathFinding::Circle> UnitT;
		PathFinding::Circle Shape;

		CirclePathUnit(const int filter) :
			PathUnit(new UnitT(filter))
		{ 
			ShapeType = CollisionShapes::Circle;
		}

		UnitT* GetBoundedUnit()
		{
			return static_cast<UnitT*>(Unit);
		}

		void SetShape(PhysicalObject* object)
		{
			auto& pos = object->GetPositionAbsolute();
			auto& hsize = object->GetHalfSize();
			Shape = PathFinding::Circle(PFVector2(pos.x,pos.z), std::max(hsize.x, hsize.z));
			PathFinding::Circle LookShape = Shape;
			LookShape.SetExtends( LookShape.GetExtends() * 2.f );
			GetBoundedUnit()->SetCollisionShape(Shape);
			GetBoundedUnit()->SetLookUpShape(LookShape);
		}

		void UpdateShape(PhysicalObject* object)
		{
			auto& pos = object->GetPositionAbsolute();
			auto& hsize = object->GetHalfSize();
			Shape = PathFinding::Circle(PFVector2(pos.x,pos.z), std::max(hsize.x, hsize.z));
			PathFinding::Circle LookShape = Shape;
			LookShape.Radius = Shape.Radius * 2.f;
			GetBoundedUnit()->SetCollisionShape(Shape);
			GetBoundedUnit()->SetLookUpShape(LookShape);
		}

		PathFinding::Circle& GetShape() { return Shape; }
	};

	inline PathUnit* CreateShapedPathUnit(CollisionShapeType shape, const int filter)
	{
		if(shape == CollisionShapes::Box)
			return xNew1(BoxPathUnit,filter);
		else if(shape == CollisionShapes::Circle)
			return xNew1(CirclePathUnit,filter);
		return 0;
	}

	inline void SetPathUnitShape(PathUnit* pathUnit, PhysicalObject* object)
	{
		if(pathUnit->ShapeType == CollisionShapes::Box)
			static_cast<BoxPathUnit*>(pathUnit)->SetShape(object);
		else if(pathUnit->ShapeType == CollisionShapes::Circle)
			static_cast<CirclePathUnit*>(pathUnit)->SetShape(object);
	}

	inline void UpdatePathUnitShape(PathUnit* pathUnit, PhysicalObject* object)
	{
		if(pathUnit->ShapeType == CollisionShapes::Box)
			static_cast<BoxPathUnit*>(pathUnit)->UpdateShape(object);
		else if(pathUnit->ShapeType == CollisionShapes::Circle)
			static_cast<CirclePathUnit*>(pathUnit)->UpdateShape(object);
	}

	inline PathFinding::Box GetPathUnitBoundingBox(PathUnit* pathUnit)
	{
		if(pathUnit->ShapeType == CollisionShapes::Box)
			return static_cast<BoxPathUnit*>(pathUnit)->GetShape();
		else if(pathUnit->ShapeType == CollisionShapes::Circle)
			return static_cast<CirclePathUnit*>(pathUnit)->GetShape();
	}

	inline PathFinding::Circle GetPathUnitBoundingCircle(PathUnit* pathUnit)
	{
		if(pathUnit->ShapeType == CollisionShapes::Box)
		{
			PathFinding::Box& shape = static_cast<BoxPathUnit*>(pathUnit)->GetShape();
			return PathFinding::Circle(shape.GetCenter(), 
				std::max(shape.GetHalfExtend(PathFinding::Axes::X), shape.GetHalfExtend(PathFinding::Axes::Y)));
		}
		else if(pathUnit->ShapeType == CollisionShapes::Circle)
			return static_cast<CirclePathUnit*>(pathUnit)->GetShape();
	}
}