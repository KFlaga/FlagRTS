#pragma once

#define SINGLETON_CLASS(Type) \
	private: \
		static Type* _instance; \
		Type(); \
		Type(const Type&); \
		Type& operator=(const Type&); \
	public: \
		static void Initialize(); \
		static void Release(); \
		static Type* Instance() { return _instance; }

namespace FlagRTS
{
	// Base class for all manager classes ( singleton with Initialize / Release )
	template<class Type>
	class GenericManager
	{
	protected:
		static Type* _instance;

		GenericManager() { }
		GenericManager(const Type&);
		GenericManager& operator=(const Type&);
	public:
		static void Initialize()
		{
			if(_instance == 0)
			{
				_instance = new Type();
			}
		}

		static void Release()
		{
			if(_instance != 0)
			{
				delete _instance;
				_instance = 0;
			}
		}
		
		static Type* Instance() 
		{ 
			return _instance; 
		}

		// Used when setting global instances in other dlls etc
		static void SetNewInstance(Type* instance) 
		{ 
			_instance = instance;
		}
	};
}