#pragma once

#include "DataTypes.h"

namespace FlagRTS
{
	class PhysicalObject;
	class CollisionShape;

	/**
	Allows for custom dyscrimination of objects from PathingQuery results.
	If Dyscriminator returns true then hit-test is accepted.
	*/
	struct Dyscriminator
	{
	public:
		virtual bool operator()(PhysicalObject* object)
		{
			return true;
		}
	};
	
	/**
	Interface for intersection (collision) test query.
	Finds all objects added to PathingSystem that intersects with supplied test shape.
	*/
	class IPathingQuery
	{
	protected:
		Array<PhysicalObject*> _hitObjects;
		CollisionShape* _collisionShape;

	public:
		/**
		Finds all intersections with TestShape's bounding box.
		TestShape must be set before call.
		*/
		virtual void Execute() = 0;
		
		/**
		Finds all collisions with TestShape's bounding box that are
		blocked by test object (test.filter.blocks(other.filter) == true)
		TestShape must be set before call.
		*/
		virtual void Execute_CatchBlockedByTestObject() = 0;

		/**
		Finds all collisions with TestShape's bounding box that are
		blocking test object (test.filter.isBlockedBy(other.filter) == true)
		TestShape must be set before call.
		*/
		virtual void Execute_CatchBlocksTestObject() = 0;
		
		/**
		Finds all collisions with TestShape's bounding box that are
		accepted by custom Dyscriminator.
		TestShape must be set before call.
		*/
		virtual void Execute(Dyscriminator&) = 0;

		void SetTestShape(CollisionShape* shape)
		{
			_collisionShape = shape;
		}
		
		CollisionShape* GetTestShape() const
		{
			return _collisionShape;
		}

		/**
		Returns results of query.
		Results are valid after call to Execute() untill next call to Execute()
		*/
		Array<PhysicalObject*>& GetObjectsHit()
		{
			return _hitObjects;
		}
	};
}