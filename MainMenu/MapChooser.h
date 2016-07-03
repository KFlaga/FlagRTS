#pragma once

#include <FileBrowser.h>
#include <Xml.h>

namespace MyGUI
{
	class ListBox;
}

namespace FlagRTS
{
	struct MapBaseInfo
	{
		RefPtr<XmlDocument> MapDoc;
		string MapName;
		string Size;
		string Players;
		string Mode;
	};

	// Looks for map files in current directory, parses its headers and
	// stores its base info which is shown on map list ( name / size / players / is_custom_mode )
	// Uses MyGUI listbox for showing maps / input
	class MapChooser
	{
		FileBrowser _fileBrowser;
		string _currentDir;
		string _exePath;
		Array<MapBaseInfo> _maps; // Maps in current dir

		Event<RefPtr<XmlDocument>&> _mapSelected;

		MyGUI::ListBox* _listBox;
		
		int _mapsFirstIndex;
		
		string _newDir;
		RefPtr<XmlDocument> _newMapDoc;
		bool _pendingChangeDirectory;
		bool _pendingMapChoosen;

	public:
		MapChooser(MyGUI::ListBox* listBox, const string& initalDir);

		void Update(float ms);

		void Show();
		void Hide();
		bool IsVisible() const;

		// Changes current dir and parses new maps
		// isRelative should be true is path is relative to .exe
		void ChangeCurrentDirectory(const string& dir, bool isRelative = true);

		const string& GetCurrentDir() const { return _currentDir; }

		// Fired when user selects map for list - map header passed as param
		Event<RefPtr<XmlDocument>&>& MapSelected() { return _mapSelected; }

	private:
		// Returns false if its disc-level directory ( for now do not allow change of disc )
		bool IsUpLevelAvailable();
		// Sets current dir as previous folder
		void UpOneLevel();

		void ItemSelected(MyGUI::ListBox* listBox, size_t index);

		bool IsUpOneLevelSelected(size_t index) const;
		bool IsFolderSelected(size_t index) const;
		bool IsMapSelected(size_t index) const;

		string EnsureEndsWithDash(const string& str);
		string RemoveLastDash(const string& str);
	};
}