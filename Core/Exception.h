#pragma once

#include <exception>
#include <string>

namespace FlagRTS
{
	class Exception
	{
	private:
		const char* _reason;
		const char* _file;
		int _line;
		bool _isFatal;
		bool _autoLog;

	public:
		Exception(const char* reason, const char* file, int line, bool isFatal = false, bool autoLog = true) _NOEXCEPT;
		Exception(const std::string& reason, const char* file, int line, bool isFatal = false, bool autoLog = true) _NOEXCEPT;
	
		void ShowMessageBox() const;
		void AddLog() const _NOEXCEPT;

	};

#define CastFatalException(reason) throw( new Exception(reason, __FILE__, __LINE__, true) )
#define CastException(reason) throw( new Exception(reason, __FILE__, __LINE__, false, true) )
#define CastExceptionLog(reason, log) throw( new Exception(reason, __FILE__, __LINE__, false, log) )
#define CastSpecException(ExcT, reason) throw( new ExcT(reason, __FILE__, __LINE__) )
#define CastFatalSpecException(ExcT, reason) throw( new ExcT(reason, __FILE__, __LINE__, true) )

// DEXCEPTION -> thrown only when debugging
#ifdef _DEBUG
#define CastException_d(reason) CastExceptionLog(reason, false)
#define ShowMessageBox_d(exc) exc->ShowMessageBox
#else
// In release mode only create exception  for logging, but do not throw
#define CastException_d(reason) Exception(reason, __FILE__, __LINE__, false, true)
#define ShowMessageBox_d(exc)
#endif

#define THROW throw(Exception*)
#define NO_THROW throw()
}