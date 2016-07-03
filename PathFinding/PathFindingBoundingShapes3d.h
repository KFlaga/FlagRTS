#pragma once

#include "PathFindingPreReqs.h"
#include "PathFindingIDebugDrawer.h"

namespace PathFinding
{
	struct Box3d
	{
		PFVector3 Minimum;
		PFVector3 Maximum;

		// Creates uninitilazied box
		Box3d() { }

		Box3d(const PFVector3& min, const PFVector3& max) :
			Minimum(min),
			Maximum(max)
		{ }

		// Returns true if given point is inside box
		bool IsPointInside(const PFVector3& point) const;
		// Returns true if other box intersects this box
		bool Intersect(const Box3d& other) const;

		// Returns size along given axis
		float GetExtend(Axes::Axis axis) const;
		// Returns half the size along given axis
		float GetHalfExtend(Axes::Axis axis) const;
		// Returns size of box
		PFVector3 GetExtends() const;
		// Returns area of box ( x * y )
		float GetArea() const;
		// Returns true of other box is fully inside this one
		bool Contains(const Box3d& other) const;
		// Expands corners, so that this box fully contains other one
		// Returns true if box actually expanded
		bool ExpandToFit(const Box3d& other);

		Box3d ExpandToFitCopy(const Box3d& other) const;

		float GetAreaExpanded(const Box3d& other) const;

		PFVector3 GetCenter() const;

		// Returns true if both boxes have same corners
		bool operator==(const Box3d& other) const
		{
			return Minimum.squaredDistance(other.Minimum) < VERTEX_ERROR_SQUARED &&
				Maximum.squaredDistance(other.Maximum)  < VERTEX_ERROR_SQUARED;
		}

		// Returns true if boxes have different corners
		bool operator!=(const Box3d& other) const
		{
			return Minimum.squaredDistance(other.Minimum) > VERTEX_ERROR_SQUARED ||
				Maximum.squaredDistance(other.Maximum) > VERTEX_ERROR_SQUARED;
		}

		void DebugDraw(IDebugDrawer* drawer) const;

		// Return distance between closest points of boxes
		float DistanceTo(const Box3d& other);
		// Return distance between closest points of boxes
		float DistanceToSquared(const Box3d& other);
	};

	inline bool Box3d::IsPointInside(const PFVector3& point) const
	{
		return !(point.x < Minimum.x - VERTEX_ERROR || 
			point.x > Maximum.x + VERTEX_ERROR ||
			point.y < Minimum.y - VERTEX_ERROR ||
			point.y > Maximum.y + VERTEX_ERROR ||
			point.z < Minimum.z - VERTEX_ERROR ||
			point.z > Maximum.z + VERTEX_ERROR );
	}

	inline bool Box3d::Intersect(const Box3d& other) const
	{
		return !(other.Minimum.x > Maximum.x + VERTEX_ERROR ||
			other.Maximum.x < Minimum.x - VERTEX_ERROR ||
			other.Minimum.y > Maximum.y + VERTEX_ERROR ||
			other.Maximum.y < Minimum.y - VERTEX_ERROR ||
			other.Minimum.z > Maximum.z + VERTEX_ERROR ||
			other.Maximum.z < Minimum.z - VERTEX_ERROR);
	}

	inline float Box3d::GetExtend(Axes::Axis axis) const
	{
		PFVector3 v = Maximum - Minimum;
		return reinterpret_cast<float*>(&v)[axis];
	}

	inline PFVector3 Box3d::GetExtends() const
	{
		return Maximum - Minimum;
	}

	inline float Box3d::GetHalfExtend(Axes::Axis axis) const
	{
		return GetExtend(axis) * 0.5f;
	}

	inline float Box3d::GetArea() const
	{
		PFVector3 v = Maximum - Minimum;
		return 2.f*(v.x * v.y + v.x * v.z + v.y * v.z);
	}

	inline bool Box3d::Contains(const Box3d& other) const
	{
		return !(Minimum.x > other.Minimum.x ||
			Minimum.y > other.Minimum.y ||
			Minimum.z > other.Minimum.z ||
			Maximum.x < other.Maximum.x ||
			Maximum.y < other.Maximum.y ||
			Maximum.z < other.Maximum.z );
	}

	inline bool Box3d::ExpandToFit(const Box3d& other)
	{
		bool expanded = false;
		if( Minimum.x > other.Minimum.x )
		{
			Minimum.x = other.Minimum.x;
			expanded = true;
		}
		if( Minimum.y > other.Minimum.y )
		{
			Minimum.y = other.Minimum.y;
			expanded = true;
		}
		if( Minimum.z > other.Minimum.z )
		{
			Minimum.z = other.Minimum.z;
			expanded = true;
		}
		if( Maximum.x < other.Maximum.x )
		{
			Maximum.x = other.Maximum.x;
			expanded = true;
		}
		if( Maximum.y < other.Maximum.y )
		{
			Maximum.y = other.Maximum.y;
			expanded = true;
		}
		if( Maximum.z < other.Maximum.z )
		{
			Maximum.y = other.Maximum.y;
			expanded = true;
		}
		return expanded;
	}

	inline Box3d Box3d::ExpandToFitCopy(const Box3d& other) const
	{
		return Box3d( PFVector3(std::min( Minimum.x, other.Minimum.x ), 
			std::min(Minimum.y, other.Minimum.y), std::min(Minimum.z, other.Minimum.z)),
			PFVector3(std::max(Maximum.x, other.Maximum.x),
			std::max(Maximum.y, other.Maximum.y), std::max(Maximum.z, other.Maximum.z)));
	}

	inline float Box3d::GetAreaExpanded(const Box3d& other) const
	{
		return ExpandToFitCopy(other).GetArea();
	}

	inline PFVector3 Box3d::GetCenter() const
	{
		return (Minimum + Maximum) * 0.5f;
	}

	// Dist = Len(Max((0, 0), Abs(Center - otherCenter) - (Extent + otherExtent)))
	inline float Box3d::DistanceTo(const Box3d& other)
	{
		return std::sqrtf(DistanceToSquared(other));
	}

	inline float Box3d::DistanceToSquared(const Box3d& other)
	{
		/*float lenX = std::max(0.f, std::abs((TopLeft.x+BotRight.x) - (other.TopLeft.x+other.BotRight.x)) -
		BotRight.x + TopLeft.x - other.BotRight.x + other.TopLeft.x) * 0.5f;
		float lenY = std::max(0.f, std::abs((TopLeft.y+BotRight.y) - (other.TopLeft.y+other.BotRight.y)) -
		BotRight.y + TopLeft.y - other.BotRight.y + other.TopLeft.y) * 0.5f;
		return (lenX * lenX + lenY * lenY);*/
		return 0.f;
	}

	inline void Box3d::DebugDraw(IDebugDrawer* drawer) const
	{
		drawer->DrawBox3d(PFVector3(Minimum.x, Maximum.y, Minimum.z),
			PFVector3(Minimum.x, Maximum.y, Maximum.z),
			Maximum,
			PFVector3(Maximum.x, Maximum.y, Minimum.z),
			Minimum,
			PFVector3(Minimum.x, Minimum.y, Maximum.z),
			PFVector3(Maximum.x, Minimum.y, Maximum.z),
			PFVector3(Maximum.x, Minimum.y, Minimum.z));
	}
}