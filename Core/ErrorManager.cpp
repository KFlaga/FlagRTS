#include "ErrorManager.h"
#include "LogManager.h"
#include <Windows.h>
#include "ErrorCodes.h"

namespace FlagRTS
{
	ErrorManager* ErrorManager::_instance = 0;

	ErrorManager::ErrorManager()
	{
		RegisterStdErrors();
	}

	void ErrorManager::RegisterNewPositiveErrorCode(ErrorCode& code, const string& errMsg)
	{
		ErrorCode newcode = (++_highError);
		_errors.insert(std::make_pair(newcode, errMsg));
		code = newcode;
	}

	void ErrorManager::RegisterNewNegativeErrorCode(ErrorCode& code, const string& errMsg)
	{
		ErrorCode newcode = (--_lowError);
		_errors.insert(std::make_pair(newcode, errMsg));
		code = newcode;
	}

	bool ErrorManager::RegisterErrorCode(ErrorCode code, const string& errMsg)
	{
		if(_errors.count(code) > 0)
			return false;

		_errors.insert(std::make_pair(code, errMsg));
		return true;
	}

	bool ErrorManager::ChangeErrorMessage(ErrorCode code, const string& errMsg)
	{
		if(_errors.count(code) != 0)
			return false;

		_errors.erase(code);
		_errors.insert(std::make_pair(code, errMsg));
		return true;
	}

	string ErrorManager::InterpretErrorCode(ErrorCode code)
	{
		if(_errors.count(code) > 0)
		{
			return _errors.find(code)->second;
		}
		else
		{
			return "No Error Msg";
		}
	}

	void ErrorManager::LogError(ErrorCode code, const string& file, uint32 linenr)
	{
		ADD_ERRLOG(string("Error: ") + InterpretErrorCode(code) + 
			" . In file: " + file + ", line: " + std::to_string(linenr));
	}

	void ErrorManager::LogOnError(bool condition, ErrorCode code, const string& file, uint32 linenr)
	{
		if(!condition)
		{
			ADD_ERRLOG(string("Error: ") + InterpretErrorCode(code) + 
				" . In file: " + file + ", line: " + std::to_string(linenr));
		}
	}

	void ErrorManager::Assert(bool condition, ErrorCode code, const string& file, uint32 linenr)
	{
		if(!condition)
		{
			ADD_ERRLOG(string("Error: ") + InterpretErrorCode(code) + 
				" . In file: " + file + ", line: " + std::to_string(linenr));
			exit(code);
		}
	}

	void ErrorManager::RegisterStdErrors()
	{
		// Basic errors
		RegisterErrorCode(ErrorCodes::Success, "Success");
		RegisterErrorCode(ErrorCodes::Unknown, "Unknown Error");

		// GUIManager errors
		RegisterErrorCode(ErrorCodes::GuiAlreadyRegistered, "Gui Already Registered");
		RegisterErrorCode(ErrorCodes::GuiNotFound, "Gui Not Found");

		// GUI errors
		RegisterErrorCode(ErrorCodes::LayoutAdded, "Layout already added");
		RegisterErrorCode(ErrorCodes::LayoutAlreadyLoaded, "Layout already loaded");
		RegisterErrorCode(ErrorCodes::LayoutNotFound, "Layount not found");
		RegisterErrorCode(ErrorCodes::LayoutNotLoaded, "Layout not loaded");

		RegisterErrorCode(ErrorCodes::RenderSystemNotFound, "RenderSystem not found");
		RegisterErrorCode(ErrorCodes::RenderSystemNotFound, "RenderSystem not found");

		RegisterErrorCode(ErrorCodes::ConfigFileCoundntOpen, "Couldn't open config file");
		RegisterErrorCode(ErrorCodes::ConfigSectionCoundntFind, "Couldn't find given section");

		RegisterErrorCode(ErrorCodes::CursorFailedToLoadResources, "Failed to load cursor resources");
		RegisterErrorCode(ErrorCodes::CursorContextBadCast, "Bad cast of resource to Cursor");
		RegisterErrorCode(ErrorCodes::CurserContextNotFound, "Couldn't find Cursor resource");

		RegisterErrorCode(ErrorCodes::AppStateNameNotFound, "Couldn't find AppState");

		_highError = 1;
		_lowError = -6;
	}
}