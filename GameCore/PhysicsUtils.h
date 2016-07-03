#pragma once

#include <Ogre\OgreVector3.h>
#include <Ogre\OgreQuaternion.h>
#include <Bullet\LinearMath\btQuaternion.h>
#include <Bullet\LinearMath\btVector3.h>
#include <Bullet\LinearMath\btTransform.h>
#include <Bullet\LinearMath\btTransformUtil.h>

namespace ORTS
{
	namespace Physics
	{
		inline Ogre::Vector3 ConvertToOgre(const btVector3& bullet)
		{
			return Ogre::Vector3(bullet.x(), bullet.y(), bullet.z());
		}

		inline btVector3 ConvertToBullet(const Ogre::Vector3& ogre)
		{
			return btVector3(ogre.x, ogre.y, ogre.z);
		}

		inline Ogre::Quaternion ConvertToOgre(const btQuaternion& bullet)
		{
			return Ogre::Quaternion(bullet.w(), bullet.x(), bullet.y(), bullet.z());
		}

		inline btQuaternion ConvertToBullet(const Ogre::Quaternion& ogre)
		{
			return btQuaternion(ogre.x, ogre.y, ogre.z, ogre.w);
		}

		inline btTransform CreateTransform(const Ogre::Vector3& position, const Ogre::Quaternion& orientation)
		{
			return btTransform( ConvertToBullet(orientation), ConvertToBullet(position) );
		}
	}
}