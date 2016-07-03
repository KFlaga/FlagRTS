#include "CursorContext.h"

namespace FlagRTS
{
	void CursorContext::deserialization(MyGUI::xml::ElementPtr xmlNode, MyGUI::Version version)
	{
		// Taken from MyGUI sample demo
		Base::deserialization(xmlNode, version);

		MyGUI::xml::ElementEnumerator info = xmlNode->getElementEnumerator();
		while (info.next())
		{
			if (info->getName() == "Property")
			{
				const std::string& key = info->findAttribute("key");

			}
			else if (info->getName() == "Map")
			{
				MyGUI::xml::ElementEnumerator item = info->getElementEnumerator();
				while (item.next("Item"))
				{
					_cursorsForStates[item->findAttribute("name")] = item->getContent();
				}
			}
		}
	}

	string CursorContext::GetCursorForState(const string& state)
	{
		MyGUI::MapString::iterator item = _cursorsForStates.find(state);
		if (item != _cursorsForStates.end())
			return item->second;
		return _cursorsForStates.find("Default")->second;
	}
}