#include "LocalizationManager.h"

namespace FlagRTS
{
	LocalizationManager* LocalizationManager::_instance = 0;

	LocalizationManager::LocalizationManager()
	{

	}

	void LocalizationManager::LoadLanguages()
	{
		ADD_LOG("Loading languages");
		RefPtr<Configuration> mainConf = ConfigurationManager::Instance()->GetConfiguration("Main");
		// Find location of labguauges config file
		string langFile = mainConf->FindOptionValue("Languages","Directories") + "Languages.cfg";
		// Lang conf contains available languauges, choosen one and path to text files
		RefPtr<Configuration> langConf = ConfigurationManager::Instance()->GetConfiguration("Languages");
		langConf->Load(langFile.c_str());
		// Add all available languages
		SectionIterator langIt;
		SectionIterator sectionEnd;
		if( langConf->Begin("AvailableLanguages", langIt) == ErrorCodes::Success)
		{
			_availableLanguages.clear();
			langConf->End("AvailableLanguages", sectionEnd);
			for(;langIt != sectionEnd; langIt++)
			{
				ADD_LOG(string("Adding languauge : ") + (*langIt).Value->Name);
				RefPtr<Configuration> lng = ConfigurationManager::Instance()->GetConfiguration(
					string("LNG") + (*langIt).Value->Name);
				lng->Load((*langIt).Value->Value.c_str());
				_availableLanguages.push_back((*langIt).Value->Name);
			}
		}

		// Select selected one or "English" if not found
		string curLng = langConf->FindOptionValue("Selected");
		if(curLng.size() > 0)
		{
			SetLanguauge(curLng);
		}
		else
		{
			SetLanguauge("English");
		}
		ADD_LOG("Languauges loaded");
	}
}