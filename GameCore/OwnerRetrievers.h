#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class OwnerRetrieverFactory
	{
	public:
		static IObjectRetriever<int>* Create(XmlNode* vectorNode);
	};

	class OwnerFixedRetriever : public IObjectRetriever<int>
	{
	private:
		int _owner;

	public:
		OwnerFixedRetriever(int owner) :
			_owner(owner)
		{ }

		operator int() const
		{
			return _owner;
		}
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(OwnerFixedRetriever, _owner); 
		}
	};

	class OwnerXmlNumberRetriever : public IObjectRetriever<int>
	{
	private:
		int _owner;

	public:
		OwnerXmlNumberRetriever(XmlNode* ownerNode)
		{ 
			_owner = XmlUtility::XmlGetInt(ownerNode, "value");
		}

		operator int() const
		{
			return _owner;
		}
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(OwnerFixedRetriever, _owner); 
		}
	};

	class OwnerVariableRetriever : public IObjectRetriever<int>
	{
		string _varName;
	public:
		OwnerVariableRetriever(const string& value) :
			_varName(value)
		{ }

		operator int() const;
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew1(OwnerVariableRetriever, _varName); 
		}
	};

	class OwnerClientPlayerRetriever : public IObjectRetriever<int>
	{
	public:
		OwnerClientPlayerRetriever()
		{ }

		operator int() const;
		
		IObjectRetriever<int>* GetCopy() 
		{ 
			return xNew0(OwnerClientPlayerRetriever); 
		}
	};

	// OwnerOfUnit
}