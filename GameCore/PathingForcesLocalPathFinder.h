#pragma once

#include "PathingIPathFinder.h"

namespace FlagRTS
{
	class UniformGridPathingMap;

	template<typename UnitT>
	class IUnitUnitPairer;

	template<typename UnitT>
	class ForcesPathFinder : public ILocalPathFinder<UnitT>
	{
	private:
		UniformGridPathingMap* _obstaclesMap;
		Array<UnitT*> _units;
		IUnitUnitPairer<UnitT>* _uuPairer;

		// Parameters
		float _repulsionForceCoeff; // Relative unit-unit repulsion force
		float _attractionForceCoeff; // Relative unit-goal attraction force
		float _repulsionFallOff; // Distance on which repulsion force = 0, or number of collRads of source unit
		float _repulsionFallOffInv;
		bool _scaleFallOff; // If true scale repulsion falloff by collison radius
		float forceStrength = 10.f;
		float forceDistance = 200.f;
		float goalForceMult = 10.f;
		float maxPeneteration = 0.1f;

		struct Force
		{
			Vector2 Vector;
		};

		Vector2 GetGoalForce(Vector2 goal)
		{
			return goal.normalisedCopy() * goalForceMult;
		}

	public:
		ForcesPathFinder(UniformGridPathingMap* obstaclesMap, IUnitUnitPairer<UnitT>* uuPairer) :
			_obstaclesMap(obstaclesMap),
			_uuPairer(uuPairer)
		{

		}

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

		PFArray<UnitT*>* GetUnits() { return &_units; }

		void FindUnitMoves(float lastFrameMs);

		// Computes resultant force for each unit and final move distance
		void ComputeUnitsForces()
		{

		}

		void SetRepulsionForceCoeff(float coeff) { _repulsionForceCoeff = coeff; }
		void SetGoalAttractionForceCoeff(float coeff) { _attractionForceCoeff = coeff; }
		void SetRepulsionForceFallOff(float distance) { _repulsionFallOff = distance; _repulsionFallOffInv = 1.f/distance; }
		void SetIsRepulsionFallOffScaling(bool value) { _scaleFallOff = value; }

	private:
		float ComputeRepusliveForce(UnitT* source, float distance)
		{
			//// Assume distance > 0
			//return 	direction * _repulsionForceCoeff * ( _scaleFallOff == true ?
			//	(_repulsionFallOff - distance) * _repulsionFallOffInv :
			//(_repulsionFallOff * source->GetCollisionRadius() - distance) / (_repulsionFallOff * source->GetCollisionRadius()) );
			float dist = std::max(forceDistance-distance, 0.f);
			return forceStrength * dist / forceDistance;
		}
	};

	template<typename UnitT>
	void ForcesPathFinder<UnitT>::FindUnitMoves(float lastFrameMs)
	{
		// First find uu pairs and sum uu + goal forces
		_uuPairer->FindUnitPairs();
		auto& uuMap = _uuPairer->GetUnitPairsMap();

		// For each unit find if it is on collision path or close enough to 
		// emit some repuslive force
		for(auto unitIt = uuMap.begin(); unitIt != uuMap.end(); ++unitIt)
		{
			auto subjectUnit = unitIt->first;
			Vector2 goalDir = (subjectUnit->GetGoal() - subjectUnit->GetPosition()).normalisedCopy();
			Vector2 totalRepForce(0.f,0.f);
			bool resolveCollision = false;

			// For each unit compute small repulsive forces
			for(unsigned int i = 0; i < unitIt->second.size(); ++i)
			{
				auto sourceUnit = unitIt->second[i];
				float distance = subjectUnit->GetDitanceTo(sourceUnit);
				if( distance < 0.f )
				{
					//	// We got collision -> move unit to position where distance == 0
					//	subjectUnit->SetFrameMove( distance *
					//		(sourceUnit->GetPosition() - subjectUnit->GetPosition()).normalisedCopy());
					//	resolveCollision = true;
					//	break;

				}
				//else
				//{
				// Compute forces between units
				float repForceMag = ComputeRepusliveForce(sourceUnit, distance);
				Vector2 repForce;
				// Make force perpendicular to goal
				//Vector2 perpGoal(-goalDir.y, goalDir.x);
				repForce = (sourceUnit->GetPosition() - subjectUnit->GetPosition()).normalisedCopy() * repForceMag;

				totalRepForce += repForce;
				//}
			}
			if(resolveCollision)
				continue;

			// Have sum of forces for this unit -> now compute over-all direction
			float totalForceStrength = totalRepForce.length();
			if( totalForceStrength < goalForceMult * 0.1f )
			{
				totalRepForce = Vector2(0.f,0.f);
			}
			else
			{
				totalRepForce = totalRepForce * 1.f;
			}

			//	// Check if unit have a goal set
			if(subjectUnit->GetIsPathRequested() == true)
			{
				// Add force towards goal
				Vector2 goalForce = GetGoalForce(goalDir);
				// Sub it from rep force
				totalRepForce -= goalForce;
			}

			if( (subjectUnit->GetGoal() - subjectUnit->GetPosition()).length() < subjectUnit->GetMaxSpeed() * lastFrameMs)
			{
				subjectUnit->SetFrameMove( subjectUnit->GetGoal() - subjectUnit->GetPosition() );
			}
			else
			{
				totalRepForce.normalise();
				// Move unit in direction -repForce
				subjectUnit->SetFrameMove( -totalRepForce * subjectUnit->GetMaxSpeed() * lastFrameMs );
			}
		}
	}
	
	template<typename UnitT>
	//void ForcesPathFinder::ComputeUnitsForces()
	//{
	//	//// First find uu pairs and sum uu + goal forces
	//	_uuPairer->FindUnitPairs();
	//	auto& uuMap = _uuPairer->GetUnitPairsMap();

	//	// For each unit find if it is on collision path or close enough to 
	//	// emit some repuslive force
	//	for(auto unitIt = uuMap.begin(); unitIt != uuMap.end(); ++unitIt)
	//	{
	//		auto subjectUnit = unitIt->first;
	//		Vector2 sumForce;
	//	//	PFArray<Force> forces;
	//	//	// Check if unit have a goal set
	//		if(subjectUnit->GetIsPathRequested() == true)
	//		{
	//			// Add force towards goal

	//	//		// For each force
	//	//		for(subjectUnit = unitIt->first; unitIt != uuMap.end() && subjectUnit == unitIt->first; ++unitIt)
	//	//		{
	//	//			ForcesUnit* sourceUnit = unitIt->second.SourceUnit;
	//	//			float distance = unitIt->second.Distance; // Distance between collision circles of units
	//	//			Force repulsion;
	//	//			if(sourceUnit->GetGoal().x > 0)
	//	//			{
	//	//				// Both units have goals set, so those rules apply:
	//	//				// - repulsion to subject is perpendicular to source goal, one farther from source
	//	//				// - if both direction are equaly ok ( so source's goal is toward subject )
	//	//				//  get one closer to own goal
	//	//				// - if choice is still ambigious ( so units move toward each other ), then
	//	//				//  choose one with greater angle from source goal 
	//	//				// ( it will ensure that opposite forces are choosen for boyh units )
	//	//				// First check for special cases 
	//	//				Vector2 vecTowardSource(sourceUnit->GetPosition() - subjectUnit->GetPosition());
	//	//				vecTowardSource.normalise();
	//	//				Vector2 vecTowardSubject(subjectUnit->GetPosition() - sourceUnit->GetPosition());
	//	//				vecTowardSubject.normalise();
	//	//				Vector2 sourceGoalDir = sourceUnit->GetGoal().normalisedCopy();
	//	//				Vector2 forceVecRight(sourceGoalDir.y, -sourceGoalDir.x);
	//	//				Vector2 forceVecLeft(-sourceGoalDir.y, sourceGoalDir.x);
	//	//				if( (vecTowardSubject - sourceGoalDir).squaredLength() < RELATIVE_ERROR )
	//	//				{
	//	//					// Source moves toward subject
	//	//					Vector2 subjectGoalDir = subjectUnit->GetGoal().normalisedCopy();
	//	//					if( (vecTowardSource - subjectGoalDir).squaredLength() < RELATIVE_ERROR )// Unit move toward each other
	//	//						// Add 'left' force
	//	//							repulsion.Vector = ComputeRepusliveForce(sourceUnit, forceVecLeft, distance);
	//	//					else // Choose force closer to subject's goal
	//	//					{
	//	//						// As vectors are opposite, one must have positive dotProduct ( angle < 90deg )
	//	//						// and other negative ( 270deg > angle > 90deg )
	//	//						// Of course negative is farther , so choose this one
	//	//						if(forceVecLeft.dotProduct(subjectGoalDir) < 0) // Right is ok	
	//	//							repulsion.Vector = ComputeRepusliveForce(sourceUnit, forceVecRight, distance);
	//	//						else // Left is ok
	//	//							repulsion.Vector = ComputeRepusliveForce(sourceUnit, forceVecLeft, distance);
	//	//					}
	//	//				}
	//	//				else
	//	//				{
	//	//					// As vectors are opposite, one must have positive dotProduct ( angle < 90deg )
	//	//					// and other negative ( 270deg > angle > 90deg )
	//	//					// Of course negative is farther , so choose this one
	//	//					if(forceVecLeft.dotProduct(vecTowardSource) > 0) // Right is ok	
	//	//						repulsion.Vector = ComputeRepusliveForce(sourceUnit, forceVecRight, distance);
	//	//					else // Left is ok
	//	//						repulsion.Vector = ComputeRepusliveForce(sourceUnit, forceVecLeft, distance);
	//	//				}

	//	//				if(repulsion.Vector.squaredLength() > RELATIVE_ERROR)
	//	//					forces.push_back(repulsion);
	//	//			}
	//	//			else if(subjectUnit->IsAllowDisplacement()) // Check if unit can be pushed if collides with others' paths
	//	//			{

	//	//			}
	//	//			// Else unit is immovable, so omit it
	//	//		}

	//	//		// Execute move and find if unit do not collide with obstacle
	//	//		// If yes, apply force, which would place unit outside obstacle bounds,
	//	//		// move back to beggining, recompute forces and move in new direction
	//	//		_uoPairer->FindCollidingPairs();
	//		}
	//	}
	//}
}