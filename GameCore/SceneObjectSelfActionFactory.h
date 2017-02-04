#pragma once

#include "ISelfActionFactory.h"
#include "SceneObjectSelfAction.h"
#include <map>
#include <StringUtils.h>

namespace FlagRTS
{
	class SceneObjectSelfActionFactory : public ISelfActionFactory<XmlNode*, SceneObject*>
	{
	protected:
		typedef std::map<string,  ISelfActionFactory<XmlNode*, SceneObject*>*, StringLess> SubFactoryMap;
		SubFactoryMap _subFactories;

	public:
		SceneObjectSelfActionFactory();
		~SceneObjectSelfActionFactory();

		EventHandler<SceneObject*>* Create(XmlNode* actionNode);
	
		void AddFactory(const string& type, ISelfActionFactory<XmlNode*, SceneObject*>* factory)
		{
			_subFactories.insert(std::make_pair(type,factory));
		}
	};
}