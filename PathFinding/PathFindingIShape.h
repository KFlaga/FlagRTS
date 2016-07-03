#pragma once

#include "PathFindingPreReqs.h"

namespace PathFinding
{
	class IDebugDrawer;

	namespace Axes
	{
		enum Axis : int
		{
			X = 0,
			Y,
			Z
		};
	}

	// Generic 2d shape used for Dbvh
	class IShape2d
	{
		// Returns true if given point is inside shape
		virtual bool IsPointInside(const PFVector2& point) const { return false; }
		// Returns true if other shape intersects this shape
		virtual bool Intersect(const IShape2d& other) const { return false; }
		// Returns size along given axis
		virtual float GetExtend(Axes::Axis axis) const { return 0.f; }
		// Returns half the size along given axis
		virtual float GetHalfExtend(Axes::Axis axis) const { return 0.f; }
		// Returns size of shape
		virtual PFVector2 GetExtends() const { return PFVector2(0.f, 0.f); }
		// Returns area of shape
		virtual float GetArea() const { return 0.f; }
		// Returns true of other shape is fully inside this one
		virtual bool Contains(const IShape2d& other) const { return false; }
		// Expands shape, so that this box fully contains other one
		// Returns true if shape actually expanded
		virtual bool ExpandToFit(const IShape2d& other) { return false; }
		// Saves expanded shape in 'copy'
		virtual void ExpandToFitCopy(const IShape2d& other, IShape2d& copy) const { }
		// Returns area of expanded shape
		virtual float GetAreaExpanded(const IShape2d& other) const { return 0.f; }
		// Returns center of this shape
		virtual PFVector2 GetCenter() const { return PFVector2(0.f, 0.f); }
		// Sets new center of shape ( extends remains the same )
		virtual void SetCenter(const PFVector2& center) const { }
		// Sets new extends of shape ( center remains the same )
		virtual void SetExtends(const PFVector2& center) const { }
		// Draws shape
		virtual void DebugDraw(IDebugDrawer* drawer) const { }
	};


}