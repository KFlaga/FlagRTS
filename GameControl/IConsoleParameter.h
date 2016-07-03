#pragma once

#include <DataTypes.h>

namespace FlagRTS
{
	// Generic paramter
	class IConsoleParameter
	{
	public:
		enum ParamType
		{
			Boolean,
			Integer,
			Float,
			String,
			Vector2,
			Vector3,
			Quaternion
		};

	protected:
		ParamType _paramType;
		string _paramTypeName;
		string _paramName;

	public:
		IConsoleParameter(ParamType paramType, const string& paramTypeName, const string& paramName) :
			_paramName(paramName),
			_paramType(paramType),
			_paramTypeName(paramTypeName)
		{ }
		virtual ~IConsoleParameter() { }

		ParamType GetParamType() const { return _paramType; }
		const string GetParamTypeName() const { return _paramTypeName; }
		const string GetParamName() const { return _paramName; }

		// Returns parameter from query -> most probably it will be 'this' parameter
		// but there may exists parameters with options or function-parameters which
		// needs additional input
		// 'startIndex' is index after parameter name
		// If parameter parsed any strings, position after last used string is
		// stored in 'startIndex'
		virtual IConsoleParameter* GetParameter(Array<string>& query, uint32* startIndex)
		{
			return this;
		}

		// Returns pointer to memory that can be used as temporary value storage
		virtual size_t GetValueStorage() const { return 0; }

		// Parses paramater value and saves result on given stroage ( on success )
		// Returns true on success or false if string dont represent valid value
		bool ParseParamValue(const string& param, size_t valueStorage)
		{
			return false;
		}

		virtual string ToString() const { return ""; }

		// Saves value of parameter in given storage
		virtual void GetParamValue(size_t valueStorage) const { }

		// Sets value of parameter in to value stored in given storage
		virtual void SetParamValue(size_t valueStorage) { }

		virtual bool operator==(IConsoleParameter* otherParam)
		{
			return false;
		}

		virtual bool operator!=(IConsoleParameter* otherParam)
		{
			return false;
		}

		virtual IConsoleParameter* operator+(IConsoleParameter* otherParam)
		{
			return this;
		}

		virtual IConsoleParameter* operator-(IConsoleParameter* otherParam)
		{
			return this;
		}

		virtual IConsoleParameter* operator+=(IConsoleParameter* otherParam)
		{
			return this;
		}

		virtual IConsoleParameter* operator-=(IConsoleParameter* otherParam)
		{
			return this;
		}
	};
}