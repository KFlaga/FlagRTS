#pragma once

#include "TypeDefs.h"
#include "Memory.h"

namespace FlagRTS
{
	inline char* CopyString(const string& str)
	{
		char* emptyCharPtr = sNewArray(char, str.size()+1); 
		std::memcpy(emptyCharPtr, str.c_str(), str.size()); 
		emptyCharPtr[str.size()] = '\0';
		return emptyCharPtr;
	}

	inline char* CopyChar(const char* str, const int len)
	{
		char* emptyCharPtr = sNewArray(char, len+1); 
		std::memcpy(emptyCharPtr, str, len); 
		emptyCharPtr[len] = '\0';
		return emptyCharPtr;
	}

	inline char* CopyChar(const char* str)
	{
		int len = 0; 
		while(str[len] != '\0') ++len; 
		char* emptyCharPtr = sNewArray(char, len+1); 
		std::memcpy(emptyCharPtr, str, len); 
		emptyCharPtr[len] = '\0';
		return emptyCharPtr;
	}
}