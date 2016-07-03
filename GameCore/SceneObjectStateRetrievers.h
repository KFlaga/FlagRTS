#pragma once

#include "IObjectRetriever.h"
#include "SceneObjectState.h"

namespace FlagRTS
{
	class SceneObjectStateFixedRetriever : public IObjectRetriever<size_t>
	{
		size_t _value;
	public:
		SceneObjectStateFixedRetriever(size_t value) :
			_value(value)
		{ }

		operator size_t() const
		{
			return _value;
		}
		
		IObjectRetriever<size_t>* GetCopy() 
		{ 
			return xNew1(SceneObjectStateFixedRetriever, _value); 
		}
	};
	
	class SceneObjectStateXmlRetriever : public IObjectRetriever<size_t>
	{
		size_t _value;
	public:
		SceneObjectStateXmlRetriever(XmlNode* valueNode)
		{ 
			_value = SceneObjectStates::ParseStateType(
				XmlUtility::XmlGetString(valueNode, "value"));
		}
		
		operator size_t() const
		{
			return _value;
		}
		
		IObjectRetriever<size_t>* GetCopy() 
		{ 
			return xNew1(SceneObjectStateFixedRetriever, _value); 
		}
	};
	
	class SceneObjectStateRetrieverFactory
	{
	public:
		static IObjectRetriever<size_t>* Create(XmlNode* valueNode);
	};
}