#pragma once

#include <IGameObject.h>
#include <TypeId.h>
#include "DataTypes.h"
#include <ObjectHandle.h>
#include <Xml.h>

namespace FlagRTS
{
	class IKindSpecificDataSupplier;
	class IObjectSpecificDataSupplier;
	// Base class for all object defnitions
	// Contains only name of defined object and TypeId of
	// child class next in hierarchy
	// Inherits IGameObject, as itshould be created by factories
	class ObjectDefinition : public IGameObject
	{
	protected:
		string _name;
		string _finalTypeName;
		// Object data name defines which Kind/Object specific data should be used
		string _objectDataName;

		size_t _kindSpecificDataHandle;
		IObjectSpecificDataSupplier* _objectDataHandleSupplier;
		IKindSpecificDataSupplier* _kindDataHandleSupplier;

	public:
		ObjectDefinition();
		ObjectDefinition(XmlNode* defNode);
		virtual ~ObjectDefinition();

		void SetName(const string& name) { _name = name; }
		const string& GetName() const { return _name; }
		
		void SetFinalTypeName(const string& type) { _finalTypeName = type; }
		const string& GetFinalTypeName() const { return _finalTypeName; }

		const string& GetObjectDataName() const { return _objectDataName; }

		void SetKindDataHandleSupplier(IKindSpecificDataSupplier* supplier)
		{
			_kindDataHandleSupplier = supplier;
		}

		size_t GetKindSpecificDataHandle() const { return _kindSpecificDataHandle; }
		void SetKindSpecificDataHandle(size_t handle) { _kindSpecificDataHandle = handle; }

		template<typename DataType>
		DataType* GetKindSpecificData() const 
		{ 
			return reinterpret_cast<DataType*>(_kindSpecificDataHandle); 
		}

		void SetObjectDataHandleSupplier(IObjectSpecificDataSupplier* supplier)
		{
			_objectDataHandleSupplier = supplier;
		}

		size_t RequestObjectDataHandle() const;
		void DeleteObjectDataHandle(size_t handle) const;
	};
}