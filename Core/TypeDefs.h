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

#define DISALLOW_COPY(Type) \
	private: \
		Type(const Type&); \
		Type& operator=(const Type&);

#define DEFINE_SINGLETON_CLASS(Type) \
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
}