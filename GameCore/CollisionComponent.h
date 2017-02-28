#pragma once

#include "IObjectComponent.h"
#include "ObjectDefinition.h"
#include <Event.h>
#include "CollisionShapes.h"

namespace FlagRTS
{
	class SceneObject;
	class PhysicalObject;
	class UniformGridObstacle;

	/**
	Expected layout of node:
	<Component type="MinimapComponent">
	<Flags>
	<RevealedVisible/>
	<UseCustomIcon/>
	<MinimapSelectable/>
	</Flags>
	<Size x="80.f" y="160.f"/> <!-- Size of minimap icon -> whole minimap is 1000x1000 * aspect -->
	<CustomIcon name="icon.png"/> <!-- Must be provided if flag UseCustomIcon is set -->
	</Component>
	*/
	class CollisionComponentDefinition : public ObjectDefinition
	{
	protected:
		PathFinding::UniformGridObstacle* _footprint;
		CollisionFilter _collisionFilter;
		CollisionShapeType _shapeType;

	public:
		CollisionComponentDefinition(XmlNode* compNode);

		UniformGridObstacle* GetFootprint() const
		{
			return _footprint;
		}

		CollisionFilter GetCollisionFilter() const
		{
			return _collisionFilter;
		}
		
		CollisionShapeType GetCollisionShapeType() const
		{
			return _shapeType;
		}
	};

	/**
	Respresent's object's simplified physical shape used for collision/pathing.
	If object have a footprint, then it is added to ObstaclesMap.

	Owner must be a PhysicalObject.
	*/
	class CollisionComponent : public IObjectComponent
	{
	protected:
		CollisionShape _shape;
		Vector2 _orientedBounds[4]; // 2D - OOBB centered on (0,0) : corners (left-top,rigth-top,left-bot,right-bot)	

	public:
		CollisionComponent(ObjectDefinition* def, IGameObject* owner);

		/**

		*/
		void LoadResources(Ogre::SceneManager* ogreMgr);

		/**

		*/
		void UnloadResources(Ogre::SceneManager* ogreMgr);
		
		/**

		*/
		void Update(float ms);

		/**
		Returns 2d OOBB centered on (0,0)
		*/
		const Vector2* GetLocalOrientedBoundingBox() const
		{
			return _orientedBounds;
		}

		const CollisionShape& GetCollisionShape() const
		{
			return _shape;
		}
		
		CollisionShape& GetCollisionShape()
		{
			return _shape;
		}

		CollisionFilter GetCollisionFilter() const
		{
			return _shape.GetCollisionFilter();
		}

		UniformGridObstacle* GetFootprint() const 
		{ 
			return GetDefinition<CollisionComponentDefinition>()->GetFootprint(); 
		}
		
		/**

		*/
		float GetDistanceToOtherObject(PhysicalObject* target);

	protected:
		void ObjectMoved(SceneObject* );
		DEFINE_DELEGATE1(ObjectMovedDelegate, CollisionComponent,
			ObjectMoved, SceneObject*);
		ObjectMovedDelegate _onObjectMoved;
		
		void ObjectRotated(SceneObject* );
		DEFINE_DELEGATE1(ObjectRotatedDelegate, CollisionComponent,
			ObjectRotated, SceneObject*);
		ObjectRotatedDelegate _onObjectRotated;

		void ObjectSpawned(SceneObject* obj);
		DEFINE_DELEGATE1(ObjectSpawnedDelegate, CollisionComponent,
			ObjectSpawned, SceneObject*);
		ObjectSpawnedDelegate _onObjectSpawned;

		void ObjectDespawned(SceneObject* obj);
		DEFINE_DELEGATE1(ObjectDespawnedDelegate, CollisionComponent,
			ObjectDespawned, SceneObject*);
		ObjectDespawnedDelegate _onObjectDespawned;
	};
}