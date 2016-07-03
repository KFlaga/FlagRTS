#include "PathFindingPolygon.h"

namespace PathFinding
{
	struct Vertex;

	struct Edge
	{
		int Index; // Index in edges array
		int BeginVertex;
		int EndVertex;
		int CloseIndex; // When edge is divided, actual edge to be closed on old end vertex may change
		float Tangens; // tg of angle (x-axis, edge)
		const Polygon* Owner;
		bool IsOpened;

		Edge(const int index, const Polygon* Owner, const int begin, const int end, 
			const PFArray<Vertex>& vertices );
		void ComputeTangens(const PFArray<Vertex>& vertices);
	};

	struct Vertex
	{
		int Index;
		PFVector2 Position;
		PFArray<int> EdgesIn;
		PFArray<int> EdgesOut;

		Vertex(const int index, const PFVector2& position, 
			const int initialEdgeBegin, const int initialEdgeEnd);
	};

	typedef PFArray<Edge> MergeEdgeArray;
	typedef PFArray<Vertex> MergeVertexArray;
	typedef PFArray<int> MergeVertexQueue;
	typedef PFList<int> OpenedEdgesList;

	class PolygonMerger
	{
	private:
		MergeEdgeArray _edges;
		MergeVertexArray _vertices;
		OpenedEdgesList _openedEdges;
		PFArray<int> _edgesToBeClosed;

	public:
		Polygon Merge(const Polygon& first, const Polygon& second);

	private:
		void AddEgesVerticesFromPolygon(const Polygon& polygon);

		void ProcessVertex(Vertex& vertex);
		void CheckEdgeForIntersections(Edge& edge);
		void OpenEdge(Edge& edge);
		void CloseEdge(Edge& edge);
		void DivideEdgeMiddlePoint(
			Edge& edgeOpened, 
			Edge& edgeOld,
			const PFVector2& point);
		void DivideEdgeParallel(
			Edge& edgeOpened, 
			Edge& edgeOld,
			const PFVector2& point);
		void LinkEdges(Polygon& polygon);
		
		// For sorting by X, then by Y ( so first in queue are elements with smaller x
		// and with same x with smaller y )
		struct SortVerticesByXYVal
		{
			static PolygonMerger* Merger;

			bool operator()(const int v1, const int v2)
			{
				return IsCloseToZeroV(Merger->_vertices[v1].Position.x - Merger->_vertices[v2].Position.x) ?
					Merger->_vertices[v1].Position.y > Merger->_vertices[v2].Position.y :
					Merger->_vertices[v1].Position.x > Merger->_vertices[v2].Position.x;
			}
		};
	};

	Polygon Polygon::Merge(const Polygon& first, const Polygon& second)
	{
		PolygonMerger merger;
		return merger.Merge(first, second);
	}

	PolygonMerger* PolygonMerger::SortVerticesByXYVal::Merger = 0;
	Polygon PolygonMerger::Merge(const Polygon& first, const Polygon& second)
	{
		AddEgesVerticesFromPolygon(first);
		AddEgesVerticesFromPolygon(second);

		MergeVertexQueue vertexQueue;
		const unsigned int size = _vertices.size();
		vertexQueue.reserve(size);
		for(unsigned int i = 0; i < size; ++i)
			vertexQueue.push_back(_vertices[i].Index);

		SortVerticesByXYVal sorter;
		std::sort(vertexQueue.begin(), vertexQueue.end(), sorter);

		for(unsigned int v = 0; v < size;)
		{
			float xCoord = _vertices[v].Position.x;
			// Before closing any edges process all vertices with same x
			while( v < size && 
				IsCloseToZeroV(_vertices[v].Position.x - xCoord) )
			{
				auto vertex = _vertices[v];
				ProcessVertex(vertex);
				++v;
			}

			// When all possible intersections so far are processed, we can close edges
			for(unsigned int v = 0; v < _edgesToBeClosed.size(); ++v)
			{
				CloseEdge(_edges[_edgesToBeClosed[v]]);
			}
			_edgesToBeClosed.clear();
		}
		Polygon merged;
		// Finally build merged polygon
		LinkEdges(merged);

		return merged;
	}

	inline void PolygonMerger::ProcessVertex(Vertex& vertex)
	{
		// Check for edge intersection before opening it, to avoid
		// check with itself
		const int beginEdge = vertex.EdgesOut[0];
		const int endEdge = vertex.EdgesIn[0];
		if(!_edges[beginEdge].IsOpened)
			CheckEdgeForIntersections(_edges[beginEdge]);
		if(!_edges[endEdge].IsOpened)
			CheckEdgeForIntersections(_edges[endEdge]);

		if(!_edges[beginEdge].IsOpened)
			OpenEdge(_edges[beginEdge]);
		else
			_edgesToBeClosed.push_back(_edges[beginEdge].Index);

		if(!_edges[endEdge].IsOpened)
			OpenEdge(_edges[endEdge]);
		else
			_edgesToBeClosed.push_back(_edges[endEdge].Index);
	}

	void PolygonMerger::CheckEdgeForIntersections(Edge& edge)
	{
		// Check intersection with all opened edges and add to opened edges list
		OpenedEdgesList::iterator eIt = _openedEdges.begin();
		unsigned int i = 0, size = _openedEdges.size(); 
		PFVector2 intersectionPoint;
		int intersectionType;
		for(; i < size; ++i, ++eIt) // Check against size as open list can change but new edges should not be tested against
		{
			if( edge.Owner == _edges[*eIt].Owner) // Dont check edges in same polygon
				continue;

			intersectionType = FindLinesIntersectionPoint(
				_vertices[edge.BeginVertex].Position,
				_vertices[edge.EndVertex].Position,
				_vertices[_edges[*eIt].BeginVertex].Position,
				_vertices[_edges[*eIt].EndVertex].Position,
				intersectionPoint);
			// Depending on intersection type, divide edge accordlingly
			if(intersectionType == IntersectionType::MiddlePoint)
			{
				DivideEdgeMiddlePoint(edge, _edges[*eIt], intersectionPoint);
			}
			else if(intersectionType == IntersectionType::EdgesParallel)
			{
				DivideEdgeParallel(edge, _edges[*eIt], intersectionPoint);
			}
			else if(intersectionType != IntersectionType::NoIntersection)
			{
				// DivideEdgeParallel(edge, _edges[*eIt], intersectionPoint);
			}
			// If there is no intersection leave it as it is
		}
	}

	inline void PolygonMerger::OpenEdge(Edge& edge)
	{
		// Close edge and remove from opened list
		edge.IsOpened = true;
		_openedEdges.push_back(edge.Index);
	}

	inline void PolygonMerger::CloseEdge(Edge& edge)
	{
		// Close edge and remove from opened list
		edge.IsOpened = false;
		_openedEdges.remove(edge.CloseIndex);
	}

	void PolygonMerger::DivideEdgeMiddlePoint(
		Edge& edge1, 
		Edge& edge2,
		const PFVector2& point)
	{
		
	}

	inline void PolygonMerger::LinkEdges(Polygon& polygon)
	{
		
	}

	void PolygonMerger::DivideEdgeParallel(
		Edge& edgeOpened, 
		Edge& edgeOld,
		const PFVector2& point)
	{
		// Extract edge from between points 
		// It will not be added to final polygon, but order of edges can change
		// We have situations :
		// (1) : eold.b >> enew.b >> eold.e >> enew.e ( e and b can be swapped )
		// or more visually one end of edge is between the other
		// (2) : eold.b >> enew.b >> enew.e >> eold.e ( eold/new e.b can be swapped )
		// or more visually one edge is fully within the other
		// In situation (1) handle few separate cases
		// In situaltion (2) create artificialy enlengthen prev/next edges
		// and treat it as MiddlePoint intersection

		//// Find which vertices have smaller x coord
		//bool oldBeginFirst = edgeOld.GetBeginVertex().x < edgeOld.GetEndVertex().x;
		//bool newBeginFirst = edgeOpened.GetBeginVertex().x < edgeOpened.GetEndVertex().x;
		//// Check for (1)
		//if( oldBeginFirst )
		//{ 
		//	if(newBeginFirst)
		//	{
		//		if( edgeOld.GetEndVertex().x > edgeOpened.GetBeginVertex().x )
		//		{
		//			// (1) eold.b >> enew.b >> eold.e >> enew.e
		//		}
		//		else
		//		{
		//			// (2) 
		//		}
		//	}
		//	else
		//	{
		//		if( edgeOld.GetEndVertex().x > edgeOpened.GetEndVertex().x )
		//		{
		//			// (1) eold.b >> enew.e >> eold.e >> enew.b
		//		}
		//		else
		//		{
		//			// (2)
		//		}
		//	}
		//}
		//else
		//{
		//	if(newBeginFirst)
		//	{
		//		if( edgeOld.GetBeginVertex().x > edgeOpened.GetBeginVertex().x )
		//		{
		//			// (1) eold.e >> enew.b >> eold.b >> enew.e
		//		}
		//		else
		//		{
		//			// (2)
		//		}
		//	}
		//	else
		//	{
		//		if( edgeOld.GetBeginVertex().x > edgeOpened.GetEndVertex().x )
		//		{
		//			// (1) eold.b >> enew.e >> eold.e >> enew.b
		//		}
		//		else
		//		{
		//			// (2)
		//		}
		//	}
		//}


		// Situation (2) :
		// Becouse of sorted order of opening edges, we know that 'opened' edge
		// is inside old one. 
	}

	
	void PolygonMerger::AddEgesVerticesFromPolygon(const Polygon& polygon)
	{
		// Save first and last vertex/edge indices ( same ) in arrays for this polygon
		const unsigned int last = _vertices.size() + polygon.GetVertexCount() - 1;
		const unsigned int first = _vertices.size();
		// Reserve memory at the begining
		_vertices.reserve(last + 1);
		_edges.reserve(last + 1);
		// Create and add vertices -> first and last outside loop
		_vertices.push_back(Vertex(first, polygon.GetVertex(0), first, last));
		for(unsigned int v = 1; v < _vertices.size() - 1; ++v)
		{
			_vertices.push_back(Vertex(first + v, polygon.GetVertex(v), first+v, first+v-1));
		}
		_vertices.push_back(Vertex(last, polygon.GetVertex(_vertices.size() - 1), last, last-1));
		// Same for edges
		_edges.push_back(Edge(first, &polygon, first, first+1, _vertices));
		for(unsigned int v = 1; v < _vertices.size() - 1; ++v)
		{
			_edges.push_back(Edge(first + v, &polygon, first+v, first+v+1, _vertices));
		}
		_edges.push_back(Edge(last, &polygon, last, first, _vertices));
	}

	Edge::Edge(const int index, 
		const Polygon* owner, 
		const int begin, 
		const int end, 
		const PFArray<Vertex>& vertices ) :
		Index(index),
		BeginVertex(begin),
		EndVertex(end),
		IsOpened(false),
		Owner(owner)
	{ 
		ComputeTangens(vertices);
	}

	inline void Edge::ComputeTangens(const PFArray<Vertex>& vertices)
	{

	}

	
	Vertex::Vertex(const int index, const PFVector2& position, 
			const int initialEdgeBegin, const int initialEdgeEnd) :
		Index(index),
		Position(position)
	{ 
		EdgesIn.reserve(4);
		EdgesOut.reserve(4);
		EdgesIn.push_back(initialEdgeEnd);
		EdgesOut.push_back(initialEdgeBegin);
	}
}