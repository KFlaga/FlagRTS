#pragma once

#include "IConsoleParameter.h"

namespace FlagRTS
{
	class BooleanConsoleParameter : public IConsoleParameter
	{
	public:
		typedef bool*(*GetBooleanParamFunc)();

	protected:
		bool _storage;
		GetBooleanParamFunc _paramGetter;

	public:
		BooleanConsoleParameter(GetBooleanParamFunc paramGetter, const string& paramName) :
			IConsoleParameter(ParamType::Integer, "Integer", paramName),
			_paramGetter(paramGetter)
		{ }

		size_t GetValueStorage() const { return reinterpret_cast<size_t>(&_storage); }

		bool ParseParamValue(const string& param, size_t storage) const
		{
			if( param.compare("True") == 0 || param.compare("true") )
			{
				*(reinterpret_cast<bool*>(storage)) = true;
			}
			else if( param.compare("False") == 0 || param.compare("false") )
			{
				*(reinterpret_cast<bool*>(storage)) = true;
			}
			else
				return false;

			return true;
		}

		string ToString() const 
		{ 
			return std::to_string(*_paramGetter()); 
		}

		void GetParamValue(bool* value) const
		{ 
			*value = *_paramGetter(); 
		}

		void GetParamValue(size_t storage) const
		{ 
			*(reinterpret_cast<int64*>(storage)) = *_paramGetter(); 
		}

		void SetParamValue(bool* value)
		{
			*_paramGetter() = *value;
		}

		void SetParamValue(size_t storage)
		{
			*_paramGetter() = *(reinterpret_cast<bool*>(storage));
		}

		bool operator==(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() != ParamType::Boolean)
				return false;

			bool val = 0;
			otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
			return *_paramGetter() == val;
		}

		bool operator!=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() != ParamType::Boolean)
				return false;

			bool val = 0;
			otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
			return *_paramGetter() != val;
		}

		IConsoleParameter* operator+(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Boolean)
			{
				bool val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() += val;
			}
			return this;
		}

		IConsoleParameter* operator-(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Boolean)
			{
				bool val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() -= val;
			}
			return this;
		}

		IConsoleParameter* operator+=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Boolean)
			{
				bool val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() += val;
			}
			return this;
		}

		IConsoleParameter* operator-=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Boolean)
			{
				bool val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() -= val;
			}
			return this;
		}
	};
}