#include "CursorImageWin32.h"
#include <MyGUI\MyGUI_DataManager.h>
#include <Windows.h>

namespace FlagRTS
{
	CursorImageWin32::CursorImageWin32() :
		_handle(0)
	{
	}

	void CursorImageWin32::deserialization(MyGUI::xml::ElementPtr _node, MyGUI::Version _version)
	{
		Base::deserialization(_node, _version);

		MyGUI::xml::ElementEnumerator info = _node->getElementEnumerator();
		while (info.next())
		{
			if (info->getName() == "Property")
			{
				const std::string& key = info->findAttribute("key");

				if (key == "SourceFile")
				{
					std::string path = MyGUI::DataManager::getInstance().getDataPath(info->getContent());
					_handle = (size_t)LoadCursorFromFileA(path.c_str());
				}
				else if (key == "SourceSystem")
				{
					std::string value = info->getContent();
					if (value == "IDC_ARROW")
						_handle = (size_t)::LoadCursor(NULL, IDC_ARROW);
					else if (value == "IDC_IBEAM")
						_handle = (size_t)::LoadCursor(NULL, IDC_IBEAM);
					else if (value == "IDC_WAIT")
						_handle = (size_t)::LoadCursor(NULL, IDC_WAIT);
					else if (value == "IDC_CROSS")
						_handle = (size_t)::LoadCursor(NULL, IDC_CROSS);
					else if (value == "IDC_UPARROW")
						_handle = (size_t)::LoadCursor(NULL, IDC_UPARROW);
					else if (value == "IDC_SIZE")
						_handle = (size_t)::LoadCursor(NULL, IDC_SIZE);
					else if (value == "IDC_ICON")
						_handle = (size_t)::LoadCursor(NULL, IDC_ICON);
					else if (value == "IDC_SIZENWSE")
						_handle = (size_t)::LoadCursor(NULL, IDC_SIZENWSE);
					else if (value == "IDC_SIZENESW")
						_handle = (size_t)::LoadCursor(NULL, IDC_SIZENESW);
					else if (value == "IDC_SIZEWE")
						_handle = (size_t)::LoadCursor(NULL, IDC_SIZEWE);
					else if (value == "IDC_SIZENS")
						_handle = (size_t)::LoadCursor(NULL, IDC_SIZENS);
					else if (value == "IDC_SIZEALL")
						_handle = (size_t)::LoadCursor(NULL, IDC_SIZEALL);
					else if (value == "IDC_NO")
						_handle = (size_t)::LoadCursor(NULL, IDC_NO);
					else if (value == "IDC_HAND")
						_handle = (size_t)::LoadCursor(NULL, IDC_HAND);
					else if (value == "IDC_APPSTARTING")
						_handle = (size_t)::LoadCursor(NULL, IDC_APPSTARTING);
					else if (value == "IDC_HELP")
						_handle = (size_t)::LoadCursor(NULL, IDC_HELP);
				}
			}
		}
	}
}