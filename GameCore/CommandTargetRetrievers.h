#pragma once

#include "IObjectRetriever.h"
#include "Command.h"

namespace FlagRTS
{
	class CommandTargetFixedRetriever : public IObjectRetriever<CommandTarget*>
	{
		CommandTarget* _value;
	public:
		// Passed pointer is deleted in destructor
		CommandTargetFixedRetriever(CommandTarget* value) :
			_value(value)
		{ }

		~CommandTargetFixedRetriever();

		operator CommandTarget*() const
		{
			return _value;
		}
		
		IObjectRetriever<CommandTarget*>* GetCopy() 
		{ 
			return xNew1(CommandTargetFixedRetriever, xNew1(CommandTarget, *_value)); 
		}
	};
	
	class CommandTargetPointXmlRetriever : public IObjectRetriever<CommandTarget*>
	{
		CommandTarget* _value;
	public:
		CommandTargetPointXmlRetriever(XmlNode* valueNode);
		~CommandTargetPointXmlRetriever();
		
		operator CommandTarget*() const
		{
			return _value;
		}
		
		IObjectRetriever<CommandTarget*>* GetCopy() 
		{ 
			return xNew1(CommandTargetFixedRetriever, xNew1(CommandTarget, *_value)); 
		}
	};

	class CommandTargetUnitXmlRetriever : public IObjectRetriever<CommandTarget*>
	{
		CommandTarget* _value;
	public:
		CommandTargetUnitXmlRetriever(XmlNode* valueNode);
		~CommandTargetUnitXmlRetriever();
		
		operator CommandTarget*() const
		{
			return _value;
		}
		
		IObjectRetriever<CommandTarget*>* GetCopy() 
		{ 
			return xNew1(CommandTargetFixedRetriever, xNew1(CommandTarget, *_value)); 
		}
	};

	class CommandTargetPointRetriever : public IObjectRetriever<CommandTarget*>
	{
		CommandTarget* _value;
	public:
		CommandTargetPointRetriever(const Vector3& targetPos);
		~CommandTargetPointRetriever();
		
		operator CommandTarget*() const
		{
			return _value;
		}
		
		IObjectRetriever<CommandTarget*>* GetCopy() 
		{ 
			return xNew1(CommandTargetFixedRetriever, xNew1(CommandTarget, *_value)); 
		}
	};

	class CommandTargetUnitRetriever : public IObjectRetriever<CommandTarget*>
	{
		CommandTarget* _value;
	public:
		CommandTargetUnitRetriever(Unit* targetUnit);
		~CommandTargetUnitRetriever();
		
		operator CommandTarget*() const
		{
			return _value;
		}
		
		IObjectRetriever<CommandTarget*>* GetCopy() 
		{ 
			return xNew1(CommandTargetFixedRetriever, xNew1(CommandTarget, *_value)); 
		}
	};
	
	class CommandTargetRetrieverFactory
	{
	public:
		static IObjectRetriever<CommandTarget*>* Create(XmlNode* valueNode);
	};
}