#pragma once

#include <TypeDefs.h>
#include <Pointers.h>
#include <Ogre\OgreVector2.h>
#include <Ogre\OgreVector3.h>
#include <Ogre\OgreVector4.h>
#include <Ogre\OgreQuaternion.h>
#include <IntVector2.h>

// Commonly used containers
//#include <list>
//#include <map>

// Ogre Classes declarations
namespace Ogre
{
	class MovableObject;
	class Entity;
	class Camera;
	class SceneNode;
	class BillboardSet;
	class ManualObject;
	class SceneNode;
	class Polygon;
	class SceneManager;
}

// Typedefs to use in project
namespace FlagRTS
{
	typedef Ogre::MovableObject MovableObject;
	typedef Ogre::SceneNode SceneNode;
	typedef Ogre::Polygon Polygon;

	typedef uint64 UnitClass;
}



