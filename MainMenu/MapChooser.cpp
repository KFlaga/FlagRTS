#include "MapChooser.h"
#include <MyGUI\MyGUI_ListBox.h>
#include <Xml.h>
#include <Windows.h>

namespace FlagRTS
{
	MapChooser::MapChooser(MyGUI::ListBox* listBox, 
		const string& initalDir) :
	_listBox(listBox),
		_pendingChangeDirectory(false),
		_pendingMapChoosen(false)
	{
		_listBox->setVisible(false);
		_fileBrowser.SetSort(true);
		int dirSize = GetCurrentDirectory(0, 0);
		char* dirBuff = new char[dirSize+1];
		GetCurrentDirectory(dirSize+1, dirBuff);
		_exePath = dirBuff;
		delete dirBuff;

		ChangeCurrentDirectory(initalDir);

		_listBox->eventListMouseItemActivate += MyGUI::newDelegate(this, 
			&MapChooser::ItemSelected);
	}

	void MapChooser::Update(float ms)
	{
		if( _pendingChangeDirectory )
		{
			_pendingChangeDirectory = false;
			ChangeCurrentDirectory(_newDir, false);
		}
		else if( _pendingMapChoosen )
		{
			_pendingMapChoosen = false;
			_mapSelected.Fire(_maps[_newMapIdx]);
		}
	}

	void MapChooser::Show()
	{
		_listBox->setVisible(true);
	}

	void MapChooser::Hide()
	{
		_listBox->setVisible(false);
	}

	bool MapChooser::IsVisible() const
	{
		return _listBox->getVisible();
	}

	bool MapChooser::IsUpLevelAvailable()
	{
		return _currentDir.size() > 2;
	}

	void MapChooser::UpOneLevel()
	{
		// Find if dir ends with \ / and omit it
		string tempDir = RemoveLastDash(_currentDir);

		// Find last \ / in dir
		int pos = tempDir.size();
		for(unsigned int i = tempDir.size()-1; i > 2; --i)
		{
			if( tempDir[i] == '\\' || tempDir[i] == '/' )
			{
				pos = i;
				break;
			}
		}

		if( pos != tempDir.size() )
		{
			tempDir = tempDir.substr(0, pos);
			ChangeCurrentDirectory(tempDir, false);
		}
	}

	void MapChooser::ChangeCurrentDirectory(const string& dir, bool isRelative)
	{
		string oldDir = _currentDir;
		if(isRelative)
		{
			_currentDir = ChangeDirPathToAbsolute(dir);
		}
		_currentDir = EnsureEndsWithDash(dir);

		_fileBrowser.SetDirectory(_currentDir);
		if( _fileBrowser.UpdateFiles() == false)
		{
			_currentDir = oldDir;
			_fileBrowser.SetDirectory(_currentDir);
			return;
		}

		_listBox->removeAllItems();
		_maps.clear();

		auto& folders = _fileBrowser.GetAllFolders();
		// Add all folders first, starting with '...'
		if( IsUpLevelAvailable() )
		{
			_listBox->addItem("...", (int)0);
		}

		unsigned int folderNum = 0;
		for(; folderNum < folders.size(); ++folderNum)
		{
			_listBox->addItem(string("\\") + folders[folderNum], (int)(folderNum+1));
		}

		_mapsFirstIndex = folderNum + 1;
		auto& mapFiles = _fileBrowser.GetFilesWithExtension("map");
		// Parse all maps headers -> for each map open xml file
		for(unsigned int mapNum = 0; mapNum < mapFiles.size(); ++mapNum)
		{
			auto mapDoc = XmlUtility::XmlDocFromFile(
				(_currentDir + string("\\") + mapFiles[mapNum]).c_str());

			if(mapDoc.IsValid())
			{
				try
				{
					MapBaseInfo mapInfo;
					mapInfo.MapDoc = mapDoc;
					XmlNode* mapNode = mapDoc->first_node("Map");
					mapInfo.MapDirName = XmlUtility::XmlGetString(mapNode->first_node("MapDataDir"), "value");
					mapInfo.MapName = XmlUtility::XmlGetString(mapNode->first_node("MapName"), "value");
					mapInfo.Players = XmlUtility::XmlGetString(mapNode->first_node("Players"), "value");
					mapInfo.Mode = XmlUtility::XmlGetString(mapNode->first_node("DefaultMode"), "name");
					mapInfo.Size = XmlUtility::XmlGetString(mapNode->first_node("Size"), "value");

					_maps.push_back(mapInfo);
				}
				catch(...)
				{ }
			}
		}

		// Add all parsed maps to listbox
		for(unsigned int mapNum = 0; mapNum < _maps.size(); ++mapNum)
		{
			MapBaseInfo& mapInfo = _maps[mapNum];
			// Full shown map name consists of map name, size, players, mode
			// TODO create some gridbox for it
			_listBox->addItem(mapInfo.MapName + " (Size:" + mapInfo.Size + 
				" Players:" + mapInfo.Players + " Mode:" + mapInfo.Mode + ")", 
				(int)(_mapsFirstIndex + mapNum));
		}
	}

	string MapChooser::ChangeDirPathToAbsolute(const string& dir)
	{
		// Convert relative to absolute path :
		// Find amount of '../' and go up this number of levels from exe dir
		// Then go down rest of folders
		string tempDir = _exePath;
		int lastDot = 0;
		for(unsigned int i = 0; i < dir.size()-1; ++i)
		{
			if(dir[i] == '.' && dir[i+1] == '.')
			{
				// Found '..' -> go up one level with tempDir : find pos of last '\' or '/' and remove to this point
				int pos = tempDir.size();
				for(unsigned int k = tempDir.size()-1; k > 1; --k)
				{
					if( tempDir[k] == '\\' || tempDir[k] == '/' )
					{
						pos = k;
						break;
					}
				}
				if( pos != tempDir.size() )
				{
					tempDir = tempDir.substr(0, pos);
				}
				++i;
				lastDot = i;
			}
		}
		// Append rest of directory to tempDir
		tempDir.append(dir.substr(lastDot+1));
		return tempDir;
	}

	void MapChooser::ItemSelected(MyGUI::ListBox* listBox, size_t index)
	{
		if(index > _mapsFirstIndex + _maps.size())
		{
			// Invalid index
			return;
		}

		auto fileIndex = *_listBox->getItemDataAt<int>(index);
		if( IsUpOneLevelSelected(fileIndex) )
		{
			UpOneLevel();
		}
		else if( IsFolderSelected(fileIndex) )
		{
			_newDir = EnsureEndsWithDash(_currentDir) +
				_fileBrowser.GetAllFolders()[fileIndex-1] + "\\";
			_pendingChangeDirectory = true;
		}
		else
		{
			// Map was choosen
			_newMapIdx = fileIndex - _mapsFirstIndex;
			_pendingMapChoosen = true;
		}
	}

	bool MapChooser::IsUpOneLevelSelected(size_t index) const
	{
		return index == 0;
	}

	bool MapChooser::IsFolderSelected(size_t index) const
	{
		return index < (size_t)_mapsFirstIndex;
	}

	bool MapChooser::IsMapSelected(size_t index) const
	{
		return index >= (size_t)_mapsFirstIndex;
	}

	string MapChooser::EnsureEndsWithDash(const string& str)
	{
		string tempStr = str;
		if(str[str.size()-1] != '\\' && str[str.size()-1] != '/')
			tempStr.append("\\");
		return tempStr;
	}

	string MapChooser::RemoveLastDash(const string& str)
	{
		if(str[str.size()-1] == '\\' || str[str.size()-1] == '/')
			return str.substr(0, str.size()-1);
		return str;
	}
}