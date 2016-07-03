#pragma once

namespace FlagRTS
{
	// That's interesting one - enables to retrieve last fired event
	// arguments in actions ( stores passed arguments bu pointers, so
	// they're valid only within eventhandler scope )
	
	class EventArgsRetriever
	{
	private:
		static void* _arg;
		//static void* _arg2;
		//static void* _arg3;

	public:
		template<typename T1>
		static void Capture(T1* arg) 
		{ 
			_arg = reinterpret_cast<void*>(arg); 
		//	_arg2 = reinterpret_cast<void*>(arg2); 
		//	_arg3 = reinterpret_cast<void*>(arg3); 
		}

		template<typename T1>
		static T1* Retrieve()
		{
			return reinterpret_cast<T1*>(_arg);
		}
		/*
		template<typename T2>
		static T2* Retrieve_2()
		{
			return reinterpret_cast<T2*>(_arg2);
		}

		template<typename T3>
		static T3* Retrieve_3()
		{
			return reinterpret_cast<T3*>(_arg3);
		}
		*/
	};
}