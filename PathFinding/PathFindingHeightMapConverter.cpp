#include "PathFindingHeightMapConverter.h"
#include "PathFindingUniformGridPathingMap.h"

namespace PathFinding
{
	HeightMapConverter::HeightMapConverter()
	{

	}
	void HeightMapConverter::SetMap(UniformGridPathingMap* pathingMap, 
		PFArray2d<float>* heightMap)
	{
		_targetMap = pathingMap;
		_heightMap = heightMap;
	}

	void HeightMapConverter::AddSlopeRule(const SlopeRule& rule)
	{
		_rules.push_back(rule);
	}

	void HeightMapConverter::ClearRules()
	{
		_rules.clear();
	}

	void HeightMapConverter::ComputeObstacles()
	{
		float cellSizeX = _targetMap->GetCellSize().x;
		float cellSizeY = _targetMap->GetCellSize().y;
		float cellSizeD = std::sqrtf(cellSizeX*cellSizeX + cellSizeY*cellSizeY);
		for(int y = 0; y < _targetMap->GetCellCountY(); ++y)
		{
			for(int x = 0; x < _targetMap->GetCellCountX(); ++x)
			{
				// get 4 corners
				float height_1 = (*_heightMap)(y,x);
				float height_2 = (*_heightMap)(y+1,x);
				float height_3 = (*_heightMap)(y+1,x+1);
				float height_4 = (*_heightMap)(y,x+1);
				float maxAngle = 0.f;
				// get max angle : assume distance is equal 1 or 1.4 CellSize
				float angle = std::atan2f(std::abs(height_1-height_2), cellSizeX);
				if( angle > maxAngle )
					maxAngle = angle;

				angle = std::atan2f(std::abs(height_2-height_3), cellSizeY);
				if( angle > maxAngle )
					maxAngle = angle;

				angle = std::atan2f(std::abs(height_3-height_4), cellSizeX);
				if( angle > maxAngle )
					maxAngle = angle;

				angle = std::atan2f(std::abs(height_4-height_1), cellSizeY);
				if( angle > maxAngle )
					maxAngle = angle;

				angle = std::atan2f(std::abs(height_1-height_3), cellSizeD);
				if( angle > maxAngle )
					maxAngle = angle;

				angle = std::atan2f(std::abs(height_2-height_4), cellSizeD);
				if( angle > maxAngle )
					maxAngle = angle;

				maxAngle = maxAngle * 57.3f; // Convert rad to deg
				// Find rules where maxangle lies between specified values
				for(unsigned int i = 0; i < _rules.size(); ++i)
				{
					if( _rules[i].MaxAngle >= maxAngle &&
						_rules[i].MinAngle <= maxAngle )
					{
						auto filter = _rules[i].Filter;
						_targetMap->GetCell(y, x) += filter;
					}
				}
			}
		}
	}
}