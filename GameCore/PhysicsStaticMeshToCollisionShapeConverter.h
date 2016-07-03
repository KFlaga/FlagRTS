#pragma once

#include "PhysicsIMeshToCollisionShapeConverter.h"

namespace ORTS
{
	namespace Physics
	{
		class StaticMeshToCollisionShapeConverter : public IMeshToCollisionShapeConverter
		{
		protected:

		public:
			StaticMeshToCollisionShapeConverter();
			~StaticMeshToCollisionShapeConverter();

			void AddMesh(Ogre::Mesh* mesh);
			void AddEntity(Ogre::Entity* entity);
		};
	}
}