#pragma once

#include "PathFindingIPathFinder.h"

namespace PathFinding
{
	class UniformGridPathingMap;
	template<typename VectorT>
	class IUnitUnitPairer;

	class ForcesPathFinder : public ILocalPathFinder<PFVector2>
	{
	public:
		typedef IPathUnit<PFVector2> UnitT;

	private:
		UniformGridPathingMap* _obstaclesMap;
		PFArray<UnitT*> _units;
		IUnitUnitPairer<PFVector2>* _uuPairer;

		// Parameters
		float _repulsionForceCoeff; // Relative unit-unit repulsion force
		float _attractionForceCoeff; // Relative unit-goal attraction force
		float _repulsionFallOff; // Distance on which repulsion force = 0, or number of collRads of source unit
		float _repulsionFallOffInv;
		bool _scaleFallOff; // If true scale repulsion falloff by collison radius


	public:
		ForcesPathFinder(UniformGridPathingMap* obstaclesMap, IUnitUnitPairer<PFVector2>* uuPairer);

		void AddUnit(UnitT* unit);
		void RemoveUnit(UnitT* unit);

		void MoveUnit(UnitT* unit);

		PFArray<UnitT*>* GetUnits() { return &_units; }
		
		void FindUnitMoves(float lastFrameMs);

		// Computes resultant force for each unit and final move distance
		void ComputeUnitsForces();

		void SetRepulsionForceCoeff(float coeff) { _repulsionForceCoeff = coeff; }
		void SetGoalAttractionForceCoeff(float coeff) { _attractionForceCoeff = coeff; }
		void SetRepulsionForceFallOff(float distance) { _repulsionFallOff = distance; _repulsionFallOffInv = 1.f/distance; }
		void SetIsRepulsionFallOffScaling(bool value) { _scaleFallOff = value; }

	private:
		float ComputeRepusliveForce(UnitT* source, float distance);
	};
}