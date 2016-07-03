#include "PathFindingPolygon.h"

namespace PathFinding
{

	Polygon::Polygon()
	{
		
	}

	Polygon::Polygon(const unsigned int finalVertexCount)
	{
		_vertices.reserve(finalVertexCount);
	}

	Polygon::Polygon(const PFArray<PFVector2>& vertices)
	{
		_vertices = vertices;
	}

	void Polygon::InsertVertex(const unsigned int idx, const PFVector2& vertex)
	{
		_vertices.push_back(_vertices[_vertices.size() - 1]);
		for(unsigned int i = _vertices.size() - 1;
			i > idx; --i)
		{
			_vertices[i] = _vertices[i+1];
		}
		_vertices[idx] = vertex;
	}

	void Polygon::RemoveVertex(const unsigned int idx)
	{
		for(unsigned int i = idx; 
			i < _vertices.size(); ++i)
		{
			_vertices[i] = _vertices[i+1];
		}
		_vertices.pop_back();
	}

	void Polygon::Update()
	{
		if(_geoChanged)
		{
			// Find max/min x/y
			float maxX = _vertices[0].x, maxY = _vertices[0].y;
			float minX = _vertices[0].x, minY = _vertices[0].y;
			for(unsigned int i = 1;	i <  _vertices.size(); ++i)
			{
				maxX = std::max(maxX, _vertices[i].x);
				maxY = std::max(maxY, _vertices[i].y);
				minX = std::min(minX, _vertices[i].x);
				minY = std::min(minY, _vertices[i].y);
			}
			_boundingBox.TopLeft = PFVector2(minX, minY);
			_boundingBox.BotRight = PFVector2(maxX, maxY);
		}
	}

	bool Polygon::IsPointInside(const PFVector2& point)
	{
		// Raycast positive horizontal ray from point unto each edge 
		// ( with x >= point.x ) and check if it intersects - simple way is to check if
		// edge.begin and edge.end lies on opposite sides of horizontal line
		// Assume point.y = y0, edge.begin.y = y1,  edge.end.y = y2
		// then ( (y1-y0)(y2-y0) > 0 if lies on same side )
		// First however check bounding box
		if(_boundingBox.IsPointInside(point))
		{
			int crossesCount = 0;
			unsigned int v = 0;
			for(; v < _vertices.size() - 1; ++v)
			{
				// Check if ray moves through edge xbounds and if edge crosses it
				if( !(_vertices[v].x < point.x ||
					_vertices[v+1].x < point.x ) )
				{
					if( IsCloseToZero(_vertices[v].y - point.y) )
					{
						// Crosses vertex, count it as one pass
						++crossesCount;
						// HORIZONTAL LINES ??
					}
					else if( (_vertices[v].y - point.y)*(_vertices[v+1].y - point.y) < 0)
						++crossesCount;
				}
			}
			// Last check for last edge
			if( (!(_vertices[v].x < point.x ||
				_vertices[0].x < point.x )) &&
				( IsCloseToZero(_vertices[v].y - point.y) ||
				(_vertices[v].y - point.y)*(_vertices[0].y - point.y) < 0) )
				++crossesCount;

			// If cross count is odd point is inside
			return (crossesCount & 1) > 0;
		}
		return false;
	}

	bool Polygon::DoesIntersect(const Polygon& other)
	{
		return false;
	}

	//class PolygonMerger
	//{
	//private:
	//	Polygon::MergeEdgeList _edges;
	//	Polygon::MergeVertexQueue _vertices;
	//	Polygon::OpenedEdgesList _openedEdges;
	//	PFArray<int> _edgesToBeClosed;

	//public:
	//	Polygon Merge(const Polygon& first, const Polygon& second);

	//private:
	//	void ProcessVertex(Polygon::MergeVertex& vertex);
	//	void CheckEdgeForIntersections(Polygon::MergeEdge& edge);
	//	void OpenEdge(Polygon::MergeEdge& edge);
	//	void CloseEdge(Polygon::MergeEdge& edge);
	//	void DivideEdgeMiddlePoint(
	//		Polygon::MergeEdge& edgeOpened, 
	//		Polygon::MergeEdge& edgeOld,
	//		const PFVector2& point);
	//	void DivideEdgeParallel(
	//		Polygon::MergeEdge& edgeOpened, 
	//		Polygon::MergeEdge& edgeOld,
	//		const PFVector2& point);
	//	void LinkEdges(Polygon& polygon, const int startEdge);
	//};

	//Polygon Polygon::Merge(const Polygon& first, const Polygon& second)
	//{
	//	PolygonMerger merger;
	//	return merger.Merge(first, second);
	//}

	//Polygon PolygonMerger::Merge(const Polygon& first, const Polygon& second)
	//{
	//	// Create edge list/vertex queue of both polygons
	//	second.FillEdgeList(_edges);
	//	second.FillVertexQueue(_vertices, 0u);
	//	unsigned int offset = _edges.size();
	//	first.FillEdgeList(_edges);
	//	first.FillVertexQueue(_vertices, offset);
	//	// Store first vertex - construction of merged polygon will start with this one
	//	Polygon::MergeVertex firstVertex = _vertices.top();

	//	while(_vertices.size() > 0)
	//	{
	//		float xCoord = _vertices.top().Coords.x;
	//		// Before closing any edges process all vertices with same x
	//		while( _vertices.size() > 0 && 
	//			IsCloseToZeroV(_vertices.top().Coords.x - xCoord) )
	//		{
	//			auto vertex = _vertices.top();
	//			ProcessVertex(vertex);
	//			_vertices.pop();
	//		}

	//		// When all possible intersections so far are processed, we can close edges
	//		for(unsigned int v = 0; v < _edgesToBeClosed.size(); ++v)
	//		{
	//			CloseEdge(_edges[_edgesToBeClosed[v]]);
	//		}
	//		_edgesToBeClosed.clear();
	//	}
	//	Polygon merged;
	//	// Finally build merged polygon
	//	LinkEdges(merged, firstVertex.BeginEdge);

	//	return merged;
	//}

	//void PolygonMerger::ProcessVertex(Polygon::MergeVertex& vertex)
	//{
	//	// Check for edge intersection before opening it, to avoid
	//	// check with itself
	//	if(!_edges[vertex.BeginEdge].IsOpened)
	//		CheckEdgeForIntersections(_edges[vertex.BeginEdge]);
	//	if(!_edges[vertex.EndEdge].IsOpened)
	//		CheckEdgeForIntersections(_edges[vertex.EndEdge]);

	//	if(!_edges[vertex.BeginEdge].IsOpened)
	//		OpenEdge(_edges[vertex.BeginEdge]);
	//	else
	//		_edgesToBeClosed.push_back(_edges[vertex.BeginEdge].Index);

	//	if(!_edges[vertex.EndEdge].IsOpened)
	//		OpenEdge(_edges[vertex.EndEdge]);
	//	else
	//		_edgesToBeClosed.push_back(_edges[vertex.EndEdge].Index);
	//}

	//void PolygonMerger::CheckEdgeForIntersections(Polygon::MergeEdge& edge)
	//{
	//	// Check intersection with all opened edges and add to opened edges list
	//	Polygon::OpenedEdgesList::iterator eIt = _openedEdges.begin();
	//	unsigned int i = 0, size = _openedEdges.size(); 
	//	PFVector2 intersectionPoint;
	//	int intersectionType;
	//	for(; i < size; ++i, ++eIt) // Check against size as open list can change but new edges should not be tested against
	//	{
	//		if( edge.Owner == _edges[*eIt].Owner) // Dont check edges in same polygon
	//			continue;

	//		intersectionType = FindLinesIntersectionPoint(
	//			edge.GetBeginVertex(),
	//			edge.GetEndVertex(),
	//			_edges[*eIt].GetBeginVertex(),
	//			_edges[*eIt].GetEndVertex(),
	//			intersectionPoint);
	//		// Depending on intersection type, divide edge accordlingly
	//		if(intersectionType == IntersectionType::MiddlePoint)
	//		{
	//			DivideEdgeMiddlePoint(edge, _edges[*eIt], intersectionPoint);
	//		}
	//		else if(intersectionType == IntersectionType::EdgesParallel)
	//		{
	//			DivideEdgeParallel(edge, _edges[*eIt], intersectionPoint);
	//		}
	//		else if(intersectionType != IntersectionType::NoIntersection)
	//		{
	//			// DivideEdgeParallel(edge, _edges[*eIt], intersectionPoint);
	//		}
	//		// If there is no intersection leave it as it is
	//	}
	//}

	//inline void PolygonMerger::OpenEdge(Polygon::MergeEdge& edge)
	//{
	//	// Close edge and remove from opened list
	//	edge.IsOpened = true;
	//	_openedEdges.push_back(edge.Index);
	//}

	//inline void PolygonMerger::CloseEdge(Polygon::MergeEdge& edge)
	//{
	//	// Close edge and remove from opened list
	//	edge.IsOpened = false;
	//	_openedEdges.remove(edge.RemoveOnCloseIdx);
	//}

	//void PolygonMerger::DivideEdgeMiddlePoint(
	//	Polygon::MergeEdge& edge1, 
	//	Polygon::MergeEdge& edge2,
	//	const PFVector2& point)
	//{
	//	// So we need to divide 2 edges in point, resulting in 4 edges total
	//	// Goal is set edges next/previous indexes to proper ones
	//	// What is sure is that subedge from one polygon will be succedded by
	//	// subedge from second one ( as we assume polygon is not self-intersecting )
	//	// What is also sure that next edge have to be a most 'outer' ( technically 
	//	// have highest angle between current and next edge direction in 0-2pi range
	//	// to prserve cc-ordering ). So we need to join old edge1 with new edge2 
	//	Polygon::MergeEdge newEdge1(_edges.size(), edge1.Owner,
	//		point, edge2.GetEndVertex(),
	//		edge1.Index, edge2.NextEdge);

	//	Polygon::MergeEdge newEdge2(_edges.size() + 1, edge2.Owner,
	//		point, edge1.GetEndVertex(),
	//		edge2.Index, edge1.NextEdge);

	//	_edges[edge1.NextEdge].PrevoiusEdge = newEdge2.Index;
	//	edge1.NextEdge = newEdge1.Index;
	//	edge1.VertexEnd = point;
	//	_edges[edge2.NextEdge].PrevoiusEdge = newEdge1.Index;
	//	edge2.NextEdge = newEdge2.Index;
	//	edge2.VertexEnd = point;
	//	// As some other edges can still intersect with divided one it needs to be opened
	//	// but as intersection point is not pushed into v-queue, push it to additional array
	//	newEdge1.IsOpened = true;
	//	_openedEdges.push_back(newEdge1.Index);
	//	newEdge2.IsOpened = true;
	//	_openedEdges.push_back(newEdge2.Index);

	//	//// Queue closing of edges ( old or new ones ) that are left from i-point
	//	if(point.x > edge1.GetBeginVertex().x)
	//	{
	//		// Begin of old edge is on left side -> close it
	//		_edgesToBeClosed.push_back(edge1.Index);
	//		edge1.RemoveOnCloseIdx = newEdge2.Index;
	//	}
	//	else
	//		_edgesToBeClosed.push_back(newEdge1.Index);

	//	// Queue closing of edges ( old or new ones ) that are left from i-point
	//	if(point.x > edge2.GetBeginVertex().x)
	//	{
	//		// Begin of old edge is on left side -> close it
	//		_edgesToBeClosed.push_back(edge2.Index);
	//		edge2.RemoveOnCloseIdx = newEdge1.Index;
	//	}
	//	else
	//		_edgesToBeClosed.push_back(newEdge2.Index);

	//	// Push back new edges after all operations - otherwise 
	//	// references might be invalidated
	//	_edges.push_back(newEdge1);
	//	_edges.push_back(newEdge2);
	//}

	//inline void PolygonMerger::LinkEdges(Polygon& polygon, const int start)
	//{
	//	//- start with first vertex ( it is included for sure, as it is top-left )
	//	//- get vertex begin edge - it is first edge of merged polygon ( its index passed as start )
	//	//- add next edges' vertices untill polygon is closed
	//	int currentEdge = start;
	//	do
	//	{
	//		polygon.AddVertex(_edges[currentEdge].GetBeginVertex());
	//		currentEdge = _edges[currentEdge].NextEdge;
	//	}
	//	while(_edges[currentEdge].Index != start);
	//}

	//void PolygonMerger::DivideEdgeParallel(
	//	Polygon::MergeEdge& edgeOpened, 
	//	Polygon::MergeEdge& edgeOld,
	//	const PFVector2& point)
	//{
	//	// Extract edge from between points 
	//	// It will not be added to final polygon, but order of edges can change
	//	// We have situations :
	//	// (1) : eold.b >> enew.b >> eold.e >> enew.e ( e and b can be swapped )
	//	// or more visually one end of edge is between the other
	//	// (2) : eold.b >> enew.b >> enew.e >> eold.e ( eold/new e.b can be swapped )
	//	// or more visually one edge is fully within the other
	//	// In situation (1) handle few separate cases
	//	// In situaltion (2) create artificialy enlengthen prev/next edges
	//	// and treat it as MiddlePoint intersection

	//	// Find which vertices have smaller x coord
	//	bool oldBeginFirst = edgeOld.GetBeginVertex().x < edgeOld.GetEndVertex().x;
	//	bool newBeginFirst = edgeOpened.GetBeginVertex().x < edgeOpened.GetEndVertex().x;
	//	// Check for (1)
	//	if( oldBeginFirst )
	//	{ 
	//		if(newBeginFirst)
	//		{
	//			if( edgeOld.GetEndVertex().x > edgeOpened.GetBeginVertex().x )
	//			{
	//				// (1) eold.b >> enew.b >> eold.e >> enew.e
	//			}
	//			else
	//			{
	//				// (2) 
	//			}
	//		}
	//		else
	//		{
	//			if( edgeOld.GetEndVertex().x > edgeOpened.GetEndVertex().x )
	//			{
	//				// (1) eold.b >> enew.e >> eold.e >> enew.b
	//			}
	//			else
	//			{
	//				// (2)
	//			}
	//		}
	//	}
	//	else
	//	{
	//		if(newBeginFirst)
	//		{
	//			if( edgeOld.GetBeginVertex().x > edgeOpened.GetBeginVertex().x )
	//			{
	//				// (1) eold.e >> enew.b >> eold.b >> enew.e
	//			}
	//			else
	//			{
	//				// (2)
	//			}
	//		}
	//		else
	//		{
	//			if( edgeOld.GetBeginVertex().x > edgeOpened.GetEndVertex().x )
	//			{
	//				// (1) eold.b >> enew.e >> eold.e >> enew.b
	//			}
	//			else
	//			{
	//				// (2)
	//			}
	//		}
	//	}
	//		

	//	// Situation (2) :
	//	// Becouse of sorted order of opening edges, we know that 'opened' edge
	//	// is inside old one. 
	//}

	//void Polygon::FillEdgeList(MergeEdgeList& edges) const
	//{
	//	// Create and add edges (v,v+1) ( (max,0) , (0,1) outside loop )
	//	const unsigned int last = edges.size() + _vertices.size() - 1;
	//	const unsigned int first = edges.size();
	//	edges.reserve(last + 1);
	//	edges.push_back(MergeEdge(first, this, _vertices[0], _vertices[1], last, first + 1));
	//	for(unsigned int v = 1; v < _vertices.size() - 1; ++v)
	//	{
	//		edges.push_back(MergeEdge(first + v, this, _vertices[v], _vertices[v+1], 
	//			first + v-1, first + v+1));
	//	}
	//	edges.push_back(MergeEdge(last, this, _vertices[_vertices.size()-1], _vertices[0], last-1, first));
	//}

	//void Polygon::FillVertexQueue(MergeVertexQueue& vertices, const unsigned int offset) const
	//{
	//	const unsigned int last = offset + _vertices.size() - 1;
	//	vertices.push(MergeVertex(_vertices[0], offset, last));
	//	for(unsigned int v = 1; v < _vertices.size(); ++v)
	//	{
	//		vertices.push(MergeVertex(_vertices[v], offset+v, offset+v-1));
	//	}
	//}

	//Polygon::MergeEdge::MergeEdge() :
	//	IsOpened(false)
	//{ }

	//Polygon::MergeEdge::MergeEdge(
	//	const int index, 
	//	const Polygon* owner,
	//	const PFVector2& vbegin, 
	//	const PFVector2& vend, 
	//	const int prevEdge, 
	//	const int nextEdge) :
	//Index(index),
	//	VertexBegin(vbegin),
	//	VertexEnd(vend),
	//	PrevoiusEdge(prevEdge),
	//	NextEdge(nextEdge),
	//	IsOpened(false),
	//	RemoveOnCloseIdx(index),
	//	Owner(owner)
	//{ }

	//Polygon::MergeVertex::MergeVertex() :
	//	BeginEdge(-1),
	//	EndEdge(-1)
	//{ }

	//Polygon::MergeVertex::MergeVertex(
	//	const PFVector2& coords, 
	//	int ebegin, 
	//	int eend) :
	//Coords(coords),
	//	BeginEdge(ebegin),
	//	EndEdge(eend)
	//{ }
}