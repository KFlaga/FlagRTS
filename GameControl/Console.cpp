#include "Console.h"
#include "NumericConsoleParamter.h"
#include "BooleanConsoleParamter.h"
#include "GeneralConsoleFunctions.h"

namespace FlagRTS
{
	Console* GlobalConsole = 0;

	Console::Console()
	{
		AddFunction("set", xNew1(SetParamValueConsoleFunction,this));
		AddFunction("print", xNew1(PrintParamValueConsoleFunction,this));
	}

	Console::~Console()
	{
		// Delete params/funcs
	}

	void Console::AddParameter(const string& name, IConsoleParameter* param)
	{
		_globalParams[name] = param;
	}

	void Console::AddFunction(const string& name, IConsoleFunction* func)
	{
		_globalFunctions[name] = func;
	}

	void Console::SetConsolePrinter(IConsolePrinter* consolePrinter)
	{
		_printer = consolePrinter;
	}

	void Console::ResolveQuery(ConsoleQuery& query)
	{
		// First get name of function
		if( query.size() == 0 )
			return;

		auto funIt = _globalFunctions.find(query[0]);
		if( funIt == _globalFunctions.end() )
		{
			Print("Bad function name :" + query[0]);
			return;
		}

		if( funIt->Value->ResolveQuery(query, 1) == false )
		{
			Print("Error during executing query: ");
			Print( funIt->Value->GetError() );
		}
		else
		{
			Print("Probably worked");
		}
	}
}