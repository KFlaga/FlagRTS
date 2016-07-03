#pragma once

#include "PathFindingUtils.h"

namespace PathFinding
{
	class UniformGridObstacle;
	class PotentialForce;

	// Potential obstacles are immobile object that emits repulsive potential
	class PotentialObstacle
	{
	public:
		struct CellForce
		{
			IntVector2 Shift;
			float Force;
		};

	protected:
		UniformGridObstacle* _footprint; // Shape of obstacle
		PFArray<CellForce> _potentialMask; // Cell-shifts ( counted from unit center ) with potential values
		IntVector2 _position; // Cell obstacle's center is on top-left corner of footprint

	public:
		PotentialObstacle(
			UniformGridObstacle* footprint, 
			const int forceRadius, const int forceMaskShape,
			PotentialForce* repulsionForce);

		UniformGridObstacle* GetFootprint() const { return _footprint; }
		
		void SetPosition(const IntVector2& position) { _position = position; }
		const IntVector2& GetPosition() const { return _position; }

		void SetRepulsiveField(const int forceRadius, const int forceMaskShape,
			PotentialForce* repulsionForce);

		const PFArray<CellForce>& GetRepulsiveField() const { return _potentialMask; }
	};
}