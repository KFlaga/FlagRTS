#pragma once

#include "Configuration.h"
#include "GenericManager.h"
#include "Pointers.h"
#include "TypeDefs.h"

namespace FlagRTS
{
	typedef std::map<std::string, RefPtr<Configuration>, string_less> ConfigMap;

	class ConfigurationManager : public GenericManager<ConfigurationManager>
	{
		ConfigMap _configs;

	protected:
		ConfigurationManager();
		friend GenericManager<ConfigurationManager>;

	public:
		// Finds config if already created, otherwise creates new one
		RefPtr<Configuration> GetConfiguration(const std::string& name);
	};
}