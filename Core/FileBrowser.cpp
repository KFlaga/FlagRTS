#include "FileBrowser.h"
#include <Windows.h>
#include <algorithm>

//typedef struct _WIN32_FIND_DATA {
//  DWORD    dwFileAttributes;
//  FILETIME ftCreationTime;
//  FILETIME ftLastAccessTime;
//  FILETIME ftLastWriteTime;
//  DWORD    nFileSizeHigh;
//  DWORD    nFileSizeLow;
//  DWORD    dwReserved0;
//  DWORD    dwReserved1;
//  TCHAR    cFileName[MAX_PATH];
//  TCHAR    cAlternateFileName[14];
//} WIN32_FIND_DATA, *PWIN32_FIND_DATA, *LPWIN32_FIND_DATA;


namespace FlagRTS
{
	FileBrowser::FileBrowser() :
		_currentDir(""),
		_sortAlph(false)
	{

	}

	bool FileBrowser::UpdateFiles()
	{
		// Check that the input path plus 3 is not longer than MAX_PATH.
		// Three characters are for the "\*" plus NULL appended below.
		if(_currentDir.size() > MAX_PATH - 3)
		{
			return false;
		}

		WIN32_FIND_DATA ffd;
		HANDLE hFind = INVALID_HANDLE_VALUE;
		DWORD dwError=0;
		Array<string> tempFiles;
		Array<string> tempDirs;
		string tempDir;
		// Prepare string for use with FindFile functions.  First, copy the
		// string to a buffer, then append '\*' to the directory name.
		if( _currentDir[_currentDir.size() - 1] == '\\' ||
			_currentDir[_currentDir.size() - 1] == '/' )
		{
			tempDir = _currentDir + "*";
		}
		else
		{
			tempDir = _currentDir + "\\*";
		}

		tempDir.reserve(MAX_PATH);

		// Find the first file in the directory.

		hFind = FindFirstFile(tempDir.c_str(), &ffd);

		if (INVALID_HANDLE_VALUE == hFind) 
		{
			return false;
		} 

		// List all the files in the directory with some info about them.

		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if( strcmp(ffd.cFileName, ".") == 0 || strcmp(ffd.cFileName, "..") == 0 )
					continue; // Some crap that find returns

				tempDirs.push_back(string(ffd.cFileName));
			}
			else
			{
				tempFiles.push_back(string(ffd.cFileName));
			}
		}
		while (FindNextFile(hFind, &ffd) != 0);

		dwError = GetLastError();
		if (dwError != ERROR_NO_MORE_FILES) 
		{
			return false;
		}

		FindClose(hFind);

		_files = tempFiles;
		_dirs = tempDirs;

		if(_sortAlph)
			Sort();

		return true;
	}

	void FileBrowser::SetSort(bool value)
	{
		if(_sortAlph == false && value == true)
		{
			Sort();
		}

		_sortAlph = value;

	}

	void FileBrowser::Sort()
	{
		std::sort(_files.begin(), _files.end());
		std::sort(_dirs.begin(), _dirs.end());
	}

	const Array<string>& FileBrowser::GetFilesWithExtension(const string& ext)
	{
		_lastFilter.clear();
		for(unsigned int i = 0; i < _files.size(); ++i)
		{
			string& fileName = _files[i];
			if( fileName.size() < ext.size() )
				continue;
			
			bool extMatch = true;
			for(int e = (int)ext.size() - 1, c = 0; e >= 0; --e, ++c)
			{
				if(fileName[(int)fileName.size() - c - 1] != ext[e])
				{
					extMatch = false;
					break;
				}
			}

			if( extMatch )
			{
				if(fileName[(int)fileName.size() - ext.size() - 1] == '.')
					_lastFilter.push_back(fileName);
			}
		}
		return _lastFilter;
	}
}