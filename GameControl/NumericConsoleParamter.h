#pragma once

#include "IConsoleParameter.h"

namespace FlagRTS
{
	class IntegerConsoleParameter : public IConsoleParameter
	{
	public:
		typedef int32*(*GetIntegerParamFunc)();

	protected:
		int32 _storage;
		GetIntegerParamFunc _paramGetter;

	public:
		IntegerConsoleParameter(GetIntegerParamFunc paramGetter, const string& paramName) :
			IConsoleParameter(ParamType::Integer, "Integer", paramName),
			_paramGetter(paramGetter)
		{ }

		size_t GetValueStorage() const { return reinterpret_cast<size_t>(&_storage); }

		bool ParseParamValue(const string& param, size_t storage) const
		{
			try
			{
				int32 val = std::stoi(param);
				*(reinterpret_cast<int32*>(storage)) = val;
			}
			catch(...)
			{
				return false;
			}
			return true;
		}

		string ToString() const 
		{ 
			return std::to_string(*_paramGetter()) + " : integer"; 
		}

		void GetParamValue(int32* value) const
		{ 
			*value = *_paramGetter(); 
		}

		void GetParamValue(size_t storage) const
		{ 
			*(reinterpret_cast<int32*>(storage)) = *_paramGetter(); 
		}

		void SetParamValue(int32* value)
		{
			*_paramGetter() = *value;
		}

		void SetParamValue(size_t storage)
		{
			*_paramGetter() = *(reinterpret_cast<int32*>(storage));
		}

		bool operator==(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() != ParamType::Integer && 
				otherParam->GetParamType() != ParamType::Float)
				return false;

			int32 val = 0;
			otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
			return *_paramGetter() == val;
		}

		bool operator!=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() != ParamType::Integer &&
				otherParam->GetParamType() != ParamType::Float)
				return false;

			int32 val = 0;
			otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
			return *_paramGetter() != val;
		}

		IConsoleParameter* operator+(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				int32 val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() += val;
			}
			return this;
		}

		IConsoleParameter* operator-(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				int32 val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() -= val;
			}
			return this;
		}

		IConsoleParameter* operator+=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				int32 val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() += val;
			}
			return this;
		}

		IConsoleParameter* operator-=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				int32 val = 0;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() -= val;
			}
			return this;
		}
	};

	class FloatConsoleParameter : public IConsoleParameter
	{
	public:
		typedef float*(*GetFloatParamFunc)();

	protected:
		float _storage;
		GetFloatParamFunc _paramGetter;

	public:
		FloatConsoleParameter(GetFloatParamFunc paramGetter, const string& paramName) :
			IConsoleParameter(ParamType::Float, "Float", paramName),
			_paramGetter(paramGetter)
		{ }

		size_t GetValueStorage() const { return reinterpret_cast<size_t>(&_storage); }

		bool ParseParamValue(const string& param, size_t storage) const
		{
			try
			{
				float val = std::stof(param);
				*(reinterpret_cast<float*>(storage)) = val;
			}
			catch(...)
			{
				return false;
			}
			return true;
		}

		string ToString() const 
		{ 
			return std::to_string(*_paramGetter()) + " : float"; 
		}

		void GetParamValue(float* value) const
		{ 
			*value = *_paramGetter(); 
		}

		void GetParamValue(size_t storage) const
		{ 
			*(reinterpret_cast<float*>(storage)) = *_paramGetter(); 
		}

		void SetParamValue(float* value)
		{
			*_paramGetter() = *value;
		}

		void SetParamValue(size_t storage)
		{
			*_paramGetter() = *(reinterpret_cast<float*>(storage));
		}

		bool operator==(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() != ParamType::Integer && 
				otherParam->GetParamType() != ParamType::Float)
				return false;

			float val = 0.f;
			otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
			return *_paramGetter() == val;
		}

		bool operator!=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() != ParamType::Integer &&
				otherParam->GetParamType() != ParamType::Float)
				return false;

			float val = 0.f;
			otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
			return *_paramGetter() != val;
		}

		IConsoleParameter* operator+(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				float val = 0.f;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() += val;
			}
			return this;
		}

		IConsoleParameter* operator-(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				float val = 0.f;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() -= val;
			}
			return this;
		}

		IConsoleParameter* operator+=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				float val = 0.f;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() += val;
			}
			return this;
		}

		IConsoleParameter* operator-=(IConsoleParameter* otherParam)
		{
			if(otherParam->GetParamType() == ParamType::Integer || 
				otherParam->GetParamType() == ParamType::Float)
			{
				float val = 0.f;
				otherParam->GetParamValue(reinterpret_cast<size_t>(&val));
				*_paramGetter() -= val;
			}
			return this;
		}
	};
}