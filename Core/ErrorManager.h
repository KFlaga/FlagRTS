#pragma once

#include <map>
#include "GenericManager.h"
#include "ErrorCodes.h"

namespace FlagRTS
{
	// Class allows easy binding of error code with string messages for loging
	// ErrorCode == 0 always mean success, codes > 0 failure, and codes < 0 
	// could be success/neutral ( must be non-critical )
	class ErrorManager : public GenericManager<ErrorManager>
	{
	private:
		std::map<ErrorCode,string> _errors;
		ErrorCode _highError;
		ErrorCode _lowError;

		void RegisterStdErrors();

	protected:
		ErrorManager();
		friend GenericManager<ErrorManager>;

	public:
		// Register msg with next positive unregistered error code
		// Assigned value stored in 'code'
		void RegisterNewPositiveErrorCode(ErrorCode& code, const string& errMsg);

		// Register msg with next unregistered negative error code
		// Assigned value stored in 'code'
		void RegisterNewNegativeErrorCode(ErrorCode& code, const string& errMsg);

		// Registers message for given error code
		// If code is already registered returns false
		bool RegisterErrorCode(ErrorCode code, const string& errMsg);

		// Changes registered string, returns false if there's no such code registered
		bool ChangeErrorMessage(ErrorCode code, const string& errMsg);
		
		// Return message registered for given code
		string InterpretErrorCode(ErrorCode code);

		// Logs error
		void LogError(ErrorCode code, const string& file, uint32 linenr);
		// Logs error if condition is not met
		void LogOnError(bool condition, ErrorCode code, const string& file, uint32 linenr);
		// Logs error on failure and aborts app
		void Assert(bool condition, ErrorCode code, const string& file, uint32 linenr);
		// Returns given error code, but log it first
		ErrorCode ReturnErrorAndLog(ErrorCode code, const string& file, uint32 linenr)
		{
			LogError(code, file, linenr);
			return code;
		}
	};
}

#define LOG_ERROR(code) ErrorManager::Instance()->LogError((code),__FILE__,__LINE__)
#define LOG_ON_ERROR(cond,code) ErrorManager::Instance()->LogOnError((cond),(code),__FILE__,__LINE__)
#define ABORT_ON_ERROR(cond,code) ErrorManager::Instance()->Assert((cond),(code),__FILE__,__LINE__)
#define RETURN_ERROR(code) ErrorManager::Instance()->ReturnErrorAndLog(code,__FILE__,__LINE__)