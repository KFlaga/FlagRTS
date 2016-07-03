#include "GeneralConsoleFunctions.h"
#include "Console.h"
#include "IConsoleParameter.h"

namespace FlagRTS
{
	SetParamValueConsoleFunction::SetParamValueConsoleFunction(Console* console) :
		IConsoleFunction(console)
	{ }

	bool SetParamValueConsoleFunction::ResolveQuery(Array<string>& query, uint32 startIndex)
	{
		if( IConsoleFunction::ResolveQuery(query, startIndex) )
			return true;

		if(query.size() <= startIndex + 1)
		{
			_lastError = string("No required parameter name/value");
			return false;
		}

		// 1) Find paramter with typed name
		uint32 paramIndex = startIndex + 1;
		IConsoleParameter* param = FindParameter(query[startIndex])->GetParameter(query, &paramIndex);
		if( param == 0 )
		{
			_lastError = string("Bad parameter name ") + query[startIndex];
			return false;
		}

		// 2) Parse value for paramter
		size_t paramStorage = param->GetValueStorage();
		if( param->ParseParamValue(query[paramIndex], paramStorage) == false)
		{
			_lastError = string("Bad parameter value ") + query[paramIndex];
			return false;
		}

		// 3) Set parameter value
		param->SetParamValue(paramStorage);
		return true;
	}


	PrintParamValueConsoleFunction::PrintParamValueConsoleFunction(Console* console) :
		IConsoleFunction(console)
	{ }

	bool PrintParamValueConsoleFunction::ResolveQuery(Array<string>& query, uint32 startIndex)
	{
		if( IConsoleFunction::ResolveQuery(query, startIndex) )
			return true;

		if(query.size() <= startIndex)
		{
			_lastError = string("No required parameter name");
			return false;
		}

		// 1) Find paramter with typed name
		uint32 paramIndex = startIndex + 1;
		IConsoleParameter* param = FindParameter(query[startIndex])->GetParameter(query, &paramIndex);
		if( param == 0 )
		{
			_lastError = string("Bad parameter name ") + query[startIndex];
			return false;
		}

		_console->Print( param->ToString() );

		return true;
	}
}