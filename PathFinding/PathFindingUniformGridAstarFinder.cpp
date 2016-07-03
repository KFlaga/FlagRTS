#include "PathFindingUniformGridAstarFinder.h"
#include "PathFindingUniformGridPathingMap.h"
#include "PathFindingAstarNode.h"
#include "PathFindingAstarNodeQueue.h"
#include <map>
#include <set>

namespace PathFinding
{
	void UniformGridAstarFinder::FindPath(
		const short beginCellX, 
		const short beginCellY, 
		const short endCellX, 
		const short endCellY,
		const short obstacleGroup)
	{
		_lastPathDistance = 0.f;
		AstarNodeQueue openSet;
		std::unordered_map<int, AstarNode> allNodes;

		int beginCode = AstarNode::ComputeCellCode(beginCellX, beginCellY);
		int endCode = AstarNode::ComputeCellCode(endCellX, endCellY);
		int dx, dy;
		int nCode;
		AstarNode* nNode;
		std::unordered_map<int, AstarNode>::iterator nNodeIt;
		float neighbourDistance;

		// Insert first node to queue
		allNodes.insert(std::make_pair(beginCode, 
			AstarNode(beginCode, 0.f, (AstarNode*)0, endCellX, endCellY, _heuristicMultiplier)));
		AstarNode* startNode = &allNodes[beginCode];
		openSet.Insert(startNode);

		_lastExpandedNodes = 0;
		_lastVisitedNodes = 0;

		while(openSet.IsNotEmpty())
		{
			++_lastExpandedNodes;
			// While there's open node and end is not reached
			// Open most promising node
			AstarNode* currentNode = openSet.Top();
			if(currentNode->GetCellCode() == endCode)
			{
				// Next node is our goal so end
				_lastPathDistance = currentNode->GetDistanceFromStart();
				 RecosntructPath(startNode, currentNode);
				 return;
			}

			// Close current node
			openSet.Pop();
			currentNode->Close();

			// Bounds control
			int mindy = std::max(-1, -currentNode->GetCellY());
			int mindx = std::max(-1, -currentNode->GetCellX());
			for(dy = mindy; dy <= 1; ++dy)
			{
				for(dx = mindx; dx <= 1; ++dx)
				{
					++_lastVisitedNodes;
					// Update each valid neighbour
					if((_pathingMap->GetCell( currentNode->GetCellY() + dy, currentNode->GetCellX() + dx ).
						Blocking & obstacleGroup) > 0)
					{
						// We have obstacle - ignore
						// Add check for diagonal move to have a free cell
						continue;
					}
					// Get code of neighbour
					nCode = AstarNode::ComputeCellCode(
						currentNode->GetCellX()+dx,currentNode->GetCellY()+dy);
					// Add 1 per straight move or 1.4 per diagonal to current distance
					neighbourDistance = currentNode->GetDistanceFromStart() + (dx*dy == 0 ? 1.f : 1.41f);
					// Find if node is created -> if not then add it to open set
					nNodeIt = allNodes.find(nCode);
					if(nNodeIt != allNodes.end())
					{
						// Node is added, find its address
						nNode = &nNodeIt->second;
						if(nNode->IsClosed() == true)
							continue; // Node is already closed, so ignore it

						if(neighbourDistance >= nNode->GetDistanceFromStart()) 
							continue;// Better path already found
						
						// Path to neighboor node including currentNode is better, so update it
						nNode->SetAncestor(currentNode);
						nNode->UpdateTotalCost(neighbourDistance);
						openSet.UpdateCost(nNode);
					}
					else
					{
						// Create node and get its address
						nNode = &(allNodes.insert(std::make_pair(nCode, 
							AstarNode(nCode, neighbourDistance, currentNode,
							 endCellX, endCellY, _heuristicMultiplier))
							)).first->second;
						openSet.Insert(nNode);
					}
				}
			}
		}
	}
	
	void UniformGridAstarFinder::RecosntructPath(AstarNode* startNode, AstarNode* goalNode)
	{
		_lastPath.clear();
		// Reserve minimal path lenght at start - assuming all moves are diagonal
		_lastPath.reserve((int)(_lastPathDistance * 0.7f));
		AstarNode* currentNode = goalNode;
		while(currentNode->GetAncestor() != 0)
		{
			_lastPath.push_back(std::make_pair(
				currentNode->GetCellX(), currentNode->GetCellY()));
			currentNode = currentNode->GetAncestor();
		}
		_lastPath.push_back(std::make_pair(
				startNode->GetCellX(), startNode->GetCellY()));
	}
}