#include "ConfigurationManager.h"
#include "Memory.h"

namespace FlagRTS
{
	ConfigurationManager* ConfigurationManager::_instance = 0;

	ConfigurationManager::ConfigurationManager()
	{

	}

	RefPtr<Configuration> ConfigurationManager::GetConfiguration(const std::string& name)
	{
		if( _configs.count(name) > 0)
		{
			// config is in base
			return _configs.find(name)->second;
		}
		else
		{
			RefPtr<Configuration> conf(xNew0(Configuration));
			_configs.insert(std::make_pair(name, conf));
			return conf;
		}
	}
}