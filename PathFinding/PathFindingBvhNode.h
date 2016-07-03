#pragma once

#include "PathFindingBoundingShapes.h"

namespace PathFinding
{
	template<typename ShapeT>
	class BvhNode
	{
	public:
		typedef ShapeT Shape;
		static const unsigned short InvalidNode = (unsigned short)(-1);

		Shape BoundingVolume;
		unsigned short Depth;
		unsigned short Parent;
		union
		{ 
			struct
			{
				unsigned short LeftChild;
				unsigned short RightChild;
			};
			size_t UserObjectHandle;
		};
		unsigned short Index;
		bool IsLeaf;

		BvhNode() :
			Index(InvalidNode)
		{ }

		template<typename ObjT>
		ObjT* GetUserObject() const 
		{ 
			_ASSERT(IsLeaf);
			return reinterpret_cast<ObjT*>(UserObjectHandle); 
		}

		template<typename ObjT>
		void SetUserObject(ObjT* obj) 
		{ 
			UserObjectHandle = reinterpret_cast<size_t>(obj); 
		}

		float GetSAH() const
		{
			return BoundingVolume.GetArea();
		}

		static float GetSAH(const Shape& ofShape)
		{
			return ofShape.GetArea();
		}
		
		// Returns SAH of shape containing both shapes
		static float GetSAH(const Shape& shape1, const Shape& shape2)
		{
			return shape1.GetAreaExpanded(shape2);
		}

		bool HaveParent()
		{
			return Parent != InvalidNode;
		}

		// Returns true if 'leftShape' should remain as left child after node split or
		// false if 'leftShape' should be swapped with 'rightShape'
		static bool IsShapeOnLeftSide(const Shape& leftShape, const Shape& rightShape)
		{
			PFVector2 minCorner1 = leftShape.GetCenter() - leftShape.GetExtends() * 0.5f;
			PFVector2 minCorner2 = rightShape.GetCenter() - rightShape.GetExtends() * 0.5f;

			// For now decide by top-left position - one with smaller sum of corner coors will be left one
			return minCorner1.x + minCorner1.y < minCorner2.y + minCorner2.y;
		}

		bool ExpandNodeVolume(const Shape& shapeToFit, PFArray<BvhNode>* allNodes, bool recursive = true) 
		{
			bool expanded = BoundingVolume.ExpandToFit(shapeToFit);
			if( recursive && expanded && HaveParent())
			{
				(*allNodes)[Parent].ExpandNodeVolume(BoundingVolume, allNodes);
			}
			return expanded;
		}

		bool RefitVolume(PFArray<BvhNode>* allNodes, bool recursive = true) 
		{
			_ASSERT(!IsLeaf);

			Shape newVolume = (*allNodes)[LeftChild].BoundingVolume.ExpandToFitCopy(
				(*allNodes)[RightChild].BoundingVolume);

			if (BoundingVolume != newVolume) 
			{
				BoundingVolume = newVolume;
				if(recursive && HaveParent()) 
					(*allNodes)[Parent].RefitVolume(allNodes, recursive);
				return true;
			} 
			else 
			{
				return false;
			}
		}

		void SetDepth(unsigned int depth, PFArray<BvhNode>* allNodes)
		{
			Depth = depth;
			if( !IsLeaf )
			{
				(*allNodes)[LeftChild].SetDepth(depth + 1u, allNodes);
				(*allNodes)[RightChild].SetDepth(depth + 1u, allNodes);
			}
		}
	};
}