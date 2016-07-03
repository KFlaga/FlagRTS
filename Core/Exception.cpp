#include "Exception.h"
#include <Windows.h>
#include "LogManager.h"

namespace FlagRTS
{
	Exception::Exception(const char* reason, const char* file, int line, bool isFatal, bool autoLog) _NOEXCEPT :
		_reason(reason),
		_file(file),
		_line(line),
		_isFatal(isFatal),
		_autoLog(autoLog)
	{
		if(_autoLog)
			AddLog();
	}

	void Exception::ShowMessageBox() const
	{
		// TODO add some sensible message box with call stack etc.
		string message = "";
		if(_isFatal)
		{
			message += "Fatal exception cast. Reason: \r\n";
		}
		else
		{
			message += "Exception cast. Reason: \r\n";
		}
		message += _reason;
		message += "\r\nIn file : \r\n";
		message += _file;
		message += "\r\nOn line : \r\n";
		message += std::to_string(_line);

		MessageBoxA(0, message.c_str(), "Error", MB_OK);
	}

	void Exception::AddLog() const _NOEXCEPT
	{
#ifndef _DEBUG // Dont log it on debug for now
		string message = "";
		if(_isFatal)
		{
			message += "Fatal exception cast. Reason: ";
		}
		else
		{
			message += "Exception cast. Reason: ";
		}
		message += _reason;
		message += "In file : ";
		message += _file;
		message += "On line : ";
		message += std::to_string(_line);
		ADD_ERRLOG(message);
#endif
	}
}