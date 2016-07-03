#pragma once

#include "ICondition.h"
#include <Array.h>
#include <Xml.h>
#include "Variable.h"
#include "IObjectRetriever.h"

namespace FlagRTS
{
	class GeneralFamilyConditionFactory : public IConditionFactory
	{
		std::map<string, IConditionFactory*> _factories;

	public:
		GeneralFamilyConditionFactory();
		~GeneralFamilyConditionFactory();

		ICondition* Create(XmlNode* condNode);
	};

	// Condition true if all of conditions are true
	class AndCondition : public ICondition
	{
	protected:
		Array<ICondition*> _conditions;

	public:
		~AndCondition();

		bool CheckCondition();

		AndCondition* GetCopy();

		void AddCondition(ICondition* cond)
		{
			_conditions.push_back(cond);
		}

		void RemoveCondition(ICondition* cond)
		{
			_conditions.remove(cond);
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};

	// Condition true if all any of conditions are true
	class OrCondition : public ICondition
	{
	protected:
		Array<ICondition*> _conditions;

	public:
		~OrCondition();

		bool CheckCondition();

		OrCondition* GetCopy();

		void AddCondition(ICondition* cond)
		{
			_conditions.push_back(cond);
		}

		void RemoveCondition(ICondition* cond)
		{
			_conditions.remove(cond);
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};

	// Condition true if set condition is false
	class NotCondition : public ICondition
	{
	protected:
		ICondition* _condition;

	public:
		~NotCondition();

		bool CheckCondition();

		NotCondition* GetCopy();

		void SetCondition(ICondition* cond)
		{
			_condition = cond;
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};

	// Condition true if variables are equal
	class EqualsCondition : 
		public IParametrizedCondition<IObjectRetriever<Variable>*, IObjectRetriever<Variable>*>
	{
	protected:
		IObjectRetriever<Variable>* _var1;
		IObjectRetriever<Variable>* _var2;

	public:
		~EqualsCondition();

		bool CheckCondition();

		EqualsCondition* GetCopy();

		void SetParameters(IObjectRetriever<Variable>* var1, 
			IObjectRetriever<Variable>* var2)
		{
			_var1 = var1;
			_var2 = var2;
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};

	// Condition true if variables are not equal
	class NotEqualsCondition : 
		public IParametrizedCondition<IObjectRetriever<Variable>*, IObjectRetriever<Variable>*>
	{
	protected:
		IObjectRetriever<Variable>* _var1;
		IObjectRetriever<Variable>* _var2;

	public:
		~NotEqualsCondition();

		bool CheckCondition();

		NotEqualsCondition* GetCopy();

		void SetParameters(IObjectRetriever<Variable>* var1, 
			IObjectRetriever<Variable>* var2)
		{
			_var1 = var1;
			_var2 = var2;
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};

	// Condition true if first variable is greater than second one
	class GreaterCondition : 
		public IParametrizedCondition<IObjectRetriever<Variable>*, IObjectRetriever<Variable>*>
	{
	protected:
		IObjectRetriever<Variable>* _var1;
		IObjectRetriever<Variable>* _var2;

	public:
		~GreaterCondition();

		bool CheckCondition();

		GreaterCondition* GetCopy();

		void SetParameters(IObjectRetriever<Variable>* var1, 
			IObjectRetriever<Variable>* var2)
		{
			_var1 = var1;
			_var2 = var2;
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};

	// Condition true if first variable is lesser than second one
	class LesserCondition : 
		public IParametrizedCondition<IObjectRetriever<Variable>*, IObjectRetriever<Variable>*>
	{
	protected:
		IObjectRetriever<Variable>* _var1;
		IObjectRetriever<Variable>* _var2;

	public:
		~LesserCondition();

		bool CheckCondition();

		LesserCondition* GetCopy();

		void SetParameters(IObjectRetriever<Variable>* var1, 
			IObjectRetriever<Variable>* var2)
		{
			_var1 = var1;
			_var2 = var2;
		}

		class Factory : public IConditionFactory
		{
		public:
			ICondition* Create(XmlNode* condNode);
		};
	};
}