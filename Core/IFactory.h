#pragma once

#include "TypeDefs.h"
#include "IGameObject.h"

namespace FlagRTS
{
	// Interface for generic item factory
	// Creates GameObjects via call to Create() with any paramters
	// ( when multiple parameters are to be passes, pass as dedicated struct)
	template<typename CreationParameter>
	class IFactory
	{
	private:
		string _returnTypeName;

	public:
		IFactory(const string& returnTypeName) :
			_returnTypeName(returnTypeName)
		{ }
		virtual ~IFactory() { }

		const string& GetFactoryType() const
		{
			return _returnTypeName;
		}

		// Allocs memory and creates object
		virtual IGameObject* Create(CreationParameter parameter) = 0;
	};

	// Templated class for Factory, which allows for creation of items with
	// casting, also specifies expected construction params in template
	template<typename ReturnType , typename CreationParameter>
	class IFactoryCast : public IFactory<CreationParameter>
	{
	public:
		IFactoryCast(const string& returnTypeName) :
			IFactory(returnTypeName)
		{ }

		ReturnType* CreateCast(CreationParameter parameters...)
		{
			return static_cast<ReturnType*>(Create(parameters));
		}
	};

	// Generally factories of subtypes will work that way:
	// the most specialized factory creates the final object but
	// sets only most specialized properties, leaving lower level
	// factory to set base class parameters
}

#include "Memory.h"

#define DEFINE_FINAL_FACTORY(FactoryType, ObjType, ObjTypeName, Parameter) \
	class FactoryType : public IFactoryCast<ObjType, Parameter> \
	{ \
	public: \
		FactoryType() : IFactoryCast(ObjTypeName) { } \
		IGameObject* Create(Parameter parameter) { \
			return xNew1(ObjType, parameter); } \
	};
