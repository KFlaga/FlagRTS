#pragma once

#include "IAction.h"
#include <map>

namespace FlagRTS
{
	class SceneObjectFamilyActionFactory : public IActionFactory
	{
	private:
		std::map<string, IActionFactory*> _factories;

	public:
		SceneObjectFamilyActionFactory();
		~SceneObjectFamilyActionFactory();

		IAction* Create(XmlNode* actionNode);
	};
}