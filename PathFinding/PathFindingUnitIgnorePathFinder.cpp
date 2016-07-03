#include "PathFindingUnitIgnorePathFinder.h"
#include "PathFindingUniformGridPathingMap.h"
#include "PathFindingIPathUnit.h"

namespace PathFinding
{
	UnitIgnorePathFinder::UnitIgnorePathFinder(UniformGridPathingMap* obstaclesMap) :
		_obstaclesMap(obstaclesMap)
	{

	}

	void UnitIgnorePathFinder::AddUnit(UnitT* unit)
	{
		_units.push_back(unit);
	}

	void UnitIgnorePathFinder::RemoveUnit(UnitT* unit)
	{
		Array::RemoveElement(_units, unit);
	}

	void UnitIgnorePathFinder::MoveUnit(UnitT* unit)
	{

	}

	void UnitIgnorePathFinder::FindUnitMoves(float lastFrameMs)
	{
		for(unsigned int i = 0; i < _units.size(); ++i)
		{
			UnitT* unit = _units[i];
			if(unit->GetIsPathRequested())
			{
				float distToGoal = unit->GetGoal().distance(unit->GetPosition());
				PFVector2 goalDir = (unit->GetGoal() - unit->GetPosition()).normalisedCopy();
				unit->SetFrameMove( goalDir * std::min(distToGoal, unit->GetMaxSpeed() * lastFrameMs) );
			}
			else
			{
				unit->SetFrameMove(PFVector2(0.f,0.f));
			}
		}
	}
}