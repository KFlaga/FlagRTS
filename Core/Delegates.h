#pragma once

#include <list>

namespace FlagRTS
{
	template<typename fntype, typename... Args>
	class LambdaDelegate
	{
	private:
		fntype _callback;

	public:
		LambdaDelegate(fntype callback) : _callback(callback) { }

		const fntype& GetCallback() const { return _callback; }

		void operator()(Args... args) { _callback(args...); }

		LambdaDelegate& operator=(const LambdaDelegate& other)
		{
			_callback = other._callback;
		}
	};

	template<class OwnerClass, typename ReturnType, typename... Args>
	class Delegate
	{
		typedef ReturnType(OwnerClass::*_delegate)(Args...);

	private:
		_delegate _callback;
		OwnerClass* _owner;

	public:
		Delegate() :
			_owner(0), _callback(0) { }

		Delegate(OwnerClass* owner, _delegate callback) :  
			_owner(owner), _callback(callback) { }

		ReturnType operator()(Args... args) { _owner->_callback(args); }

		Delegate& operator=(Delegate& other)
		{
			_callback = other._callback;
			_owner = other._owner;
			return *this;
		}
	};
}