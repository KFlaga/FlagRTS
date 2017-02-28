#pragma once

#include "PathingDefs.h"
#include "PathingDrawer.h"

#undef min
#undef max

namespace FlagRTS
{
	struct BoundingCircle;
	// Simple bounding box
	struct BoundingBox
	{
		Vector2 TopLeft;
		Vector2 BotRight;

		// Creates uninitilazied box
		BoundingBox() { }

		BoundingBox(const Vector2& topLeft, const Vector2& botRight) :
			TopLeft(topLeft),
			BotRight(botRight)
		{ }

		// Creates box which contains both boxes
		BoundingBox(const BoundingBox& box1, const BoundingBox& box2);

		// Returns true if given point is inside box
		bool IsPointInside(const Vector2& point) const;
		// Returns true if other box intersects this box
		bool Intersect(const BoundingBox& other) const;
		// Returns true if other circle intersects this box
		bool Intersect(const BoundingCircle& other) const;

		BoundingCircle GetBoundingCircle() const;

		// Returns size along given axis
		float GetExtend(Axes::Axis axis) const;
		// Returns half the size along given axis
		float GetHalfExtend(Axes::Axis axis) const;
		// Returns size of box
		Vector2 GetExtends() const;
		// Returns area of box ( x * y )
		float GetArea() const;
		// Returns true of other box is fully inside this one
		bool Contains(const BoundingBox& other) const;
		// Expands corners, so that this box fully contains other one
		// Returns true if box actually expanded
		bool ExpandToFit(const BoundingBox& other);

		BoundingBox ExpandToFitCopy(const BoundingBox& other) const;

		float GetAreaExpanded(const BoundingBox& other) const;

		Vector2 GetCenter() const;

		// Sets new center, extends remain the same
		void SetCenter(const Vector2& center);
		// Sets new size, center remains the same
		void SetHalfExtends(const Vector2& size);
		// Sets new size, center remains the same
		void SetExtends(const Vector2& size);

		// Returns true if both boxes have same corners
		bool operator==(const BoundingBox& other) const
		{
			return TopLeft.squaredDistance(other.TopLeft) < VERTEX_ERROR_SQUARED &&
				BotRight.squaredDistance(other.BotRight)  < VERTEX_ERROR_SQUARED;
		}

		// Returns true if boxes have different corners
		bool operator!=(const BoundingBox& other) const
		{
			return TopLeft.squaredDistance(other.TopLeft) > VERTEX_ERROR_SQUARED ||
				BotRight.squaredDistance(other.BotRight) > VERTEX_ERROR_SQUARED;
		}

		void DebugDraw(PathingDrawer* drawer) const;

		// Return distance between closest points of boxes
		float DistanceTo(const BoundingBox& other) const;
		// Return distance between closest points of boxes
		float DistanceToSquared(const BoundingBox& other) const;
		// Return distance between closest points of shapes
		float DistanceTo(const BoundingCircle& other) const;
		// Return distance between closest points of shapes
		float DistanceToSquared(const BoundingCircle& other) const;
		// Returns circle with same center and radius = max extend
		operator BoundingCircle() const;
	};

	// Simple bounding circle
	struct BoundingCircle
	{
		BoundingCircle() { }
		BoundingCircle(const Vector2& center, float radius) :
			Center(center),
			Radius(radius)
		{ }

		Vector2 Center;
		float Radius;

		// Returns true if given point is inside box
		bool IsPointInside(const Vector2& point) const;
		// Returns true if other box intersects this box
		bool Intersect(const BoundingBox& other) const;
		// Returns true if other circle intersects this box
		bool Intersect(const BoundingCircle& other) const;

		BoundingCircle GetBoundingCircle() const;
		BoundingBox GetBoundingBox() const;

		// Returns size along given axis
		float GetExtend(Axes::Axis axis) const;
		// Returns half the size along given axis
		float GetHalfExtend(Axes::Axis axis) const;
		// Returns size of box
		Vector2 GetExtends() const;
		// Returns area of box ( x * y )
		float GetArea() const;
		// Returns true of other box is fully inside this one
		bool Contains(const BoundingCircle& other) const;
		// Expands corners, so that this box fully contains other one
		// Returns true if box actually expanded
		bool ExpandToFit(const BoundingCircle& other);

		BoundingCircle ExpandToFitCopy(const BoundingCircle& other) const;

		float GetAreaExpanded(const BoundingCircle& other) const;

		Vector2 GetCenter() const;

		// Sets new center, extends remain the same
		void SetCenter(const Vector2& center);
		// Sets new size, center remains the same
		void SetHalfExtends(const Vector2& size);
		// Sets new size, center remains the same
		void SetExtends(const Vector2& size);

		// Returns true if both boxes have same corners
		bool operator==(const BoundingCircle& other) const
		{
			return Center.squaredDistance(other.Center) < VERTEX_ERROR_SQUARED &&
				(Radius-other.Radius)*(Radius-other.Radius)  < VERTEX_ERROR_SQUARED;
		}

		// Returns true if boxes have different corners
		bool operator!=(const BoundingCircle& other) const
		{
			return Center.squaredDistance(other.Center) >= VERTEX_ERROR_SQUARED ||
				(Radius-other.Radius)*(Radius-other.Radius) >= VERTEX_ERROR_SQUARED;
		}

		void DebugDraw(PathingDrawer* drawer) const;

		// Return distance between closest points of boxes
		float DistanceTo(const BoundingCircle& other) const;
		// Return distance between closest points of boxes
		float DistanceToSquared(const BoundingCircle& other) const;
		// Return distance between closest points of boxes
		float DistanceTo(const BoundingBox& other) const;
		// Return distance between closest points of boxes
		float DistanceToSquared(const BoundingBox& other) const;
		// Returns square with same center and half extends = radius
		operator BoundingBox() const
		{
			return BoundingBox(GetCenter() - Vector2(Radius,Radius), GetCenter() + Vector2(Radius,Radius));
		}
	};

	inline bool BoundingBox::IsPointInside(const Vector2& point) const
	{
		return !(point.x < TopLeft.x - VERTEX_ERROR || 
			point.x > BotRight.x + VERTEX_ERROR ||
			point.y < TopLeft.y - VERTEX_ERROR ||
			point.y > BotRight.y + VERTEX_ERROR );
	}

	inline bool BoundingBox::Intersect(const BoundingBox& other) const
	{
		return !(other.TopLeft.x > BotRight.x + VERTEX_ERROR ||
			other.BotRight.x < TopLeft.x - VERTEX_ERROR ||
			other.TopLeft.y > BotRight.y + VERTEX_ERROR ||
			other.BotRight.y < TopLeft.y - VERTEX_ERROR );
	}

	inline bool BoundingBox::Intersect(const BoundingCircle& other) const
	{
		return other.Intersect(*this);
	}

	inline float BoundingBox::GetExtend(Axes::Axis axis) const
	{
		PFAssert( axis != Axes::Z, "" );
		Vector2 v = BotRight - TopLeft;
		return reinterpret_cast<float*>(&v)[axis];
	}

	inline Vector2 BoundingBox::GetExtends() const
	{
		return BotRight - TopLeft;
	}

	inline float BoundingBox::GetHalfExtend(Axes::Axis axis) const
	{
		return GetExtend(axis) * 0.5f;
	}

	inline float BoundingBox::GetArea() const
	{
		Vector2 v = BotRight - TopLeft;
		return v.x * v.y;
	}

	inline bool BoundingBox::Contains(const BoundingBox& other) const
	{
		return !(TopLeft.x > other.TopLeft.x ||
			TopLeft.y > other.TopLeft.y ||
			BotRight.x < other.BotRight.x ||
			BotRight.y < other.BotRight.y);
	}

	inline bool BoundingBox::ExpandToFit(const BoundingBox& other)
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

	inline BoundingBox BoundingBox::ExpandToFitCopy(const BoundingBox& other) const
	{
		return BoundingBox( Vector2(std::min( TopLeft.x, other.TopLeft.x ), 
			std::min(TopLeft.y, other.TopLeft.y)),
			Vector2(std::max(BotRight.x, other.BotRight.x),
			std::max(BotRight.y, other.BotRight.y)));
	}

	inline float BoundingBox::GetAreaExpanded(const BoundingBox& other) const
	{
		return (std::max(BotRight.x, other.BotRight.x) - std::min(TopLeft.x, other.TopLeft.x)) * 
			(std::max(BotRight.y, other.BotRight.y) - std::min(TopLeft.y, other.TopLeft.y));
	}

	inline Vector2 BoundingBox::GetCenter() const
	{
		return (TopLeft + BotRight) * 0.5f;
	}

	inline void BoundingBox::SetCenter(const Vector2& center)
	{
		Vector2 hextends(GetExtends() * 0.5f);
		TopLeft = center - hextends;
		BotRight = center + hextends;
	}

	inline void BoundingBox::SetHalfExtends(const Vector2& size)
	{
		Vector2 center(GetCenter());
		TopLeft = center - size;
		BotRight = center + size;
	}

	inline void BoundingBox::SetExtends(const Vector2& size)
	{
		Vector2 center(GetCenter());
		TopLeft = center - size * 0.5f;
		BotRight = center + size * 0.5f;
	}

	// Dist = Len(Max((0, 0), Abs(Center - otherCenter) - (Extent + otherExtent)))
	inline float BoundingBox::DistanceTo(const BoundingBox& other) const
	{
		float dist = DistanceToSquared(other);
		if ( dist < 0.f )
			return -std::sqrtf(-dist);
		return std::sqrtf(dist);
	}

	inline float BoundingBox::DistanceToSquared(const BoundingBox& other) const
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
	inline float BoundingBox::DistanceTo(const BoundingCircle& other) const
	{
		return other.DistanceTo(*this);
	}

	inline float BoundingBox::DistanceToSquared(const BoundingCircle& other) const
	{
		return other.DistanceToSquared(*this);
	}

	inline void BoundingBox::DebugDraw(PathingDrawer* drawer) const
	{
		drawer->DrawBox(TopLeft, Vector2(TopLeft.x, BotRight.y), BotRight, Vector2(BotRight.x, TopLeft.y));
	}

	inline BoundingBox::operator BoundingCircle() const
	{
		return BoundingCircle(GetCenter(), std::max(GetHalfExtend(Axes::X),GetHalfExtend(Axes::Y)));
	}

	inline bool BoundingCircle::IsPointInside(const Vector2& point) const
	{
		return point.squaredDistance(Center) <= Radius*Radius - VERTEX_ERROR_SQUARED;
	}

	inline bool BoundingCircle::Intersect(const BoundingBox& other) const
	{
		if(GetBoundingBox().Intersect(other))
		{
			float radius2 = Radius*Radius;
			if( Center.squaredDistance(other.TopLeft) <= radius2 - VERTEX_ERROR_SQUARED ||
				Center.squaredDistance(other.BotRight) <= radius2 - VERTEX_ERROR_SQUARED ||
				Center.squaredDistance(Vector2(other.TopLeft.x, other.BotRight.y)) <= radius2 - VERTEX_ERROR_SQUARED ||
				Center.squaredDistance(Vector2(other.TopLeft.y, other.BotRight.x)) <= radius2 - VERTEX_ERROR_SQUARED 
				)
				return true;
		}
		return false;
	}

	inline bool BoundingCircle::Intersect(const BoundingCircle& other) const
	{
		return other.Center.squaredDistance(Center) <=
			(other.Radius+Radius)*(other.Radius+Radius) - VERTEX_ERROR_SQUARED;
	}

	inline BoundingBox BoundingCircle::GetBoundingBox() const
	{
		return BoundingBox(Center - Radius, Center + Radius);
	}

	inline float BoundingCircle::GetExtend(Axes::Axis axis) const
	{
		_ASSERT( axis != Axes::Z, "" );
		return Radius * 2.0f;
	}

	inline Vector2 BoundingCircle::GetExtends() const
	{
		return Vector2(2*Radius, 2*Radius);
	}

	inline float BoundingCircle::GetHalfExtend(Axes::Axis axis) const
	{
		_ASSERT( axis != Axes::Z, "" );
		return Radius;
	}

	inline float BoundingCircle::GetArea() const
	{
		return PI * Radius * Radius;
	}

	inline bool BoundingCircle::Contains(const BoundingCircle& other) const
	{
		return Radius > other.Radius + Center.distance(other.Center);
	}

	inline bool BoundingCircle::ExpandToFit(const BoundingCircle& other)
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

	inline BoundingCircle BoundingCircle::ExpandToFitCopy(const BoundingCircle& other) const
	{
		float ccDist = Center.distance(other.Center);
		return BoundingCircle(0.5f * ((Center + other.Center) + 
			(Radius - other.Radius)*(Center - other.Center) / ccDist),
			0.5f * (ccDist + Radius + other.Radius));
	}

	inline float BoundingCircle::GetAreaExpanded(const BoundingCircle& other) const
	{
		float newRadius = 0.5f * (Center.distance(other.Center) + Radius + other.Radius);
		return PI * Radius * Radius;
	}

	inline Vector2 BoundingCircle::GetCenter() const
	{
		return Center;
	}

	inline void BoundingCircle::SetCenter(const Vector2& center)
	{
		Center = center;
	}

	inline void BoundingCircle::SetHalfExtends(const Vector2& size)
	{
		Radius = size.x;
	}

	inline void BoundingCircle::SetExtends(const Vector2& size)
	{
		Radius = size.x * 0.5f;
	}

	inline float BoundingCircle::DistanceTo(const BoundingCircle& other) const
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

	inline float BoundingCircle::DistanceToSquared(const BoundingCircle& other) const
	{
		float d = DistanceTo(other) ;
		return d*d;
	}

	inline float BoundingCircle::DistanceToSquared(const BoundingBox& other) const
	{
		return Center.squaredDistance(other.TopLeft) - Radius - other.TopLeft.x; //TODO is this ok??
	}

	inline float BoundingCircle::DistanceTo(const BoundingBox& other) const
	{
		float dist = DistanceToSquared(other);
		if ( dist < 0.f )
			return -std::sqrtf(-dist);
		return std::sqrtf(dist);
	}

	inline void BoundingCircle::DebugDraw(PathingDrawer* drawer) const
	{
		// 16 vertices per circle
		int vCount = 16;
		float dAng = 2.f*PI/(float)vCount;
		for(int i = 0; i < vCount; ++i)
		{
			Vector2 p1 = Vector2(GetCenter() + Radius * Vector2(
				std::sinf(i*dAng) , std::cosf(i*dAng)));
			Vector2 p2 = Vector2(GetCenter() + Radius * Vector2(
				std::sinf((i+1)*dAng) , std::cosf((i+1)*dAng)));
			drawer->DrawLine(p1,p2);
		}
	}
}