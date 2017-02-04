#pragma once

#include <string>
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

	struct StringLess
	{	
		bool operator()(const string& _Left, const string& _Right) const
		{
			return _Left.compare(_Right) < 0;
		}
	};

	struct StringIsEqual
	{
		bool operator()(const string& str1, const string& str2)
		{
			return str1.compare(str2) == 0;
		}

		bool operator()(const char* str1, const string& str2)
		{
			return str2.compare(str1) == 0;
		}

		bool operator()(const string& str1, const char* str2)
		{
			return str1.compare(str2) == 0;
		}

		bool operator()(const char* str1, const char* str2)
		{
			return std::strcmp(str1, str2) == 0;
		}
	};
}