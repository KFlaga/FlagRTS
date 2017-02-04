#pragma once

#include "GenericManager.h"
#include "TypeDefs.h"
#include "Pointers.h"
#include "CursorContext.h"
#include "ConfigurationManager.h"
#include <map>
#include <fstream>
#include "StringUtils.h"

namespace FlagRTS
{
	// Loads and finds cursor objects. Dispalys proper cursor
	class CursorManager : public GenericManager<CursorManager>
	{
	private:
		//typedef std::map<string, RefPtr<Cursor>, string_less> CursorMap;
		//CursorMap _cursors; // Map containing cursor for different contexts -> cursors are get from ResourceManager
		//typedef std::list<Cursor*> CursorQueue;
		//CursorQueue _cursorQueue;
		typedef std::map<string, size_t, StringLess> CursorHandleMap;
		CursorHandleMap _cursorHandles;
		CursorContext* _currentContext;
		CursorContext* _previousContext;
		size_t _currentCursorHandle;
		string _cursorState;
		string _currentCursor;
		size_t _hWnd;

	protected:
		CursorManager();
		friend GenericManager<CursorManager>;

	public:
		~CursorManager();

		// Creates MyGUI resource factory for cursors and 
		// loads cursors from files Cursors.xml / CursorContexts.xml in given dir
		void InitCursors(const string& dir);
		// Loads cursors from files Cursors.xml / CursorContexts.xml in given dir
		void AddCursors(const string& dir);

		// Changes current cursor state ( in fact actual cursor )
		void SetCurrentCursorState(const string& newStateName)
		{
			_cursorState = newStateName;
			UpdateCursor();
		}

		const string& GetCurrentCursorState() const
		{
			return _cursorState;
		}

		const string& GetCurrentCursorContext() const
		{
			return _currentCursor;
		}

		// Changes currently used CursorContext
		void ChangeCursorContext(const string& name);
		
		void PreviousContext();

		// Shows cursor from front of queue with state set by SetCurrentCursorState()
		void UpdateCursor();

		// Finds or adds to cursor map if not found Windows cursor and
		// Sets its handle as current cursor
		void SetCursor(const string& name);

		// Changes windows cursor for current window to one with given id
		void SetCursorHandle(size_t cursorId);

		// Checks if mouse pointer is within client window
		bool IsMouseInClient();

		// Shows/hides mouse cursor
		void SetVisible(bool visible);

		// Callback for mygui, called when cursor is to be changed
		void OnGuiCursorChanged(const string& name);
	};
}