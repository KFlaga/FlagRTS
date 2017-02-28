#pragma once

#include "DataTypes.h"
#include "IPathingQuery.h"

namespace FlagRTS
{
	class PhysicalObject;

	/**

	*/
	class IPathingSystem
	{
	public:
		IPathingSystem() { }
		virtual ~IPathingSystem() { }

		/**

		*/
		virtual void Update(float ms) = 0;

		/**

		*/
		virtual void AddPathingObject(PhysicalObject* object) = 0;

		/**

		*/
		virtual void RemovePathingObject(PhysicalObject* object) = 0;

		/**

		*/
		virtual IPathingQuery* CreatePathingQuery() = 0;

		/**
		Finds global path. When path is found sets frame move for object every frame.
		Object must have PathingComponent, be added to PathingSystem and
		have GlobalGoal set to be able to request path.
		*/
		virtual void RequestPath(PhysicalObject* object) = 0;

		/**

		*/
		virtual void AbandonPath(PhysicalObject* object) = 0;

	public:
		enum DebugDrawers : int
		{
			GridDrawer = 0,
			DbvhDrawer = 1,
			GroundObstaclesDrawer = 2,
			BuildingObstaclesDrawer = 3,
			DebugDrawersCount,
		};

		/**

		*/
		virtual void ToggleDebugDrawer(DebugDrawers drawer) = 0;
	};
}