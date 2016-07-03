#pragma once

#include "ConfigurationManager.h"
#include "ErrorCodes.h"
#include "LogManager.h"


namespace FlagRTS
{
	class LocalizationManager : public GenericManager<LocalizationManager>
	{
	private:
		RefPtr<Configuration> Texts;
		List<string> _availableLanguages;

	protected:
		LocalizationManager();
		friend GenericManager<LocalizationManager>;

	public:
		string GetText(const string& name, const string& section = "")
		{
			Option* opt = Texts->FindOption(name, section);
			if(opt != 0)
				return opt->Value;
			else
			{
				return name + " - entry not found";
				ADD_LOG(string("Couldn't find loc text: ") + name + ", in section: " + section);
			}
		}

		void SetLanguauge(const string& name)
		{
			Texts = ConfigurationManager::Instance()->GetConfiguration(
				string("LNG") + name);
			ADD_LOG(string("Setting current language = ") + name);
		}

		void LoadLanguages();

		List<string>& GetAvailableLanguages()
		{
			return _availableLanguages;
		}
	};
}

#define GET_TEXTN(name) LocalizationManager::Instance()->GetText(name)
#define GET_TEXT(name,section) LocalizationManager::Instance()->GetText(name,section)
#define GET_TEXT_XML(node) LocalizationManager::Instance()->GetText( \
	XmlUtility::XmlGetString(node, "loc", 3), \
	XmlUtility::XmlGetString(node, "sec", 3))