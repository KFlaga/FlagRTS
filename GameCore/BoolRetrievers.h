#pragma once

#include "IObjectRetriever.h"

namespace FlagRTS
{
	class BoolFixedRetriever : public IObjectRetriever<bool>
	{
		bool _value;
	public:
		BoolFixedRetriever(bool value) :
			_value(value)
		{ }

		operator bool() const
		{
			return _value;
		}

		IObjectRetriever<bool>* GetCopy() { return xNew1(BoolFixedRetriever, _value); }
	};
	
	class BoolRetrieverFactory
	{
	public:
		static IObjectRetriever<bool>* Create(XmlNode* valueNode);
	};

	// Logical retrievers -> accepts 1 or 2 IBoolRetrievers and combine values :
	// AND, OR, NOT

	class BoolAndRetriever : public IObjectRetriever<bool>
	{
		IObjectRetriever<bool>* _val1;
		IObjectRetriever<bool>* _val2;
	public:
		BoolAndRetriever(IObjectRetriever<bool>* val1, IObjectRetriever<bool>* val2) :
			_val1(val1),
			_val2(val2)
		{ }
		~BoolAndRetriever();

		operator bool() const
		{
			return *_val1 && *_val2;
		}
		
		IObjectRetriever<bool>* GetCopy() { return xNew2(BoolAndRetriever, _val1->GetCopy(), _val2->GetCopy()); }
	};

	class BoolOrRetriever : public IObjectRetriever<bool>
	{
		IObjectRetriever<bool>* _val1;
		IObjectRetriever<bool>* _val2;
	public:
		BoolOrRetriever(IObjectRetriever<bool>* val1, IObjectRetriever<bool>* val2) :
			_val1(val1),
			_val2(val2)
		{ }
		~BoolOrRetriever();

		operator bool() const
		{
			return *_val1 || *_val2;
		}
		
		IObjectRetriever<bool>* GetCopy() { return xNew2(BoolOrRetriever, _val1->GetCopy(), _val2->GetCopy()); }
	};

	class BoolNotRetriever : public IObjectRetriever<bool>
	{
		IObjectRetriever<bool>* _val1;
	public:
		BoolNotRetriever(IObjectRetriever<bool>* val1) :
			_val1(val1)
		{ }
		~BoolNotRetriever();

		operator bool() const
		{
			return !(*_val1);
		}
		
		IObjectRetriever<bool>* GetCopy() { return xNew1(BoolNotRetriever, _val1->GetCopy()); }
	};

	class BoolVariableRetriever : public IObjectRetriever<bool>
	{
		string _varName;
	public:
		BoolVariableRetriever(const string& value) :
			_varName(value)
		{ }

		operator bool() const;
		
		IObjectRetriever<bool>* GetCopy() { return xNew1(BoolVariableRetriever, _varName); }
	};
}