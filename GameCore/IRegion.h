#pragma once

#include "DataTypes.h"
#include <OgreVector2.h>
#include <OgreVector3.h>
#include <OgreSphere.h>
#include <OgrePolygon.h>

namespace FlagRTS
{
	// Region is generic finite part of map space (2d)
	// 
	class IRegion
	{
	public:
		// Returns true if point lies within region 
		// ( used for area regions )
		virtual bool CheckPointIsInRegion(const Vector2& point2d) { return true; }

		// Returns true if point lies within region 
		//( used for volume regions, can be used for area )
		virtual bool CheckPointIsInRegion(const Vector3& point3d) { return true; }

		// INTERSECTIONS

		// Creators below returns subclasses of IRegion

		// Returns CircleRegion with given radius and center
		static IRegion* FromCircle(const Vector2& center, float radius);
		// Returns RectRegion with given rectangle corners
		static IRegion* FromRectange(const Vector2& topLeft, const Vector2& botRight);
		// Returns RectRegion with given rectangle center and size
		static IRegion* FromRectangeCenter(const Vector2& rectCenter, const Vector2& rectSize);
		// Returns PolyRegion created based on given polygon
		static IRegion* FromPolygon(const Polygon& polygon);
		// Retruns volume region SphereRegion
		static IRegion* FromSphere(const Vector3& center, float radius);
		// Retruns volume region CubeRegion ( actualy not a perfect cube ) mad from center and edges
		static IRegion* FromCubeCenter(const Vector3& center, const Vector3& edges);
		// Retruns volume region CubeRegion ( actualy not a perfect cube ) made from 2 opposite corners
		static IRegion* FromCube(const Vector3& topFrontLeft, const Vector3& botBackRight);
	};

	// Region with rectange size
	class RectRegion : public IRegion
	{
	private:
		 Vector2 _topLeft;
		 Vector2 _botRight;

	public:
		RectRegion(const Vector2& tl, const Vector2& br);
	};

	// Region with rectange size
	class CircleRegion : public IRegion
	{
	private:
		 Vector2 _radius;
		 Vector2 _center;

	public:
		CircleRegion(const Vector2& center, const Vector2& rad);
	};

	// Region with rectange size
	class PolygonRegion :  public IRegion
	{
	private:
		 Polygon _polygon;

	public:
		PolygonRegion(const Polygon& poly);
	};

	// Region with rectange size
	class SphereRegion :  public IRegion
	{
	private:
		Vector2 _center;
		Vector2 _radius;

	public:
		SphereRegion(const Vector2& center, const Vector2& rad);
	};

	// Region with rectange size
	class CubeRegion :  public IRegion
	{
	private:
		Vector3 _topFrontLeft;
		Vector3 _botBackRight;

	public:
		CubeRegion(const Vector3& tfl, const Vector3& bbr);
	};
}