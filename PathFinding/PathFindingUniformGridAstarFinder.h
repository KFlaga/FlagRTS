#pragma once

#include "PathFindingPreReqs.h"

namespace PathFinding
{
	class UniformGridPathingMap;
	struct CollisionFilter;
	class AstarNode;

	// Simplest pathfinder, which finds shortest path between cells
	// Assumes that nodes are in cell centers and 8 way movement is allowed 
	// ( diagonal movement provided that there's free space between obstacles ) 
	// Accepts as input current PathingMap, specific obstacle filter and
	// start/end position
	// Outputs array of successing nodes
	class UniformGridAstarFinder
	{
		// Should provide debug drawing of last request and
		// performance statictics ( like expanded nodes, path lenght, time spent )
	
		UniformGridPathingMap* _pathingMap;
		PFArray< std::pair<int,int> > _lastPath;
		float _lastPathDistance;
		float _heuristicMultiplier;
		int _lastExpandedNodes;
		int _lastVisitedNodes;
		int _maxCellX;
		int _maxCellY;

	public:

		void SetPathingMap(UniformGridPathingMap* pathMap) { _pathingMap = pathMap; }

		void FindPath(const short beginCellX, 
			const short beginCellY, 
			const short endCellX, 
			const short endCellY,
			const short obstacleGroup);

		void SetHeuristicMultiplier(const float eps) { _heuristicMultiplier = eps; }

		float GetLastPathDistance() const { return _lastPathDistance; }

		PFArray< std::pair<int,int> >& GetLastPath() { return _lastPath; }

	private:
		void RecosntructPath(AstarNode* startNode, AstarNode* goalNode);
	};
}