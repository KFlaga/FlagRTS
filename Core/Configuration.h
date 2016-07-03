#pragma once

#include <cctype>
#include "List.h"
#include "KeyValMap.h"
#include "TypeDefs.h"

namespace FlagRTS
{
	struct Option
	{
		std::string Name;
		std::string Section;
		std::string Value;
	};

	// Inner list is list of options from single section
	typedef List<List<Option*>*> ConfigDict;
	typedef List<List<Option*>*>::iterator DictIterator; 
	typedef List<Option*> SectionOptions;
	typedef List<Option*>::iterator SectionIterator;

	class Configuration
	{
	private:
		ConfigDict _config;
		char _sep; // separator name-value
		string _path;

		// Configuration cannot be copied - always should be recived via config manager
		Configuration(const Configuration&);
		Configuration& operator=(const Configuration&);

	public:
		Configuration();

		// Adds option to configuration
		void AddOption(Option* opt);
		// Adds option to configuration
		void AddOption(const string& name, const string& value, const string& section);

		// Loads configuration from given filepath
		ErrorCode Load(const string& path);
		
		// Saves configuration from given filepath
		ErrorCode Save(const string& path);
		// Save configuration to file from it was loaded
		ErrorCode Save();

		// gets iterator to begin of specified section in config
		ErrorCode Begin(const std::string& section, SectionIterator&);
		
		// gets iterator to end of specified section in config
		ErrorCode End(const std::string& section, SectionIterator&);

		// gets iterator to begining of Dict
		DictIterator BeginDict() { return _config.begin(); }

		// gets iterator to ending of Dict
		DictIterator EndDict() { return _config.end(); }

		// Returns option value field with specifed name and section (or first with this name if section is empty)
		// Returns empty string if value not found
		std::string FindOptionValue(const std::string& name, const std::string& section = "");
		
		// Returns option poniter with specifed name and section (or first with this name if section is empty)
		// Returns 0 if option not found
		Option* FindOption(const std::string& name, const std::string& section = "");

		~Configuration()
		{
			ClearConfig();
		}

		// Deletes all options in config
		void ClearConfig();
	private:
		// Returns position of 1st nonspace character, begining from 'begin' ( return val is counted from line[0] )
		// or -1 if line is empty
		inline int Find1stNonSpace(char* line, unsigned int begin = 0)
		{
			for(unsigned int c = begin;; c++)
			{
				if(line[c] == '\0')
					return -1;
				if(!isspace(line[c]))
					return c;
			}
		}
		// Returns true if sign is comment sign
		inline bool CheckIsComment(char sign) 
		{ 
			return sign == '#';
		}
		// Retuns true if sign is section opener
		inline bool CheckIsSection(char sign)
		{ 
			return sign == '[';
		}
		// Returns section's name
		std::string ParseSection(char* line, unsigned int begin = 0);
		// Returns parsed option name, begin parsing from 'begin' position
		// In 'endPosition' stores  position of separator, if endPosition == -1, then no spearator found - invalid option
		std::string ParseOptName(char* line, unsigned int begin, int& endPosition);
		std::string ParseOptValue(char* line, unsigned int begin, int& endPosition);
	};
}