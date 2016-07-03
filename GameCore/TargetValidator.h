#pragma once

#include "PhysicalObject.h"
#include <list>
#include <map>

namespace ORTS
{
	// Helper class to validate if passed object is target of type
	// Perform series of checks 
	class HoverTargetValidator
	{
	public:
		typedef bool(*CheckFun)(RefPtr<PhysicalObject>&);
		typedef std::list<std::pair<bool,CheckFun>> CheckList;
		typedef std::map<string, CheckFun, string_less> CheckMap;

	protected:
		// Map with checks for every flag for easy parsing
		static CheckMap* _flagChecksMap;
		// List of checks to be check with expected value in pair->first
		CheckList _checks;

	public:
		HoverTargetValidator();
		HoverTargetValidator(XmlNode* targetNode);

		void AddCheck(bool mustBeTrue, CheckFun check) 
		{
			_checks.push_back(std::make_pair(mustBeTrue,check)); 
		}

		bool CheckTargetType(RefPtr<PhysicalObject>& object)
		{
			// Just check all checks
			for(auto checkIt = _checks.begin(); checkIt != _checks.end(); ++checkIt)
				if((checkIt->second)(object) == checkIt->first)
					return false;
			return true;
		}

		static void InitCheckMap();
		static void DeleteCheckMap();
	};
}