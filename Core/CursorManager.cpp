#include "CursorManager.h"
#include "ErrorManager.h"
#include "ConfigurationManager.h"
#include "LogManager.h"
#include "CursorImageWin32.h"
#include <Windows.h>
#include <OgreRenderWindow.h>
#include "Exception.h"
#include <fstream>

namespace FlagRTS
{
	CursorManager* CursorManager::_instance = 0;

	CursorManager::CursorManager() :
		_currentContext(0),
		_previousContext(0)
	{

	}

	CursorManager::~CursorManager()
	{
		MyGUI::FactoryManager::getInstance().unregisterFactory<CursorImageWin32>("Resource");
		MyGUI::FactoryManager::getInstance().unregisterFactory<CursorContext>("Resource");

		MyGUI::PointerManager& manager = MyGUI::PointerManager::getInstance();
		manager.eventChangeMousePointer -= MyGUI::newDelegate(this, &CursorManager::OnGuiCursorChanged);
	}

	void CursorManager::InitCursors(const string& dir)
	{
		ADD_LOG("Begin CursorManger Init");

		MyGUI::FactoryManager::getInstance().registerFactory<CursorImageWin32>("Resource");
		MyGUI::FactoryManager::getInstance().registerFactory<CursorContext>("Resource");
		AddCursors(dir);

		_currentCursor = MyGUI::PointerManager::getInstance().getDefaultPointer();
		MyGUI::PointerManager::getInstance().eventChangeMousePointer += 
			MyGUI::newDelegate(this, &CursorManager::OnGuiCursorChanged);
		MyGUI::PointerManager::getInstance().setVisible(false); // Hide mygui pointer - native one will be used

		// Set default cursor
		_cursorState = "Default";
		ChangeCursorContext("Cursor_Normal");
		SetVisible(true);

		ADD_LOG("End CursorManager Init");
	}

	void CursorManager::AddCursors(const string& dir)
	{
		Ogre::ResourceGroupManager::getSingleton().addResourceLocation(
			dir, "FileSystem", "MyGUI");

		// Open cursor files std streams and create xml docs from them
		std::ifstream cursorsFile;
		cursorsFile.open(dir + "Cursors.xml");
		std::ifstream contextsFile;
		contextsFile.open(dir + "CursorContexts.xml");

		if( cursorsFile.is_open() )
		{
			MyGUI::xml::Document cursorsDoc;
			cursorsDoc.open(cursorsFile);

			// Load cursors from xml
			MyGUI::ResourceManager::getInstance().loadFromXmlNode(cursorsDoc.getRoot(), "", 
				MyGUI::Version(MYGUI_VERSION_MAJOR, MYGUI_VERSION_MINOR, MYGUI_VERSION_PATCH));

			cursorsFile.close();
		}

		if( contextsFile.is_open() )
		{
			MyGUI::xml::Document contextsDoc;
			contextsDoc.open(contextsFile);

			MyGUI::ResourceManager::getInstance().loadFromXmlNode(contextsDoc.getRoot(), "", 
				MyGUI::Version(MYGUI_VERSION_MAJOR, MYGUI_VERSION_MINOR, MYGUI_VERSION_PATCH));

			contextsFile.close();
		}
	}

	void CursorManager::OnGuiCursorChanged(const string& name)
	{
		_currentCursor = name;
		SetCursor(name);
	}

	void CursorManager::SetVisible(bool _value)
	{
		// Use WinApi to show cursor
		::ShowCursor(_value);
	}

	void CursorManager::SetCursorHandle(size_t id)
	{
		// Add offset to id as to point to cursor memory in window
		SetClassLongPtr((HWND)_hWnd, GCLP_HCURSOR, (LONG_PTR)id);
		// Changes cursor of window with changed id
		::SetCursor((HCURSOR)id);
	}

	void CursorManager::SetCursor(const std::string& name)
	{
		// Try to find handle to cursor if it is already loaded
		auto iter = _cursorHandles.find(name);
		if (iter != _cursorHandles.end())
		{
			SetCursorHandle(iter->second);
		}
		else // If not create new cursor
		{
			MyGUI::IResource* resource_generic = MyGUI::ResourceManager::getInstance().getByName(name, false);
			if (resource_generic != nullptr)
			{
				CursorImageWin32* cursor = resource_generic->castType<CursorImageWin32>(false);
				if (cursor != nullptr)
				{
					_cursorHandles[name] = cursor->GetCursorHandle();
					SetCursorHandle(cursor->GetCursorHandle());
				}
				else
				{
					//CastException("Bad cast of mygui resource to cursor");
				}
			}
			else
			{
				//CastException("Bad cast of mygui resource to cursor");
			}
		}
	}

	void CursorManager::ChangeCursorContext(const string& name)
	{
		MyGUI::IResource* resource_generic = MyGUI::ResourceManager::getInstance().getByName(name, false);
		if (resource_generic != nullptr) // Check if such resource exists
		{
			CursorContext* cursor = resource_generic->castType<CursorContext>(false);
			if (cursor != nullptr) // Check if this resource is cursor
			{
				_previousContext = _currentContext;
				_currentContext = cursor;
			}
			else
			{
				CastException("Bad cast of mygui resource to cursor");
			}
		}
		else
		{
			CastException((string("Cursor context not found. Name: ") + name).c_str());
		}

		// UpdateCursor();
	}

	void CursorManager::UpdateCursor()
	{
		if(_currentContext == 0)
			return;

		SetCursor(_currentContext->GetCursorForState(_cursorState));
	}

	bool CursorManager::IsMouseInClient()
	{
		// Get top-left of game window relative to screen
		POINT point = { 0, 0 };
		ClientToScreen((HWND)_hWnd, &point);
		// Get size of game window
		RECT client_rect = { 0, 0, 0, 0 };
		GetClientRect((HWND)_hWnd, &client_rect);
		// Get position of cursor relative to screen
		POINT cursor_point = { 0, 0 };
		GetCursorPos(&cursor_point);
		// Check if cursor is within window
		bool hor = cursor_point.x >= point.x && cursor_point.x < (point.x + client_rect.right);
		bool ver = cursor_point.y >= point.y && cursor_point.y < (point.y + client_rect.bottom);

		return hor && ver;
	}
}
