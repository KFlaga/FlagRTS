#pragma once

#include "IConsoleFunction.h"

namespace FlagRTS
{
	// Function named "set" 
	// Reads paramter next from first index and value from second 
	// Sets given value to parameter
	class SetParamValueConsoleFunction : public IConsoleFunction
	{
	public:
		SetParamValueConsoleFunction(Console* console);

		bool ResolveQuery(Array<string>& query, uint32 startIndex);
	};

	// Function named "print" 
	// Reads paramter next from first index and prints it to console
	class PrintParamValueConsoleFunction : public IConsoleFunction
	{
	public:
		PrintParamValueConsoleFunction(Console* console);

		bool ResolveQuery(Array<string>& query, uint32 startIndex);
	};
}