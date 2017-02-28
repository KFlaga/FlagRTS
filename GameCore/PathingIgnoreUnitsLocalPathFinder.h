#pragma once

#include "PathingIPathFinder.h"
#include "PathingUniformGridMap.h"

namespace FlagRTS
{
	template<typename UnitT>
	class UnitIgnorePathFinder : public ILocalPathFinder<UnitT>
	{
	private:
		UniformGridPathingMap* _obstaclesMap;
		Array<UnitT*> _units;

	public:
		UnitIgnorePathFinder(UniformGridPathingMap* obstaclesMap) :	
			_obstaclesMap(obstaclesMap)
		{ }

		void AddUnit(UnitT* unit)
		{
			_units.push_back(unit);
		}

		void RemoveUnit(UnitT* unit)
		{
			_units.remove(unit);
		}

		void MoveUnit(UnitT* unit)
		{

		}

		void FindUnitMoves(float lastFrameMs)
		{
			for(unsigned int i = 0; i < _units.size(); ++i)
			{
				UnitT* unit = _units[i];
				if(unit->GetIsPathRequested())
				{
					float distToGoal = unit->GetLocalGoal().distance(unit->GetPosition());
					Vector2 goalDir = (unit->GetLocalGoal() - unit->GetPosition()).normalisedCopy();
					unit->SetFrameMove( goalDir * std::min(distToGoal, unit->GetMaxSpeed() * lastFrameMs) );
				}
				else
				{
					unit->SetFrameMove(Vector2(0.f,0.f));
				}
			}
		}
	};
}