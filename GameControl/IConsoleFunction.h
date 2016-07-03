#pragma once

#include <DataTypes.h>
#include <Array.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	class Console;
	class IConsoleParameter;

	// Generic console function
	// Accepts console query ( starting at index after function name ) as input and process it
	class IConsoleFunction
	{
	protected:
		ArrayMaps<IConsoleParameter*>::KeyString _localParams;
		Console* _console;
		string _lastError;

	public:
		IConsoleFunction(Console* console);

		virtual ~IConsoleFunction();

		// Process given query starting at start index ( index after function name )
		// Typicaly will look for paramter/s name in query then look for paramter in
		// global (in console)/local (in this function) map of named paramters and 
		// optionaly value of this paramter and perform some action using it all
		// Returns true if parsing of params and action were successful
		// Default version checks if there's "help" in query[startIndex] and if there is
		// prints help and returns true. Otherwise do nothing and returns false
		virtual bool ResolveQuery(Array<string>& query, uint32 startIndex);

		// Returns paramter from local map or if not found, from global console map
		virtual IConsoleParameter* FindParameter(const string& name);

		// Returns last error message ( set after failure of ResolveQuery() )
		const string& GetError() const
		{
			return _lastError;
		}

		ArrayMaps<IConsoleParameter*>::KeyString& GetLocalParamsMap()
		{
			return _localParams;
		}

		// Returns help string -> list of all possible usages of this function
		virtual string GetHelp()
		{
			return "Help not available";
		}

		// Returns list of local paramters
		virtual string GetParamList();
	};
}