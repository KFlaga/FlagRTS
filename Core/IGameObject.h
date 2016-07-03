#pragma once

#include "ObjectHandle.h"
#include "TypeId.h"

namespace FlagRTS
{
	// Base class for classes to be created in factories
	// Every game object have its unique id ( handle )
	class IGameObject
	{
	private:
		ObjectHandle _handle;
		TypeId _finalType;

	public:
		IGameObject();

		virtual ~IGameObject() { }

		ObjectHandle GetHandle() const { return _handle; }

		// Can change only 'Type' part of handle
		void SetHandleTypePart(size_t handle) { _handle.Type = handle; }

		void SetFinalType(TypeId type) { _finalType = type; }
		TypeId GetFinalType() const { return _finalType; }
	};
}