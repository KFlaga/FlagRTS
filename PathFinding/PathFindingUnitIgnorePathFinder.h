#pragma once

#include "PathFindingIPathFinder.h"

namespace PathFinding
{
	class UniformGridPathingMap;

	class UnitIgnorePathFinder : public ILocalPathFinder<PFVector2>
	{
	public:
		typedef IPathUnit<PFVector2> UnitT;

	private:
		UniformGridPathingMap* _obstaclesMap;
		PFArray<UnitT*> _units;

	public:
		UnitIgnorePathFinder(UniformGridPathingMap* obstaclesMap);

		void AddUnit(UnitT* unit);
		void RemoveUnit(UnitT* unit);

		void MoveUnit(UnitT* unit);
		
		void FindUnitMoves(float lastFrameMs);
	};
}