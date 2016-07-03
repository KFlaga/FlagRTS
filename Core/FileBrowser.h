#pragma once

#include "TypeDefs.h"
#include "Event.h"
#include "Array.h"

namespace FlagRTS
{
	// Loads contents of folder with optional filter
	class FileBrowser
	{
		string _currentDir;
		Array<string> _files;
		Array<string> _dirs;
		Array<string> _lastFilter;
		bool _sortAlph;

	public:
		FileBrowser();

		// Load new files from set directory
		// Returns true on success
		bool UpdateFiles();

		// After setting new directory, UpdateFiles must be called
		void SetDirectory(const string& path)
		{
			_currentDir = path;
		}

		const string& GetDirectory() const { return _currentDir; }

		// Returns names of all files in directory ( with ext )
		const Array<string>& GetAllFiles() const { return _files; }
		// Returns names of all files with fiven extension ( with ext )
		// Returned referenced array is valid until next call tu this function
		const Array<string>& GetFilesWithExtension(const string& ext);
		// Returns names of all folders in directory
		const Array<string>& GetAllFolders() const { return _dirs; }

		// If value = true, all files/dirs returned will be in alphabetical order
		void SetSort(bool value);
		bool GetSort() const { return _sortAlph; }

	protected:
		void Sort();
	};
}