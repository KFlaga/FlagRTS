#include "Configuration.h"
#include <fstream>
#include "LogManager.h"
#include "ErrorManager.h"
#include "Memory.h"

#define CONFIG_MAX_LINE_LEN 1000

namespace FlagRTS
{
	Configuration::Configuration()
	{
		_sep = '=';
	}

	ErrorCode Configuration::Load(const string& filename)
	{
		ADD_LOG(string("Begin Loading Configuration : ") + filename );
		_path = filename;
		std::ifstream file;
		file.open(filename);

		if (!file.is_open())
		{
			return RETURN_ERROR(ErrorCodes::ConfigFileCoundntOpen);
		}

		ADD_LOG("Config file opened");

		ClearConfig();

		char line[CONFIG_MAX_LINE_LEN];
		Option curOption;
		while (!file.eof())
		{
			file.getline(line, CONFIG_MAX_LINE_LEN);
			int firstChar = Find1stNonSpace(line);
			if(firstChar < 0) // empty line
				continue;

			if (CheckIsComment(line[firstChar])) // comment -> omit
			{ }
			else if (CheckIsSection(line[firstChar])) // Section name
			{
				curOption.Section = ParseSection(line, firstChar);
			}
			else // Option name
			{
				int end;
				curOption.Name = ParseOptName(line, firstChar, end);

				if (end > 0) // Found a name, so start looking for value
				{
					curOption.Value = ParseOptValue(line, end+1, end);
					if(end > 0) // found value
					{
						Option* opt = xNew0(Option);
						opt->Name = curOption.Name;
						opt->Section = curOption.Section;
						opt->Value = curOption.Value;
						AddOption(opt);
					}

				}
			}
		}

		ADD_LOG(string("End Loading Config"));
		file.close();
		return ErrorCodes::Success;
	}

	void Configuration::AddOption(Option* opt)
	{
		// first check if section exists
		for(DictIterator it = _config.begin(); it != _config.end(); it++)
		{
			if( (*(*it).Value->begin()).Value->Section.compare(opt->Section) == 0 )
			{
				// section exists -> add option
				(*it).Value->push_back(opt);
				return;
			}
		}
		// Create new section
		SectionOptions* section = xNew0(SectionOptions);
		section->push_back(opt);
		_config.push_back(section);
	}

	void Configuration::AddOption(const string& name, const string& value, const string& section)
	{
		Option* opt = xNew0(Option);
		opt->Name = name;
		opt->Section = section;
		opt->Value = value;
		AddOption(opt);
	}

	std::string Configuration::ParseSection(char* line, unsigned int begin)
	{
		int secLenght = 0;
		for (int i = begin + 1;; i++)
		{
			if(line[i] =='\0' || line[i] == ']')
				break;
			secLenght++;
		}
		return std::string(&line[begin+1], secLenght);
	}

	std::string Configuration::ParseOptName(char* line, unsigned int begin, int& endPosition)
	{
		int keyLen = 0,  pos = begin;
		while (true) // End of key on one of separators or whitespace
		{
			if (line[pos] == '\0')
			{
				endPosition = -1;
			}

			if (line[pos] == _sep )
			{
				endPosition = pos;
				break;
			}
			keyLen++;
			pos++;
		}
		return std::string(&line[begin], keyLen);
	}

	std::string Configuration::ParseOptValue(char* line, unsigned int begin, int& endPosition)
	{
		// First trim white spaces -> value begins on first nonspace and ends with first space
		int valPos = begin;
		bool foundVal = true;
		while (true)
		{
			if (line[valPos] == '\0') //  key not found
			{
				foundVal = false;
				endPosition = -1;
				return "";
			}
			if(isspace(line[valPos]))
				valPos++;
			else
				break;
		}
		// Found value begin - now find end of value - last nonwhitespace
		int pos = valPos;
		int lastNonspacePos = valPos;
		while (true)
		{
			char c = line[pos];
			if (line[pos] == '\0')
			{
				break;
			}
			else if (!isspace(line[pos]))
			{
				lastNonspacePos = pos;
			}
			pos++;
		}

		endPosition = lastNonspacePos;
		return std::string(&line[valPos], lastNonspacePos - valPos + 1);
	}

	ErrorCode Configuration::Save(const string& filename)
	{
		ADD_LOG(string("Saving configuration to file = ") + filename);
		std::ofstream file;
		file.open(filename, std::ios::out | std::ios::trunc);
		if (!file.is_open())
		{
			return RETURN_ERROR(ErrorCodes::ConfigFileCoundntOpen);
		}

		// For each section : add section
		//	for each option : add key, add sep, add value
		for(auto sec = _config.begin(); sec != _config.end(); sec++)
		{
			file<<'['<<(*(*sec).Value->begin()).Value->Section<<']'<<std::endl;
			for(auto opt = (*sec).Value->begin(); opt != (*sec).Value->end(); opt++)
			{
				file<<(*opt).Value->Name<<_sep<<(*opt).Value->Value<<std::endl;
			}
			file<<std::endl;
		}

		file.close();
		return ErrorCodes::Success;
	}

	ErrorCode Configuration::Save()
	{
		return Save(_path);
	}

	ErrorCode  Configuration::Begin(const std::string& section, SectionIterator& sit)
	{
		for(DictIterator it = _config.begin(); it != _config.end(); it++)
		{
			if( (*(*it).Value->begin()).Value->Section.compare(section) == 0 )
			{
				sit = (*it).Value->begin();
				return ErrorCodes::Success;
			}
		}
		return RETURN_ERROR(ErrorCodes::ConfigSectionCoundntFind);
	}

	ErrorCode  Configuration::End(const std::string& section, SectionIterator& sit)
	{
		for(DictIterator it = _config.begin(); it != _config.end(); it++)
		{
			if( (*(*it).Value->begin()).Value->Section.compare(section) == 0 )
			{
				sit = (*it).Value->end();
				return ErrorCodes::Success;
			}
		}
		return RETURN_ERROR(ErrorCodes::ConfigSectionCoundntFind);
	}

	std::string  Configuration::FindOptionValue(const std::string& name, const std::string& section)
	{
		Option* opt = FindOption(name, section);
		if(opt != 0)
			return opt->Value;
		else
			return "";
	}

	Option*  Configuration::FindOption(const std::string& name, const std::string& section)
	{
		if(section.length() == 0)
		{
			for(DictIterator dit = _config.begin(); dit != _config.end(); dit++)
			{
				for(SectionIterator sit = (*dit).Value->begin(); sit != (*dit).Value->end(); sit++)
				{
					if( (*sit).Value->Name.compare(name) == 0 )
						return (*sit);
				}
			}
		}
		else
		{
			for(DictIterator dit = _config.begin(); dit != _config.end(); dit++)
			{
				if( (*(*dit).Value->begin()).Value->Section.compare(section) == 0 )
				{
					for(SectionIterator sit = (*dit).Value->begin(); sit != (*dit).Value->end(); sit++)
					{
						if( (*sit).Value->Name.compare(name) == 0 )
							return (*sit);
					}
				}
			}
		}
		return 0;
	}

	void Configuration::ClearConfig()
	{
		for(DictIterator dit = _config.begin(); dit != _config.end(); dit++)
		{
			for(SectionIterator sit = (*dit).Value->begin(); sit != (*dit).Value->end(); sit++)
			{
				xDelete(sit->Value);
			}
			xDelete(dit->Value);
		}
		_config.clear();
	}


}