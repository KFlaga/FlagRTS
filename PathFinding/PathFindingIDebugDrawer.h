#pragma once

#include "PathFindingPreReqs.h"

namespace PathFinding
{
	// Allows for drawing visual representation of pathfinder elements.
	// Ability to draw lines in given color must be supplied in derived class
	// Lines are given as 2d points on terrain, height must be calculated in dervied class
	class IDebugDrawer
	{
	public:
		IDebugDrawer() { }
		virtual ~IDebugDrawer() { }

		virtual void DrawLine(const PFVector2& startPoint, const PFVector2& endPoint) = 0;
		virtual void DrawLine(const PFVector3& startPoint, const PFVector3& endPoint) { }
		virtual void DrawMessage(const PFVector2& position, const PFVector2& size, const char* text) { }
		// Points in counter-clockwise order
		void DrawTriangle(const PFVector2& v1, const PFVector2& v2, const PFVector2& v3)
		{
			DrawLine(v1,v2);
			DrawLine(v2,v3);
			DrawLine(v3,v1);
		}
		// Points in counter-clockwise order
		void DrawBox(const PFVector2& v1, const PFVector2& v2, const PFVector2& v3, const PFVector2& v4)
		{
			DrawLine(v1,v2);
			DrawLine(v2,v3);
			DrawLine(v3,v4);
			DrawLine(v4,v1);
		}
		// Points in order : cc-bottom starting with bot-left-back, then cc-top start top-left-back
		void DrawBox3d(const PFVector3& v1, const PFVector3& v2, const PFVector3& v3, const PFVector3& v4,
			const PFVector3& v5, const PFVector3& v6, const PFVector3& v7, const PFVector3& v8)
		{
			DrawLine(v1,v2);
			DrawLine(v2,v3);
			DrawLine(v3,v4);
			DrawLine(v4,v1);
			DrawLine(v5,v6);
			DrawLine(v6,v7);
			DrawLine(v7,v8);
			DrawLine(v8,v5);
			DrawLine(v1,v5);
			DrawLine(v2,v6);
			DrawLine(v3,v7);
			DrawLine(v4,v8);
		}
	};
}