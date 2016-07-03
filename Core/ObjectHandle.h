#pragma once

// Simple types for use in various places
namespace FlagRTS
{
	// Arbitrary objects handles for use by factories etc.
	// Should define object uniquely ( at least within context it can be used )
	// Sorted with type first
	struct ObjectHandle
	{
	public:
		size_t Type;
		size_t Object;

		ObjectHandle() :
			Type(0),
			Object(0)
		{ }

		ObjectHandle(const size_t type, const size_t object) :
			Type(type),
			Object(object)
		{ }

		bool operator==(const ObjectHandle& other) const
		{
			return Type == other.Type && Object == other.Object;
		}
		
		bool operator!=(const ObjectHandle& other) const
		{
			return Type == other.Type && Object == other.Object;
		}

		bool operator>(const ObjectHandle& other) const
		{
			return Type == other.Type ?
				Object > other.Object : Type > other.Type;
		}

		bool operator<(const ObjectHandle& other) const
		{
			return Type == other.Type ?
				Object < other.Object : Type < other.Type;
		}

		bool operator>=(const ObjectHandle& other) const
		{
			return Type == other.Type ?
				Object >= other.Object : Type >= other.Type;
		}

		bool operator<=(const ObjectHandle& other) const
		{
			return Type == other.Type ?
				Object <= other.Object : Type <= other.Type;
		}
	};
}