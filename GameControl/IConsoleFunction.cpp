#include "IConsoleFunction.h"
#include "Console.h"
#include "IConsoleParameter.h"

namespace FlagRTS
{
	IConsoleFunction::IConsoleFunction(Console* console) :
		_console(console)
	{

	}

	IConsoleFunction::~IConsoleFunction()
	{
		// Delete all parameters
	}

	bool IConsoleFunction::ResolveQuery(Array<string>& query, uint32 startIndex)
	{
		// Check if help is typed
		if(query.size() <= startIndex)
			return false;

		if(query[startIndex].compare("help") == 0 || query[startIndex].compare("Help") == 0)
		{
			_console->Print(GetHelp());
			return true;
		}

		return false;
	}

	IConsoleParameter* IConsoleFunction::FindParameter(const string& name)
	{
		auto paramIt = _localParams.find(name);
		if(paramIt != _localParams.end())
		{
			return paramIt->Value;
		}
		return _console->FindParameter(name);
	}

	string IConsoleFunction::GetParamList()
	{
		string out = "Function specific paramters: ";
		for(auto paramIt = _localParams.begin(); paramIt != _localParams.end(); ++paramIt)
		{
			out += " - ";
			out += paramIt->Value->GetParamName() + " : " + paramIt->Value->GetParamTypeName();
			out += "/r/n";
		}
		return out;
	}
}