#pragma once

#include <Memory.h>
#define PFAlloc(Type) xNew0(Type)
#define PFDelete(ptr) xDelete(ptr)
#define PFAllocArray(Type, count) xNewArray0(Type,count) 
#define PFDeleteArray(Type, ptr) uDeleteArray(Type, ptr)

#include <Array.h>
#define PFArray FlagRTS::Array

#include <List.h>
#define PFList FlagRTS::List

#include <set>
#define PFSet std::set

#include <map>
#define PFMap std::map

#include <queue>
#define PFQueue std::priority_queue

#include <Ogre\OgreVector3.h>
#define PFVector3 Ogre::Vector3

#include <Ogre\OgreVector2.h>
#define PFVector2 Ogre::Vector2

#include <Ogre\OgreQuaternion.h>
#define PFQuaternion Ogre::Quaternion

#include <IntVector2.h>
#include <Array2d.h>
typedef FlagRTS::IntVector2 IntVector2;
#define PFArray2d FlagRTS::Array2d

#define PI 3.14159f
#define TWO_PI 6.28319f
#define PI_SQUARED 9.86960f
// Max error between to float values ( in vertices ) for them to be
// considered equal. Value relative to actual scene object sizes ( like 0.1% - 1% of small objects )
// Actual shapes are enlarged by this value in checks
#define VERTEX_ERROR 0.01f
#define VERTEX_ERROR_NEGATIVE -0.01f
// Vertex error for squared values ( squared lengths, squared distances, areas )
#define VERTEX_ERROR_SQUARED 1.0f
#define VERTEX_ERROR_SQUARED_NEGATIVE -1.0f
// Max relative error for value to be considered 0
#define RELATIVE_ERROR 1e-6f
#define RELATIVE_ERROR_NEGATIVE -1e-6f
#define IsCloseToZero(val) val < RELATIVE_ERROR && val > RELATIVE_ERROR_NEGATIVE
#define IsCloseToZeroV(val) val < VERTEX_ERROR && val > VERTEX_ERROR_NEGATIVE
#define IsCloseToZeroSV(val) val < VERTEX_ERROR_SQUARED && val > VERTEX_ERROR_SQUARED_NEGATIVE

#ifdef _DEBUG
#define PFAssert(cond, msg) assert(cond)
#else
#define PFAssert(cond, msg)  
#endif