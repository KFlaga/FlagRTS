#pragma once

#include <string>

namespace Ogre
{
	class Vector2 ;
	class Vector3 ;
	class Vector4 ;
	class Quaternion ;
	class Radian ;
	class Degree ;
	class Angle ;
	class Matrix3;
}


#define DllImport __declspec( dllimport )
#define DllExport __declspec( dllexport )

#ifdef DLLEXPORT
#define DllExclusive __declspec( dllexport )
#define DllApi __declspec( dllexport )
#else
#define DllApi 
#define DllExclusive __declspec( dllimport )
#endif

namespace FlagRTS
{
	typedef unsigned __int64 uint64;
	typedef unsigned __int32 uint32;
	typedef unsigned __int16 uint16;
	typedef unsigned __int8 uint8;

	typedef signed __int64 int64;
	typedef signed __int32 int32;
	typedef signed __int16 int16;
	typedef signed __int8 int8;

	typedef float single;
	typedef std::string string;
	typedef int ErrorCode;

	// Ogre maths - to use still have to include ogre header
	typedef Ogre::Vector2 Vector2;
	typedef Ogre::Vector3 Vector3;
	typedef Ogre::Vector4 Vector4;
	typedef Ogre::Quaternion Quaternion;
	typedef Ogre::Radian Radian;
	typedef Ogre::Degree Degree;
	typedef Ogre::Angle Angle;
	typedef Ogre::Matrix3 Matrix3;

	typedef int KeyCode;
	typedef int MouseButton;
	namespace MouseButtons
	{
		enum MB : int
		{
			Left = 0,
			Right = 1,
			Middle = 2
		};
	}

	typedef int ModifierCode;
	namespace ModifierCodes
	{
		enum MC
		{
			Shift = 1 << 0,
			Ctrl = 1 << 4,
			Alt = 1 << 8
		};
	}

	class InputState;


	struct string_less
	{	
		bool operator()(const string& _Left, const string& _Right) const
		{
			return _Left.compare(_Right) < 0;
		}
	};

	template< bool condition, typename type_true, typename type_false >
	struct ConditinalType { };

	template<typename type_true, typename type_false >
	struct ConditinalType<true, type_true, type_false>
	{
		typedef type_true type;
	};

	template<typename type_true, typename type_false >
	struct ConditinalType<false, type_true, type_false>
	{
		typedef type_false type;
	};

	struct StringIsEqual
	{
		bool operator()(const string& str1, const string& str2)
		{
			return str1.compare(str2) == 0;
		}

		bool operator()(const char* str1, const string& str2)
		{
			return str2.compare(str1) == 0;
		}

		bool operator()(const string& str1, const char* str2)
		{
			return str1.compare(str2) == 0;
		}

		bool operator()(const char* str1, const char* str2)
		{
			return std::strcmp(str1, str2) == 0;
		}
	};
}