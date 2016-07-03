#pragma once

#include <PathFindingUniformGridPathingMap.h>

namespace FlagRTS
{
	typedef PathFinding::UniformGridPathingMap PathingMap;
	typedef PathFinding::UniformGridObstacle PathingObstacle;
	typedef PathFinding::PathingGroup PathingGroup;

	class PathingController
	{
		PathingMap* _pathingMap;

	public:
		PathingController();

		void Update(float ms);

		PathingMap* GetPathingMap();
	};
}