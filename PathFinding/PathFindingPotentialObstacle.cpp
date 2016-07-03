#include "PathFindingPotentialObstacle.h"
#include "PathFindingPotentialForce.h"
#include "PathFindingUniformGridObstacle.h"

namespace PathFinding
{
	PotentialObstacle::PotentialObstacle(
		UniformGridObstacle* footprint,
		const int forceRadius, const int forceMaskShape, 
		PotentialForce* repulsionForce) :
			_footprint(footprint),
			_position(-1,-1)
	{
		SetRepulsiveField(forceRadius, forceMaskShape, repulsionForce);
	}

	void PotentialObstacle::SetRepulsiveField(
		const int forceRadius, const int forceMaskShape, 
		PotentialForce* repulsionForce)
	{
		bool oddSizeX = (_footprint->GetColumnCount() & 1) > 0;
		bool oddSizeY = (_footprint->GetRowCount() & 1) > 0;
		int halfSizeX = (_footprint->GetColumnCount() >> 1);
		int halfSizeY = (_footprint->GetRowCount() >> 1);

		if(forceMaskShape == PotentialForce::PotentialMaskShape::Square)
		{
			int xmin, xmax, ymin, ymax;
			if(oddSizeX)
			{
				xmin = -(halfSizeX + forceRadius);
				xmax =  (halfSizeX + forceRadius);
			}
			else
			{
				xmin = -(halfSizeX + forceRadius - 1);
				xmax =  (halfSizeX + forceRadius);
			}
			if(oddSizeX)
			{
				ymin = -(halfSizeY + forceRadius);
				ymax =  (halfSizeY + forceRadius);
			}
			else
			{
				ymin = -(halfSizeY + forceRadius - 1);
				ymax =  (halfSizeY + forceRadius);
			}
			for(int dx = xmin; dx <= xmax; ++dx)
			{
				for(int dy = ymin; dy <= ymax; ++dy)
				{
					CellForce cf;
					cf.Shift = IntVector2(dx,dy);
					IntVector2 distance(0,0);

					if(!oddSizeX && (-dx+1 > halfSizeX) )
						distance.X = -dx - halfSizeX + 1;
					else if(std::abs(dx) > halfSizeX)
						distance.X = std::abs(dx) - halfSizeX;

					if(!oddSizeY && (-dy+1 > halfSizeY) )
						distance.Y = -dy - halfSizeY + 1;
					else if(std::abs(dy) > halfSizeY)
						distance.Y = std::abs(dy) - halfSizeY;
					
					repulsionForce->ComputeForce(distance.Length());
					_potentialMask.push_back(cf);
				}
			}
		}
	}
}