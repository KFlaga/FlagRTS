#pragma once

#include <Array.h>
#include <DataTypes.h>
#include <ArrayMap.h>

namespace FlagRTS
{
	typedef Array<string> ConsoleQuery;
	class IConsoleParameter;
	class IConsoleFunction;

	class IConsolePrinter
	{
	public:
		// Prints text block
		virtual void Print(const string&) = 0;
	};

	// Console serves as string query resolver - parses array of strings and takes proper action
	// Each string in query may contain -> function name / parameter name / parameter value
	// 1st must be function name :
	// look for function handler in map
	// Function is of type void(*)(const string& query, int startIndex) and it resolves rest of query
	// start index is index after function name
	// Second is probably parameter name
	// paramters are also mapped -> mapped are handlers :
	// IParameter(*)(string&)
	// Function loads paramters and optional values and acts on them
	// Each function define funcionality with IParameter
	class Console
	{
	public:
		// Global instance of console -> for easy addition of additional functions
		static Console* GlobalConsole;

	private:
		ArrayMaps<IConsoleParameter*>::KeyString _globalParams;
		ArrayMaps<IConsoleFunction*>::KeyString _globalFunctions;
		IConsolePrinter* _printer;

	public:
		Console();
		~Console();

		void SetConsolePrinter(IConsolePrinter* consolePrinter);

		void AddParameter(const string& name, IConsoleParameter* param);
		void AddFunction(const string& name, IConsoleFunction* func);

		// Parses given query and executes proper IConsoleFunction
		void ResolveQuery(ConsoleQuery& query);

		virtual IConsoleParameter* FindParameter(const string& name)
		{
			auto paramIt = _globalParams.find(name);
			if(paramIt != _globalParams.end())
			{
				return paramIt->Value;
			}
			return 0;
		}
		
		void Print(const string& msg)
		{
			_printer->Print(msg);
		}
	};
}