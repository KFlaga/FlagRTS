#pragma once

#include "PathingBvhNode.h"
#include <map>
#include <set>

namespace FlagRTS
{
	template<typename ShapeT>
	class Dbvh
	{
	public:
		typedef BvhNode<ShapeT> BvhNode;
		typedef ShapeT Shape;

	protected:
		struct CompareDepth
		{
		public:
			Dbvh* dbvh;

			bool operator()(unsigned short idx1, unsigned short idx2)
			{
				return dbvh->_allNodes[idx1].Depth > dbvh->_allNodes[idx2].Depth;
			}
		};

		std::map<size_t, unsigned short> _objToNodesMap;
		Array<BvhNode> _allNodes;
		Array<unsigned short> _freeIndices;
		std::set<unsigned short> _refitIndices;
		CompareDepth _cd;

		float _minMergeBenefit; // Minimal benefit in SAH after merging children to apply it
		float _minRotationBenefit; // Minimal benefit in SAH after rotation to apply it
		unsigned short _rootIdx;
		bool _isCreated;

	public:
		Dbvh(float minMergeBenefit = 0.75f, float minRotationBenefit = 0.75f) :
			_minMergeBenefit(minMergeBenefit),
			_minRotationBenefit(minRotationBenefit),
			_rootIdx(BvhNode::InvalidNode)
		{
			_cd.dbvh = this;
			_isCreated = false;
		}

		bool IsCreated() const { return _isCreated; }

		template<typename ObjT>
		void AddObject(ObjT* object, const Shape& objectBoundingShape)
		{
			AddLeaf(_rootIdx, reinterpret_cast<size_t>(object), objectBoundingShape);
		}

		template<typename ObjT>
		void RemoveObject(ObjT* object)
		{
			auto leafIdxIt = _objToNodesMap.find(reinterpret_cast<size_t>(object));
			if(leafIdxIt != _objToNodesMap.end())
			{
				RemoveLeaf(_allNodes[leafIdxIt->second].Parent, leafIdxIt->second);
				_objToNodesMap.erase(leafIdxIt);
			}
		}

		template<typename ObjT>
		void ObjectChanged(ObjT* object, const Shape& newBoundingShape)
		{
			auto leafIdxIt = _objToNodesMap.find(reinterpret_cast<size_t>(object));
			if(leafIdxIt != _objToNodesMap.end())
			{
				RefitAfterObjectChanged(leafIdxIt->second, newBoundingShape);
			}
		}

		template<typename ObjT>
		void CreateInitialBvh(Array<std::pair<ObjT*,Shape>>& objects)
		{
			if( objects.size() >= 2)
			{
				CreateInitialBvhInternal(objects);
			}
		}

		void Optimize()
		{
			CompareDepth cmpDepth;
			cmpDepth.dbvh = this;

			while(_refitIndices.size() > 0)
			{
				unsigned short idx = *_refitIndices.begin();
				TryRotateNodes( _allNodes[idx] );
				_refitIndices.erase(idx);
			}

			_refitIndices.clear();
		}

		// Returns all objects intersected by given ray
		// Ray type must support operation: bool Intersect(const Shape&, VectorT&)
		// VectorT is Vector2 or Vector3 etc. depending on Shape dimensions
		// It may be also any other custom parameter for arbitrary query
		template<typename ObjT, typename RayT, typename VectorT>
		void TestRay(const RayT& ray, Array<std::pair<ObjT*, VectorT>>& hitObjects)
		{
			TestRayInternal(_allNodes[_rootIdx], ray, hitObjects);
		}

		// Version w/o intersection points ( ray must support bool Intersect(const Shape&) )
		// Also dyscryminator object may be supplied to accept only choosen hits
		// ObjDyscriminator should have bool operator()(ObjT*) overloaded -> hit accepted on true
		template<typename ObjT, typename TestShapeT, typename ObjDyscriminator>
		void TestGeneric(const TestShapeT& shape, Array<ObjT*>& hitObjects, ObjDyscriminator& acceptHit = ObjDyscriminator())
		{
			TestGenericInternal(_allNodes[_rootIdx], shape, hitObjects, acceptHit);
		}

		// void FindCollidingPairs();

		void Clear()
		{
			_allNodes.clear();
			_freeIndices.clear();
			_refitIndices.clear();
			_objToNodesMap.clear();
			_rootIdx = BvhNode::InvalidNode;
			_isCreated = false;
		}

		void DebugDraw(PathingDrawer* drawer);

	protected:
		template<typename ObjT, typename RayT, typename VectorT>
		void TestRayInternal(BvhNode& node, const RayT& ray, Array<std::pair<ObjT*, VectorT>>& hitObjects)
		{
			VectorT intersectionPoint;
			if(ray.Intersect(node.BoundingVolume, intersectionPoint))
			{
				if(node.IsLeaf)
				{
					hitObjects.push_back(std::make_pair(
						node.GetUserObject<ObjT>(), intersectionPoint));
				}
				else
				{
					TestRayInternal(_allNodes[node.LeftChild], ray, hitObjects);
					TestRayInternal(_allNodes[node.RightChild], ray, hitObjects);
				}
			}
		}

		template<typename ObjT, typename TestShapeT, typename ObjDyscriminator>
		void TestGenericInternal(BvhNode& node, 
			const TestShapeT& shape, Array<ObjT*>& hitObjects, 
			ObjDyscriminator& acceptHit)
		{
			if(shape.Intersect(node.BoundingVolume))
			{
				if(node.IsLeaf)
				{
					if(acceptHit(node.GetUserObject<ObjT>()))
						hitObjects.push_back(node.GetUserObject<ObjT>());
				}
				else
				{
					TestGenericInternal(_allNodes[node.LeftChild], shape, hitObjects, acceptHit);
					TestGenericInternal(_allNodes[node.RightChild], shape, hitObjects, acceptHit);
				}
			}
		}

		template<typename ObjT>
		void CreateInitialBvhInternal(Array<std::pair<ObjT*,Shape>>& objects)
		{
			_allNodes.reserve(objects.size());
			_rootIdx = 0;
			BvhNode rootNode;
			rootNode.Depth = 0;
			rootNode.Index = 0;
			rootNode.IsLeaf = false;
			rootNode.Parent = BvhNode::InvalidNode;

			BvhNode left;
			left.Depth = 1;
			left.Index = 1;
			left.IsLeaf = true;
			left.Parent = 0;
			BvhNode right;
			right.Depth = 1;
			right.Index = 2;
			right.IsLeaf = true;
			right.Parent = 0;
			if( BvhNode::IsShapeOnLeftSide(objects[0].second, objects[1].second) )
			{
				left.BoundingVolume = objects[0].second;
				left.SetUserObject(objects[0].first);
				_objToNodesMap[reinterpret_cast<size_t>(objects[0].first)] = 1;
				right.BoundingVolume = objects[1].second;
				right.SetUserObject(objects[1].first);
				_objToNodesMap[reinterpret_cast<size_t>(objects[1].first)] = 2;
			}
			else
			{
				right.BoundingVolume = objects[0].second;
				right.SetUserObject(objects[0].first);
				_objToNodesMap[reinterpret_cast<size_t>(objects[0].first)] = 2;
				left.BoundingVolume = objects[1].second;
				left.SetUserObject(objects[1].first);
				_objToNodesMap[reinterpret_cast<size_t>(objects[1].first)] = 1;
			}

			rootNode.BoundingVolume = left.BoundingVolume.ExpandToFitCopy(right.BoundingVolume);
			rootNode.LeftChild = left.Index;
			rootNode.RightChild = right.Index;

			_allNodes.push_back(rootNode);
			_allNodes.push_back(left);
			_allNodes.push_back(right);

			for(unsigned int i = 2; i < objects.size(); ++i)
			{
				AddObject(objects[i].first, objects[i].second);
			}

			_isCreated = true;
		}

		void AddLeaf(unsigned short nodeIdx, size_t objHandle, const Shape& objShape)
		{
			if (_allNodes[nodeIdx].IsLeaf)
			{
				// Split node to 2 - create nodes and set correct params
				unsigned short leftIdx, rightIdx;
				GetTwoIndices(leftIdx, rightIdx);
				SplitNode(objHandle, objShape, _allNodes[nodeIdx],
					_allNodes[leftIdx], _allNodes[rightIdx]);
			}
			else
			{
				// find the best way to add this object.. 3 options..
				// 1. send to left node  (L+N,R)
				// 2. send to right node (L,R+N)
				// 3. merge and pushdown left-and-right node (L+R,N)
				BvhNode& node = _allNodes[nodeIdx];
				BvhNode& left = _allNodes[node.LeftChild];
				BvhNode& right = _allNodes[node.RightChild];

				float leftSAH = left.GetSAH();
				float rightSAH = right.GetSAH();
				float sendLeftSAH = rightSAH + BvhNode::GetSAH(left.BoundingVolume, objShape);    // (L+N,R)
				float sendRightSAH = leftSAH + BvhNode::GetSAH(right.BoundingVolume, objShape);   // (L,R+N)
				Shape mergedShape(left.BoundingVolume.ExpandToFitCopy(right.BoundingVolume));
				float mergedLeftAndRightSAH = BvhNode::GetSAH(mergedShape) + BvhNode::GetSAH(objShape); // (L+R,N)

				// Doing a merge-and-pushdown can be expensive, so we only do it if it's notably better
				if (mergedLeftAndRightSAH < (std::min(sendLeftSAH, sendRightSAH)) * _minMergeBenefit)
				{
					unsigned short mergedIdx, objectIdx;
					GetTwoIndices(mergedIdx, objectIdx);
					BvhNode& node = _allNodes[nodeIdx];
					MergeChildren(objHandle, objShape, node,
						_allNodes[node.LeftChild], _allNodes[node.RightChild],
						_allNodes[mergedIdx], _allNodes[objectIdx], mergedShape);
				}
				else
				{
					if (sendLeftSAH < sendRightSAH)
					{
						// send left
						AddLeaf(left.Index, objHandle, objShape);
					}
					else
					{
						// send right
						AddLeaf(right.Index, objHandle, objShape);
					}
				}
			}

		}

		void SplitNode(size_t objHandle, const Shape& objShape, BvhNode& node, BvhNode& left, BvhNode& right)
		{
			_ASSERT(node.IsLeaf);
			// Split leaf node into 2 child nodes :
			// Set common paramters
			left.Depth = right.Depth = node.Depth + 1;
			left.IsLeaf = right.IsLeaf = true;
			left.Parent = right.Parent = node.Index;
			// Check which shape should be on left side and set them to nodes accordlingly
			if( BvhNode::IsShapeOnLeftSide( node.BoundingVolume, objShape ) )
			{
				left.BoundingVolume = node.BoundingVolume;
				left.UserObjectHandle = node.UserObjectHandle;
				_objToNodesMap[node.UserObjectHandle] = left.Index;

				right.BoundingVolume = objShape;
				right.UserObjectHandle = objHandle;
				_objToNodesMap[objHandle] = right.Index;
			}
			else
			{
				right.BoundingVolume = node.BoundingVolume;
				right.UserObjectHandle = node.UserObjectHandle;
				_objToNodesMap[node.UserObjectHandle] = right.Index;

				left.BoundingVolume = objShape;
				left.UserObjectHandle = objHandle;
				_objToNodesMap[objHandle] = left.Index;
			}
			// Set childs + refit this node
			node.LeftChild = left.Index;
			node.RightChild = right.Index;
			node.IsLeaf = false;
			node.RefitVolume(&_allNodes);
		}

		void MergeChildren(size_t objHandle, const Shape& objShape, 
			BvhNode& node, BvhNode& left, BvhNode& right,
			BvhNode& mergedNode, BvhNode& objectNode, const Shape& mergedShape)
		{
			// Push left/right children to merged node and make merged a child
			mergedNode.LeftChild = node.LeftChild;
			mergedNode.RightChild = node.RightChild;
			mergedNode.Parent = node.Index;
			left.Parent = mergedNode.Index;
			right.Parent = mergedNode.Index;
			mergedNode.BoundingVolume = mergedShape;
			mergedNode.IsLeaf = false;

			objectNode.Parent = node.Index;
			objectNode.UserObjectHandle = objHandle;
			objectNode.BoundingVolume = objShape;
			objectNode.IsLeaf = true;
			_objToNodesMap[objHandle] = objectNode.Index;

			// Set new left/right
			if( BvhNode::IsShapeOnLeftSide( node.BoundingVolume, objShape) )
			{
				node.LeftChild = mergedNode.Index;
				node.RightChild = objectNode.Index;
			}
			else
			{
				node.LeftChild = objectNode.Index;
				node.RightChild = mergedNode.Index;
			}
			node.SetDepth(node.Depth, &_allNodes); // propagate new depths to our children.
			node.RefitVolume(&_allNodes);
		}

		void RemoveLeaf(unsigned short nodeIdx, unsigned short leafIdx)
		{
			BvhNode& node = _allNodes[nodeIdx];
			_ASSERT(!node.IsLeaf);
			_ASSERT(_allNodes[leafIdx].IsLeaf);
			_ASSERT(leafIdx == node.LeftChild || leafIdx == node.RightChild);
			unsigned short keepIdx;

			if (leafIdx == node.LeftChild)
			{
				keepIdx = node.RightChild;
			}
			else
			{
				keepIdx = node.LeftChild;
			}

			BvhNode& keepLeaf = _allNodes[keepIdx];
			// "become" the leaf we are keeping.
			node.BoundingVolume = keepLeaf.BoundingVolume;
			node.UserObjectHandle = keepLeaf.UserObjectHandle;
			node.IsLeaf = keepLeaf.IsLeaf;

			if (!node.IsLeaf)
			{
				_allNodes[node.LeftChild].Parent = node.Index; 
				_allNodes[node.RightChild].Parent = node.Index;  // reassign child parents..
				node.SetDepth(node.Depth, &_allNodes); // this reassigns depth for our children
			}
			else
			{
				_objToNodesMap[node.UserObjectHandle] = node.Index;
			}

			// Remove actual node ( add to its index free indices )
			_freeIndices.push_back(_allNodes[leafIdx].Index);
			_allNodes[leafIdx].Index = BvhNode::InvalidNode;

			// propagate our new volume..
			if (node.Parent != BvhNode::InvalidNode)
			{
				_allNodes[node.Parent].RefitVolume(&_allNodes);
			}
		}

		void GetTwoIndices(unsigned short& idx1, unsigned short& idx2)
		{
			if(_freeIndices.size() >= 2)
			{
				idx1 = _freeIndices[_freeIndices.size() - 1];
				idx2 = _freeIndices[_freeIndices.size() - 2];
				_freeIndices.resize(_freeIndices.size() - 2);
			}
			else
			{
				idx1 = _allNodes.size();
				idx2 = _allNodes.size() + 1;
				_allNodes.resize( _allNodes.size() + 2 );
			}
			_allNodes[idx1].Index = idx1;
			_allNodes[idx2].Index = idx2;
		}

		void RefitAfterObjectChanged(unsigned short leafIdx, const Shape& newBoundingShape)
		{
			BvhNode& node = _allNodes[leafIdx];
			node.BoundingVolume = newBoundingShape;
			if ( _allNodes[node.Parent].RefitVolume(&_allNodes) ) 
			{                  
				_refitIndices.insert(node.Parent);
			}
		}

		// The list of all candidate rotations, from "Fast, Effective BVH Updates for Animated Scenes", Figure 1.
		enum RotationType
		{
			L_RL, L_RR, R_LL, R_LR, LL_RR, LL_RL, RotCount, NONE
		};

		class Rotation
		{ 
		public:
			float SAH;
			RotationType rot;

			Rotation(float sah, RotationType rt)
			{
				SAH = sah;
				rot = rt;
			}

			bool operator==(const Rotation& other)
			{
				return SAH == other.SAH;
			}
		};

		void TryRotateNodes(BvhNode& node)
		{
			// if we are not a grandparent, then we can't rotate, so queue our parent and bail out
			if (_allNodes[node.LeftChild].IsLeaf && _allNodes[node.RightChild].IsLeaf)
			{
				if (node.HaveParent())
				{
					_refitIndices.insert(node.Parent);
				}
				return;
			}

			// for each rotation, check that there are grandchildren as necessary (aka not a leaf)
			// then compute total SAH cost of our branches after the rotation.

			Rotation bestRot = FindBestRotation(node);

			// perform the best rotation...            
			if (bestRot.rot == RotationType::NONE)
			{
				// if the best rotation is no-rotation... we check our parents anyhow..                
				if (node.HaveParent())
				{
					// but only do it some random percentage of the time.
					if ( std::rand() % 10 < 2)
					{
						_refitIndices.insert(node.Parent);
					}
				}
			}
			else
			{
				if (node.HaveParent()) 
				{ 
					_refitIndices.insert(node.Parent);
				}

				float mySAH = _allNodes[node.LeftChild].GetSAH() + _allNodes[node.RightChild].GetSAH();
				if ( bestRot.SAH > mySAH * _minRotationBenefit)
				{
					return; // the benefit is not worth the cost
				}

				RotateNodes(node, bestRot);
			}
		}

		Rotation FindBestRotation(BvhNode& node)
		{
			if( _allNodes[node.LeftChild].IsLeaf )
				return FindBestRotationLeftLeaf(node);
			else if( _allNodes[node.RightChild].IsLeaf )
				return FindBestRotationRightLeaf(node);
			else
				return FindBestRotationNoLeafs(node);	
		}

		void RotateNodes(BvhNode& node, Rotation rotation)
		{
			if( _allNodes[node.LeftChild].IsLeaf )
				RotateNodesLeftLeaf(node, rotation);
			else if( _allNodes[node.RightChild].IsLeaf )
				RotateNodesRightLeaf(node, rotation);
			else
				RotateNodesNoLeafs(node, rotation);
		}

		Rotation FindBestRotationLeftLeaf(BvhNode& node)
		{
			Rotation bestRot(1e16f, NONE);
			BvhNode& cLeft = _allNodes[node.LeftChild];
			BvhNode& cRight = _allNodes[node.RightChild];
			BvhNode& cRight_gcLeft = _allNodes[_allNodes[node.RightChild].LeftChild];
			BvhNode& cRight_gcRight = _allNodes[_allNodes[node.RightChild].RightChild];
			// Find best rotation
			float rotSah;
			// Rotation : L_RL
			rotSah = cRight_gcLeft.GetSAH() + BvhNode::GetSAH(cLeft.BoundingVolume, cRight_gcRight.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::L_RL;
			}
			// Rotation : L_RR
			rotSah = cRight_gcRight.GetSAH() + BvhNode::GetSAH(cLeft.BoundingVolume, cRight_gcLeft.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::L_RR;
			}
			return bestRot;
		}

		Rotation FindBestRotationRightLeaf(BvhNode& node)
		{
			Rotation bestRot(1e16f, NONE);
			BvhNode& cLeft = _allNodes[node.LeftChild];
			BvhNode& cLeft_gcLeft = _allNodes[_allNodes[node.LeftChild].LeftChild];
			BvhNode& cLeft_gcRight = _allNodes[_allNodes[node.LeftChild].RightChild];
			BvhNode& cRight = _allNodes[node.RightChild];
			// Find best rotation
			float rotSah;
			// Rotation : R_LL
			rotSah = cLeft_gcLeft.GetSAH() + BvhNode::GetSAH(cRight.BoundingVolume, cLeft_gcRight.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::R_LL;
			}
			// Rotation : R_LR
			rotSah = cLeft_gcRight.GetSAH() + BvhNode::GetSAH(cRight.BoundingVolume, cLeft_gcLeft.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::R_LR;
			}
			return bestRot;
		}

		Rotation FindBestRotationNoLeafs(BvhNode& node)
		{
			Rotation bestRot(1e16f, NONE);
			BvhNode& cLeft = _allNodes[node.LeftChild];
			BvhNode& cLeft_gcLeft = _allNodes[_allNodes[node.LeftChild].LeftChild];
			BvhNode& cLeft_gcRight = _allNodes[_allNodes[node.LeftChild].RightChild];
			BvhNode& cRight = _allNodes[node.RightChild];
			BvhNode& cRight_gcLeft = _allNodes[_allNodes[node.RightChild].LeftChild];
			BvhNode& cRight_gcRight = _allNodes[_allNodes[node.RightChild].RightChild];
			// Find best rotation
			float rotSah;
			// Rotation : L_RL
			rotSah = cRight_gcLeft.GetSAH() + BvhNode::GetSAH(cLeft.BoundingVolume, cRight_gcRight.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::L_RL;
			}
			// Rotation : L_RR
			rotSah = cRight_gcRight.GetSAH() + BvhNode::GetSAH(cLeft.BoundingVolume, cRight_gcLeft.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::L_RR;
			}
			// Rotation : R_LL
			rotSah = cLeft_gcLeft.GetSAH() + BvhNode::GetSAH(cRight.BoundingVolume, cLeft_gcRight.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::R_LL;
			}
			// Rotation : R_LR
			rotSah = cLeft_gcRight.GetSAH() + BvhNode::GetSAH(cRight.BoundingVolume, cLeft_gcLeft.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::R_LR;
			}
			// Rotation : LL_RR
			rotSah = BvhNode::GetSAH(cRight_gcRight.BoundingVolume, cLeft_gcRight.BoundingVolume) + 
				BvhNode::GetSAH(cRight_gcLeft.BoundingVolume, cLeft_gcLeft.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::LL_RR;
			}
			// Rotation : LL_RL
			rotSah = BvhNode::GetSAH(cRight_gcLeft.BoundingVolume, cLeft_gcRight.BoundingVolume) + 
				BvhNode::GetSAH(cLeft_gcLeft.BoundingVolume, cRight_gcRight.BoundingVolume);
			if( rotSah < bestRot.SAH )
			{
				bestRot.SAH = rotSah;
				bestRot.rot = RotationType::LL_RL;
			}
			return bestRot;
		}

		void RotateNodesLeftLeaf(BvhNode& node, Rotation rotation)
		{
			BvhNode& cLeft = _allNodes[node.LeftChild];
			BvhNode& cRight = _allNodes[node.RightChild];
			BvhNode& cRight_gcLeft = _allNodes[_allNodes[node.RightChild].LeftChild];
			BvhNode& cRight_gcRight = _allNodes[_allNodes[node.RightChild].RightChild];
			switch (rotation.rot)
			{
			case RotationType::NONE: break;
				// child to grandchild rotations
			case RotationType::L_RL: 
				// Swap parent/child indices of cLeft and cRight_gcLeft
				cLeft.Parent = cRight.Index;
				cLeft.SetDepth(cLeft.Depth+1, &_allNodes);
				cRight.LeftChild = cLeft.Index;

				cRight_gcLeft.Parent = node.Index;
				cRight_gcLeft.SetDepth(cRight_gcLeft.Depth-1, &_allNodes);
				node.LeftChild = cRight_gcLeft.Index;

				cRight.RefitVolume(&_allNodes);
				break;
			case RotationType::L_RR: 
				cLeft.Parent = cRight.Index;
				cRight.RightChild = cLeft.Index;
				cLeft.SetDepth(cLeft.Depth+1, &_allNodes);

				cRight_gcRight.Parent = node.Index;
				node.LeftChild = cRight_gcRight.Index;
				cRight_gcRight.SetDepth(cRight_gcRight.Depth-1, &_allNodes);

				cRight.RefitVolume(&_allNodes); 
				break;
			default: 
				_ASSERT(false);
			}
		}

		void RotateNodesRightLeaf(BvhNode& node, Rotation rotation)
		{
			BvhNode& cLeft = _allNodes[node.LeftChild];
			BvhNode& cLeft_gcLeft = _allNodes[_allNodes[node.LeftChild].LeftChild];
			BvhNode& cLeft_gcRight = _allNodes[_allNodes[node.LeftChild].RightChild];
			BvhNode& cRight = _allNodes[node.RightChild];
			switch (rotation.rot)
			{
			case RotationType::NONE: break;
				// child to grandchild rotations
			case RotationType::R_LL: 
				cRight.Parent = cLeft.Index;
				cLeft.LeftChild = cRight.Index;
				cRight.SetDepth(cRight.Depth+1, &_allNodes);

				cLeft_gcLeft.Parent = node.Index;
				node.RightChild = cLeft_gcLeft.Index;
				cLeft_gcLeft.SetDepth(cLeft_gcLeft.Depth-1, &_allNodes);

				cLeft.RefitVolume(&_allNodes); 
				break;
			case RotationType::R_LR:
				cRight.Parent = cLeft.Index;
				cLeft.RightChild = cRight.Index;
				cRight.SetDepth(cRight.Depth+1, &_allNodes);

				cLeft_gcRight.Parent = node.Index;
				node.RightChild = cLeft_gcRight.Index;
				cLeft_gcRight.SetDepth(cLeft_gcRight.Depth-1, &_allNodes);

				cLeft.RefitVolume(&_allNodes);
				break;
			default: 
				_ASSERT(false);
			}
		}

		void RotateNodesNoLeafs(BvhNode& node, Rotation rotation)
		{
			BvhNode& cLeft = _allNodes[node.LeftChild];
			BvhNode& cLeft_gcLeft = _allNodes[_allNodes[node.LeftChild].LeftChild];
			BvhNode& cLeft_gcRight = _allNodes[_allNodes[node.LeftChild].RightChild];
			BvhNode& cRight = _allNodes[node.RightChild];
			BvhNode& cRight_gcLeft = _allNodes[_allNodes[node.RightChild].LeftChild];
			BvhNode& cRight_gcRight = _allNodes[_allNodes[node.RightChild].RightChild];
			switch (rotation.rot)
			{
			case RotationType::NONE: break;
				// child to grandchild rotations
			case RotationType::L_RL: 
				// Swap parent/child indices of cLeft and cRight_gcLeft
				cLeft.Parent = cRight.Index;
				cLeft.SetDepth(cLeft.Depth+1, &_allNodes);
				cRight.LeftChild = cLeft.Index;

				cRight_gcLeft.Parent = node.Index;
				cRight_gcLeft.SetDepth(cRight_gcLeft.Depth-1, &_allNodes);
				node.LeftChild = cRight_gcLeft.Index;

				cRight.RefitVolume(&_allNodes);
				break;
			case RotationType::L_RR: 
				cLeft.Parent = cRight.Index;
				cRight.RightChild = cLeft.Index;
				cLeft.SetDepth(cLeft.Depth+1, &_allNodes);

				cRight_gcRight.Parent = node.Index;
				node.LeftChild = cRight_gcRight.Index;
				cRight_gcRight.SetDepth(cRight_gcRight.Depth-1, &_allNodes);

				cRight.RefitVolume(&_allNodes); 
				break;
			case RotationType::R_LL: 
				cRight.Parent = cLeft.Index;
				cLeft.LeftChild = cRight.Index;
				cRight.SetDepth(cRight.Depth+1, &_allNodes);

				cLeft_gcLeft.Parent = node.Index;
				node.RightChild = cLeft_gcLeft.Index;
				cLeft_gcLeft.SetDepth(cLeft_gcLeft.Depth-1, &_allNodes);

				cLeft.RefitVolume(&_allNodes); 
				break;
			case RotationType::R_LR:
				cRight.Parent = cLeft.Index;
				cLeft.RightChild = cRight.Index;
				cRight.SetDepth(cRight.Depth+1, &_allNodes);

				cLeft_gcRight.Parent = node.Index;
				node.RightChild = cLeft_gcRight.Index;
				cLeft_gcRight.SetDepth(cLeft_gcRight.Depth-1, &_allNodes);

				cLeft.RefitVolume(&_allNodes);
				break;

				// grandchild to grandchild rotations
			case RotationType::LL_RR:
				cLeft_gcLeft.Parent = cRight.Index;
				cRight.RightChild = cLeft_gcLeft.Index;

				cRight_gcRight.Parent = cLeft.Index;
				cLeft.LeftChild = cRight_gcRight.Index;

				cLeft.RefitVolume(&_allNodes, false); 
				cRight.RefitVolume(&_allNodes);
				break;
			case RotationType::LL_RL: 
				cLeft_gcLeft.Parent = cRight.Index;
				cRight.LeftChild = cLeft_gcLeft.Index;

				cRight_gcLeft.Parent = cLeft.Index;
				cLeft.LeftChild = cRight_gcLeft.Index;

				cLeft.RefitVolume(&_allNodes, false); 
				cRight.RefitVolume(&_allNodes);
				break;
			default: 
				_ASSERT(false);
			}
		}
	};

	template<typename ShapeT>
	void Dbvh<ShapeT>::DebugDraw(PathingDrawer* drawer)
	{
		// Draw each valid node 
		for(unsigned int i = 0; i < _allNodes.size(); ++i)
		{
			if( _allNodes[i].Index != BvhNode::InvalidNode )
			{
				_allNodes[i].BoundingVolume.DebugDraw(drawer);
			}
		}
	}
}