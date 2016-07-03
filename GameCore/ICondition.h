#pragma once

#include "DataTypes.h"
#include <Xml.h>

namespace FlagRTS
{
	// Simple condition base class, used to add conditions
	// to triggers ( actions )
	__interface ICondition
	{
	public:
		virtual bool CheckCondition() = 0;
		virtual ICondition* GetCopy() = 0;
	};

	__interface IConditionFactory
	{
	public:
		virtual ICondition* Create(XmlNode* condNode);
	};

	template<typename... Args>
	__interface IParametrizedCondition : public ICondition
	{
	public:
		virtual void SetParameters(Args... args) = 0;
	};
}