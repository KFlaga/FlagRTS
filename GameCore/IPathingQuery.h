#pragma once

#include <PathFindingBoundingShapes.h>
#include "DataTypes.h"

namespace FlagRTS
{
	class PhysicalObject;

	struct Dyscriminator
	{
	public:
		virtual bool operator()(PhysicalObject* object)
		{
			return true;
		}
	};

	// Interface for intersection query -> returns all objects that intersect
	// with supplied test shape
	// If it is created as query with dyscriminator it can be casted to
	// IBoxDyscryminatingPathingQuery<> with any discriminator 
	// ( assuring it have a parameterless constructor )
	__interface IBoxPathingQuery
	{
	public:
		virtual void SetTestShape(const PathFinding::Box& shape) = 0;
		virtual void Execute() = 0;
		virtual void Execute(Dyscriminator&) = 0;
		virtual Array<PhysicalObject*>& GetObjectsHit() = 0;
	};
}