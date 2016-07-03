#pragma once

#include "PathFindingBoundingShapes.h"

namespace PathFinding
{
	//// Able to hold any shape
	//class IShapeHolder
	//{
	//public:
	//	// Returns true if given point is inside shape
	//	virtual bool IsPointInside(const PFVector2& point) const { return false; }
	//	// Returns true if other shape intersects this shape
	//	virtual bool Intersect(const IShapeHolder& other) const { return false; }
	//	// Returns size along given axis
	//	virtual float GetExtend(Axes::Axis axis) const { return 0.f; }
	//	// Returns half the size along given axis
	//	virtual float GetHalfExtend(Axes::Axis axis) const { return 0.f; }
	//	// Returns size of shape
	//	virtual PFVector2 GetExtends() const { return PFVector2(0.f, 0.f); }
	//	// Returns area of shape
	//	virtual float GetArea() const { return 0.f; }
	//	// Returns true of other shape is fully inside this one
	//	virtual bool Contains(const IShapeHolder& other) const { return false; }
	//	// Expands shape, so that this box fully contains other one
	//	// Returns true if shape actually expanded
	//	virtual bool ExpandToFit(const IShapeHolder& other) { return false; }
	//	// Saves expanded shape in 'copy'
	//	virtual void ExpandToFitCopy(const IShapeHolder& other, IShapeHolder& copy) const { }
	//	// Returns area of expanded shape
	//	virtual float GetAreaExpanded(const IShapeHolder& other) const { return 0.f; }
	//	// Returns center of this shape
	//	virtual PFVector2 GetCenter() const { return PFVector2(0.f, 0.f); }
	//	// Sets new center of shape ( extends remains the same )
	//	virtual void SetCenter(const PFVector2& center) const { }
	//	// Sets new extends of shape ( center remains the same )
	//	virtual void SetExtends(const PFVector2& center) const { }
	//	// Draws shape
	//	virtual void DebugDraw(IDebugDrawer* drawer) const { }

	//	template<class ShapeT>
	//	virtual ShapeT& GetShape() const = 0;
	//};

	//template<typename ShapeT>
	//class ShapeHolder : public IShapeHolder
	//{
	//public:
	//	typedef ShapeT ShapeType;

	//protected:
	//	ShapeT _shape;

	//public:
	//	ShapeHolder() { }
	//	ShapeHolder(const ShapeT& shape) :
	//		_shape(shape)
	//	{ }

	//	// Returns true if given point is inside shape
	//	bool IsPointInside(const PFVector2& point) const { return _shape.IsPointInside(point); }
	//	// Returns true if other shape intersects this shape
	//	bool Intersect(const IShapeHolder& other) const { return _shape.Intersect(other.GetShape<ShapeT>()); }
	//	// Returns size along given axis
	//	float GetExtend(Axes::Axis axis) const { return _shape.GetExtend(axis); }
	//	// Returns half the size along given axis
	//	float GetHalfExtend(Axes::Axis axis) const { return _shape.GetHalfExtend(axis); }
	//	// Returns size of shape
	//	PFVector2 GetExtends() const { return _shape.GetExtends(); }
	//	// Returns area of shape
	//	float GetArea() const { return _shape.GetArea(); }
	//	// Returns true of other shape is fully inside this one
	//	bool Contains(const IShapeHolder& other) const { return _shape.Contains(other._shape); }
	//	// Expands shape, so that this box fully contains other one
	//	// Returns true if shape actually expanded
	//	bool ExpandToFit(const IShapeHolder& other) { return false; }
	//	// Saves expanded shape in 'copy'
	//	void ExpandToFitCopy(const IShapeHolder& other, IShapeHolder& copy) const { }
	//	// Returns area of expanded shape
	//	float GetAreaExpanded(const IShapeHolder& other) const { return 0.f; }
	//	// Returns center of this shape
	//	PFVector2 GetCenter() const { return PFVector2(0.f, 0.f); }
	//	// Sets new center of shape ( extends remains the same )
	//	void SetCenter(const PFVector2& center) const { }
	//	// Sets new extends of shape ( center remains the same )
	//	void SetExtends(const PFVector2& center) const { }
	//	// Draws shape
	//	void DebugDraw(IDebugDrawer* drawer) const { }
	//};

	//class ShapeAdapter
	//{
	//	IShapeHolder* _shapeHolder;

	//	// Returns true if given point is inside shape
	//	bool IsPointInside(const PFVector2& point) const { return _shape.IsPointInside(point); }
	//	// Returns true if other shape intersects this shape
	//	bool Intersect(const ShapeAdapter& other) const { return _shapeHolder->Intersect(*other._shapeHolder); }
	//	// Returns size along given axis
	//	float GetExtend(Axes::Axis axis) const { return _shape.GetExtend(axis); }
	//	// Returns half the size along given axis
	//	float GetHalfExtend(Axes::Axis axis) const { return _shape.GetHalfExtend(axis); }
	//	// Returns size of shape
	//	PFVector2 GetExtends() const { return _shape.GetExtends(); }
	//	// Returns area of shape
	//	float GetArea() const { return _shape.GetArea(); }
	//	// Returns true of other shape is fully inside this one
	//	bool Contains(const IShapeHolder& other) const { return _shape.Contains(other._shape); }
	//	// Expands shape, so that this box fully contains other one
	//	// Returns true if shape actually expanded
	//	bool ExpandToFit(const IShapeHolder& other) { return false; }
	//	// Saves expanded shape in 'copy'
	//	void ExpandToFitCopy(const IShapeHolder& other, IShapeHolder& copy) const { }
	//	// Returns area of expanded shape
	//	float GetAreaExpanded(const IShapeHolder& other) const { return 0.f; }
	//	// Returns center of this shape
	//	PFVector2 GetCenter() const { return PFVector2(0.f, 0.f); }
	//	// Sets new center of shape ( extends remains the same )
	//	void SetCenter(const PFVector2& center) const { }
	//	// Sets new extends of shape ( center remains the same )
	//	void SetExtends(const PFVector2& center) const { }
	//	// Draws shape
	//	void DebugDraw(IDebugDrawer* drawer) const { }
	//};
}