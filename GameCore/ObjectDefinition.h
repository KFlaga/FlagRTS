#pragma once

#include "DataTypes.h"
#include <Xml.h>
#include <TypeId.h>
#include <ObjectHandle.h>

namespace FlagRTS
{
	// Base class for all object defnitions
	class ObjectDefinition
	{
	protected:
		Array<ObjectDefinition*> _componentDefinitions;
		ObjectHandle _handle;
		string _name;
		string _finalTypeName;

	public:
		ObjectDefinition();
		ObjectDefinition(XmlNode* defNode);
		virtual ~ObjectDefinition();

		void SetName(const string& name) { _name = name; }
		const string& GetName() const { return _name; }
		
		void SetFinalTypeName(const string& type) { _finalTypeName = type; }
		const string& GetFinalTypeName() const { return _finalTypeName; }
		
		ObjectHandle GetHandle() const { return _handle; }

		void SetFinalType(TypeId type) { _handle.Type; }
		TypeId GetFinalType() const { return _handle.Type; }

		Array<ObjectDefinition*>& GetComponentDefinitions() { return _componentDefinitions; }
		const Array<ObjectDefinition*>& GetComponentDefinitions() const { return _componentDefinitions; }
	};
}