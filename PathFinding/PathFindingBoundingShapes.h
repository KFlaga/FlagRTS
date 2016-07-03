#pragma once

#include "PathFindingPreReqs.h"
#include "PathFindingIDebugDrawer.h"

#undef min
#undef max

namespace PathFinding
{
	namespace Axes
	{
		enum Axis : int
		{
			X = 0,
			Y,
			Z
		};
	}

	struct Circle;
	// Simple bounding box
	struct Box
	{
		PFVector2 TopLeft;
		PFVector2 BotRight;

		// Creates uninitilazied box
		Box() { }

		Box(const PFVector2& topLeft, const PFVector2& botRight) :
			TopLeft(topLeft),
			BotRight(botRight)
		{ }

		// Creates box which contains both boxes
		Box(const Box& box1, const Box& box2);

		// Returns true if given point is inside box
		bool IsPointInside(const PFVector2& point) const;
		// Returns true if other box intersects this box
		bool Intersect(const Box& other) const;
		// Returns true if other circle intersects this box
		bool Intersect(const Circle& other) const;

		Circle GetBoundingCircle() const;

		// Returns size along given axis
		float GetExtend(Axes::Axis axis) const;
		// Returns half the size along given axis
		float GetHalfExtend(Axes::Axis axis) const;
		// Returns size of box
		PFVector2 GetExtends() const;
		// Returns area of box ( x * y )
		float GetArea() const;
		// Returns true of other box is fully inside this one
		bool Contains(const Box& other) const;
		// Expands corners, so that this box fully contains other one
		// Returns true if box actually expanded
		bool ExpandToFit(const Box& other);

		Box ExpandToFitCopy(const Box& other) const;

		float GetAreaExpanded(const Box& other) const;

		PFVector2 GetCenter() const;

		// Sets new center, extends remain the same
		void SetCenter(const PFVector2& center);
		// Sets new size, center remains the same
		void SetHalfExtends(const PFVector2& size);
		// Sets new size, center remains the same
		void SetExtends(const PFVector2& size);

		// Returns true if both boxes have same corners
		bool operator==(const Box& other) const
		{
			return TopLeft.squaredDistance(other.TopLeft) < VERTEX_ERROR_SQUARED &&
				BotRight.squaredDistance(other.BotRight)  < VERTEX_ERROR_SQUARED;
		}

		// Returns true if boxes have different corners
		bool operator!=(const Box& other) const
		{
			return TopLeft.squaredDistance(other.TopLeft) > VERTEX_ERROR_SQUARED ||
				BotRight.squaredDistance(other.BotRight) > VERTEX_ERROR_SQUARED;
		}

		void DebugDraw(IDebugDrawer* drawer) const;

		// Return distance between closest points of boxes
		float DistanceTo(const Box& other) const;
		// Return distance between closest points of boxes
		float DistanceToSquared(const Box& other) const;
		// Return distance between closest points of shapes
		float DistanceTo(const Circle& other) const;
		// Return distance between closest points of shapes
		float DistanceToSquared(const Circle& other) const;
		// Returns circle with same center and radius = max extend
		operator Circle() const;
	};

	// Simple bounding circle
	struct Circle
	{
		Circle() { }
		Circle(const PFVector2& center, float radius) :
			Center(center),
			Radius(radius)
		{ }

		PFVector2 Center;
		float Radius;

		// Returns true if given point is inside box
		bool IsPointInside(const PFVector2& point) const;
		// Returns true if other box intersects this box
		bool Intersect(const Box& other) const;
		// Returns true if other circle intersects this box
		bool Intersect(const Circle& other) const;

		Circle GetBoundingCircle() const;
		Box GetBoundingBox() const;

		// Returns size along given axis
		float GetExtend(Axes::Axis axis) const;
		// Returns half the size along given axis
		float GetHalfExtend(Axes::Axis axis) const;
		// Returns size of box
		PFVector2 GetExtends() const;
		// Returns area of box ( x * y )
		float GetArea() const;
		// Returns true of other box is fully inside this one
		bool Contains(const Circle& other) const;
		// Expands corners, so that this box fully contains other one
		// Returns true if box actually expanded
		bool ExpandToFit(const Circle& other);

		Circle ExpandToFitCopy(const Circle& other) const;

		float GetAreaExpanded(const Circle& other) const;

		PFVector2 GetCenter() const;

		// Sets new center, extends remain the same
		void SetCenter(const PFVector2& center);
		// Sets new size, center remains the same
		void SetHalfExtends(const PFVector2& size);
		// Sets new size, center remains the same
		void SetExtends(const PFVector2& size);

		// Returns true if both boxes have same corners
		bool operator==(const Circle& other) const
		{
			return Center.squaredDistance(other.Center) < VERTEX_ERROR_SQUARED &&
				(Radius-other.Radius)*(Radius-other.Radius)  < VERTEX_ERROR_SQUARED;
		}

		// Returns true if boxes have different corners
		bool operator!=(const Circle& other) const
		{
			return Center.squaredDistance(other.Center) >= VERTEX_ERROR_SQUARED ||
				(Radius-other.Radius)*(Radius-other.Radius) >= VERTEX_ERROR_SQUARED;
		}

		void DebugDraw(IDebugDrawer* drawer) const;

		// Return distance between closest points of boxes
		float DistanceTo(const Circle& other) const;
		// Return distance between closest points of boxes
		float DistanceToSquared(const Circle& other) const;
		// Return distance between closest points of boxes
		float DistanceTo(const Box& other) const;
		// Return distance between closest points of boxes
		float DistanceToSquared(const Box& other) const;
		// Returns square with same center and half extends = radius
		operator Box() const
		{
			return Box(GetCenter() - PFVector2(Radius,Radius), GetCenter() + PFVector2(Radius,Radius));
		}
	};

	inline bool Box::IsPointInside(const PFVector2& point) const
	{
		return !(point.x < TopLeft.x - VERTEX_ERROR || 
			point.x > BotRight.x + VERTEX_ERROR ||
			point.y < TopLeft.y - VERTEX_ERROR ||
			point.y > BotRight.y + VERTEX_ERROR );
	}

	inline bool Box::Intersect(const Box& other) const
	{
		return !(other.TopLeft.x > BotRight.x + VERTEX_ERROR ||
			other.BotRight.x < TopLeft.x - VERTEX_ERROR ||
			other.TopLeft.y > BotRight.y + VERTEX_ERROR ||
			other.BotRight.y < TopLeft.y - VERTEX_ERROR );
	}

	inline bool Box::Intersect(const Circle& other) const
	{
		return other.Intersect(*this);
	}

	inline float Box::GetExtend(Axes::Axis axis) const
	{
		PFAssert( axis != Axes::Z, "" );
		PFVector2 v = BotRight - TopLeft;
		return reinterpret_cast<float*>(&v)[axis];
	}

	inline PFVector2 Box::GetExtends() const
	{
		return BotRight - TopLeft;
	}

	inline float Box::GetHalfExtend(Axes::Axis axis) const
	{
		return GetExtend(axis) * 0.5f;
	}

	inline float Box::GetArea() const
	{
		PFVector2 v = BotRight - TopLeft;
		return v.x * v.y;
	}

	inline bool Box::Contains(const Box& other) const
	{
		return !(TopLeft.x > other.TopLeft.x ||
			TopLeft.y > other.TopLeft.y ||
			BotRight.x < other.BotRight.x ||
			BotRight.y < other.BotRight.y);
	}

	inline bool Box::ExpandToFit(const Box& other)
	{
		bool expanded = false;
		if( TopLeft.x > other.TopLeft.x )
		{
			TopLeft.x = other.TopLeft.x;
			expanded = true;
		}
		if( TopLeft.y > other.TopLeft.y )
		{
			TopLeft.y = other.TopLeft.y;
			expanded = true;
		}
		if( BotRight.x < other.BotRight.x )
		{
			BotRight.x = other.BotRight.x;
			expanded = true;
		}
		if( BotRight.y < other.BotRight.y )
		{
			BotRight.y = other.BotRight.y;
			expanded = true;
		}
		return expanded;
	}

	inline Box Box::ExpandToFitCopy(const Box& other) const
	{
		return Box( PFVector2(std::min( TopLeft.x, other.TopLeft.x ), 
			std::min(TopLeft.y, other.TopLeft.y)),
			PFVector2(std::max(BotRight.x, other.BotRight.x),
			std::max(BotRight.y, other.BotRight.y)));
	}

	inline float Box::GetAreaExpanded(const Box& other) const
	{
		return (std::max(BotRight.x, other.BotRight.x) - std::min(TopLeft.x, other.TopLeft.x)) * 
			(std::max(BotRight.y, other.BotRight.y) - std::min(TopLeft.y, other.TopLeft.y));
	}

	inline PFVector2 Box::GetCenter() const
	{
		return (TopLeft + BotRight) * 0.5f;
	}

	inline void Box::SetCenter(const PFVector2& center)
	{
		PFVector2 hextends(GetExtends() * 0.5f);
		TopLeft = center - hextends;
		BotRight = center + hextends;
	}

	inline void Box::SetHalfExtends(const PFVector2& size)
	{
		PFVector2 center(GetCenter());
		TopLeft = center - size;
		BotRight = center + size;
	}

	inline void Box::SetExtends(const PFVector2& size)
	{
		PFVector2 center(GetCenter());
		TopLeft = center - size * 0.5f;
		BotRight = center + size * 0.5f;
	}

	// Dist = Len(Max((0, 0), Abs(Center - otherCenter) - (Extent + otherExtent)))
	inline float Box::DistanceTo(const Box& other) const
	{
		float dist = DistanceToSquared(other);
		if ( dist < 0.f )
			return -std::sqrtf(-dist);
		return std::sqrtf(dist);
	}

	inline float Box::DistanceToSquared(const Box& other) const
	{
		float lenX = (std::abs((TopLeft.x+BotRight.x) - (other.TopLeft.x+other.BotRight.x)) -
			BotRight.x + TopLeft.x - other.BotRight.x + other.TopLeft.x) * 0.5f;
		float lenY = (std::abs((TopLeft.y+BotRight.y) - (other.TopLeft.y+other.BotRight.y)) -
			BotRight.y + TopLeft.y - other.BotRight.y + other.TopLeft.y) * 0.5f;
		if( lenX < VERTEX_ERROR_NEGATIVE && lenY < VERTEX_ERROR_NEGATIVE )
		{
			lenX = std::max(0.f, -lenX);
			lenY = std::max(0.f, -lenY);
			return -(lenX * lenX + lenY * lenY);
		}
		else
		{
			lenX = std::max(0.f, lenX);
			lenY = std::max(0.f, lenY);
			return (lenX * lenX + lenY * lenY);
		}
	}

	// Dist = Len(Max((0, 0), Abs(Center - otherCenter) - (Extent + otherExtent)))
	inline float Box::DistanceTo(const Circle& other) const
	{
		return other.DistanceTo(*this);
	}

	inline float Box::DistanceToSquared(const Circle& other) const
	{
		return other.DistanceToSquared(*this);
	}

	inline void Box::DebugDraw(IDebugDrawer* drawer) const
	{
		drawer->DrawBox(TopLeft, PFVector2(TopLeft.x, BotRight.y), BotRight, PFVector2(BotRight.x, TopLeft.y));
	}

	inline Box::operator Circle() const
	{
		return Circle(GetCenter(), std::max(GetHalfExtend(Axes::X),GetHalfExtend(Axes::Y)));
	}

	inline bool Circle::IsPointInside(const PFVector2& point) const
	{
		return point.squaredDistance(Center) <= Radius*Radius - VERTEX_ERROR_SQUARED;
	}

	inline bool Circle::Intersect(const Box& other) const
	{
		if(GetBoundingBox().Intersect(other))
		{
			float radius2 = Radius*Radius;
			if( Center.squaredDistance(other.TopLeft) <= radius2 - VERTEX_ERROR_SQUARED ||
				Center.squaredDistance(other.BotRight) <= radius2 - VERTEX_ERROR_SQUARED ||
				Center.squaredDistance(PFVector2(other.TopLeft.x, other.BotRight.y)) <= radius2 - VERTEX_ERROR_SQUARED ||
				Center.squaredDistance(PFVector2(other.TopLeft.y, other.BotRight.x)) <= radius2 - VERTEX_ERROR_SQUARED 
				)
				return true;
		}
		return false;
	}

	inline bool Circle::Intersect(const Circle& other) const
	{
		return other.Center.squaredDistance(Center) <=
			(other.Radius+Radius)*(other.Radius+Radius) - VERTEX_ERROR_SQUARED;
	}

	inline Box Circle::GetBoundingBox() const
	{
		return Box(Center - Radius, Center + Radius);
	}

	inline float Circle::GetExtend(Axes::Axis axis) const
	{
		PFAssert( axis != Axes::Z, "" );
		return Radius * 2.0f;
	}

	inline PFVector2 Circle::GetExtends() const
	{
		return PFVector2(2*Radius, 2*Radius);
	}

	inline float Circle::GetHalfExtend(Axes::Axis axis) const
	{
		PFAssert( axis != Axes::Z, "" );
		return Radius;
	}

	inline float Circle::GetArea() const
	{
		return PI * Radius * Radius;
	}

	inline bool Circle::Contains(const Circle& other) const
	{
		return Radius > other.Radius + Center.distance(other.Center);
	}

	inline bool Circle::ExpandToFit(const Circle& other)
	{
		float ccDist = Center.distance(other.Center);
		if( Radius > other.Radius + ccDist )
			return false;

		// New diameter = c1c2 distance + r1 + r2
		// New center = 0.5(c1+c2) + (r2-r1)(c2-c1)/ccDist
		Center = 0.5f * ((Center + other.Center) + (Radius - other.Radius)*(Center - other.Center) / ccDist);
		Radius = 0.5f * (ccDist + Radius + other.Radius);

		return true;
	}

	inline Circle Circle::ExpandToFitCopy(const Circle& other) const
	{
		float ccDist = Center.distance(other.Center);
		return Circle(0.5f * ((Center + other.Center) + 
			(Radius - other.Radius)*(Center - other.Center) / ccDist),
			0.5f * (ccDist + Radius + other.Radius));
	}

	inline float Circle::GetAreaExpanded(const Circle& other) const
	{
		float newRadius = 0.5f * (Center.distance(other.Center) + Radius + other.Radius);
		return PI * Radius * Radius;
	}

	inline PFVector2 Circle::GetCenter() const
	{
		return Center;
	}

	inline void Circle::SetCenter(const PFVector2& center)
	{
		Center = center;
	}

	inline void Circle::SetHalfExtends(const PFVector2& size)
	{
		Radius = size.x;
	}

	inline void Circle::SetExtends(const PFVector2& size)
	{
		Radius = size.x * 0.5f;
	}

	inline float Circle::DistanceTo(const Circle& other) const
	{
		//		// clamp(value, min, max) - limits value to the range min..max
		//
		//// Find the closest point to the circle within the rectangle
		//float closestX = clamp(circle.X, rectangle.Left, rectangle.Right);
		//float closestY = clamp(circle.Y, rectangle.Top, rectangle.Bottom);
		//
		//// Calculate the distance between the circle's center and this closest point
		//float distanceX = circle.X - closestX;
		//float distanceY = circle.Y - closestY;
		//
		//// If the distance is less than the circle's radius, an intersection occurs
		//float distanceSquared = (distanceX * distanceX) + (distanceY * distanceY);
		//return distanceSquared < (circle.Radius * circle.Radius);

		return Center.distance(other.Center) - Radius - other.Radius;
	}

	inline float Circle::DistanceToSquared(const Circle& other) const
	{
		float d = DistanceTo(other) ;
		return d*d;
	}

	inline float Circle::DistanceToSquared(const Box& other) const
	{
		return Center.squaredDistance(other.TopLeft) - Radius - other.TopLeft.x; //TODO is this ok??
	}

	inline float Circle::DistanceTo(const Box& other) const
	{
		float dist = DistanceToSquared(other);
		if ( dist < 0.f )
			return -std::sqrtf(-dist);
		return std::sqrtf(dist);
	}

	inline void Circle::DebugDraw(IDebugDrawer* drawer) const
	{
		// 16 vertices per circle
		int vCount = 16;
		float dAng = 2.f*PI/(float)vCount;
		for(int i = 0; i < vCount; ++i)
		{
			PFVector2 p1 = PFVector2(GetCenter() + Radius * PFVector2(
				std::sinf(i*dAng) , std::cosf(i*dAng)));
			PFVector2 p2 = PFVector2(GetCenter() + Radius * PFVector2(
				std::sinf((i+1)*dAng) , std::cosf((i+1)*dAng)));
			drawer->DrawLine(p1,p2);
		}
	}
}