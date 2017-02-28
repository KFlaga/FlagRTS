#include "CollisionComponent.h"

#include "PhysicalObject.h"
#include "PathingUniformGridObstacle.h"
#include "GameWorld.h"
#include "IPathingSystem.h"

namespace FlagRTS
{
	CollisionComponentDefinition::CollisionComponentDefinition(XmlNode* compNode)
	{
		/*
		<Component type="CollisionComponent">
		<AddToGraph value="true"/> <!-- If true object will be added to pathing graph, so it will be able to move and influence other objects movement -->
		<PathingGroups>
		<Group name="Ground"/>
		<Group name="Building"/>
		</PathingGroups>
		<BlockedGroups>
		<Group name="Ground"/>
		<Group name="Naval"/>
		<Group name="Building"/>
		<Group name="Missle"/>
		</BlockedGroups>
		<UseFootprint value="true"/>
		<Footprint x="6" y="6">
		<Row filled="xxxxxx"/>
		<Row filled="xxxxxx"/>
		<Row filled="xxxxxx"/>
		<Row filled="xxxxxx"/>
		<Row filled="xxxxxx"/>
		<Row filled="..xxxx"/>
		</Footprint>
		</Component>

		<Component type="PathingComponent">
		<!-- If true object will be added to pathing graph, so it will be able to move and influence other objects movement -->
		<AddToGraph value="true"/>
		<Shape value="Box"/>
		<PathingGroups>
		<Group name="Ground"/>
		</PathingGroups>
		<BlockedGroups>
		<Group name="Ground"/>
		<Group name="Naval"/>
		<Group name="Building"/>
		<Group name="Missle"/>
		</BlockedGroups>
		<UseFootprint value="false"/>
		</Component>
		*/

		uint32 pathingGroup = 0;
		XmlNode* pathingGroupsNode = compNode->first_node("PathingGroups",13);
		if(pathingGroupsNode != 0)
		{
			XmlNode* groupNode =  pathingGroupsNode->first_node("Group",5);
			while(groupNode != 0)
			{
				pathingGroup |= CollisionGroups::ParseCollisionGroupType(
					XmlUtility::XmlGetString(groupNode, "name", 4));
				groupNode = groupNode->next_sibling();
			}
		}

		uint32 pathingBlocked = 0;
		pathingGroupsNode = compNode->first_node("BlockedGroups",13);
		if(pathingGroupsNode != 0)
		{
			XmlNode* groupNode =  pathingGroupsNode->first_node("Group",5);
			while(groupNode != 0)
			{
				pathingBlocked |= CollisionGroups::ParseCollisionGroupType(
					XmlUtility::XmlGetString(groupNode, "name", 4));
				groupNode = groupNode->next_sibling();
			}
		}

		_shapeType = CollisionShapes::ParseCollisionShapeType(XmlUtility::XmlGetStringIfExists(
			compNode->first_node("Shape"), "value", "Box"));

		bool useFootprint = XmlUtility::XmlGetFlag(
			compNode, "UseFootprint");

		if(useFootprint)
		{
			XmlNode* footprintNode = compNode->first_node("Footprint", 9);
			int sizeX = XmlUtility::XmlGetInt(footprintNode, "x", 1);
			int sizeY = XmlUtility::XmlGetInt(footprintNode, "y", 1);
			_footprint = xNew2(PathFinding::UniformGridObstacle, sizeX, sizeY);

			XmlNode* rowNode = footprintNode->first_node("Row",3); 
			for(int row = 0; row < sizeY; ++row)
			{
				const char* filledCells = XmlUtility::XmlGetString(
					rowNode, "filled", 6);
				for(int col = 0; col < sizeX; ++col)
				{
					filledCells[col] == 'x' ?
						_footprint->SetAt(row, col, 
						PathFinding::CollisionFilter(pathingGroup, pathingBlocked)) :
						_footprint->SetAt(row, col, 
						PathFinding::CollisionFilter(0));
				}
				rowNode = rowNode->next_sibling();
			}
		}
	}

	CollisionComponent::CollisionComponent(ObjectDefinition* def, IGameObject* owner) :
		IObjectComponent(def, owner),
		_onObjectSpawned(this),
		_onObjectDespawned(this),
		_onObjectRotated(this),
		_onObjectMoved(this)
	{
		_ASSERT(dynamic_cast<PhysicalObject*>(_owner) != 0);
	}

	void CollisionComponent::LoadResources(Ogre::SceneManager* ogreMgr)
	{
		Vector3 halfSize(GetOwner<PhysicalObject>()->GetHalfSize());

		_shape = CollisionShape(
			GetDefinition<CollisionComponentDefinition>()->GetCollisionFilter(),
			GetDefinition<CollisionComponentDefinition>()->GetCollisionShapeType(),
			Vector3(0,0,0), halfSize);

		_orientedBounds[0] = Vector2(-halfSize.x, -halfSize.z);
		_orientedBounds[1] = Vector2(halfSize.x, -halfSize.z);
		_orientedBounds[2] = Vector2(-halfSize.x, halfSize.z);
		_orientedBounds[3] = Vector2(halfSize.x, halfSize.z);

		GetOwner<SceneObject>()->Spawned() += &_onObjectSpawned;
		GetOwner<SceneObject>()->Despawned() += &_onObjectDespawned;
		GetOwner<SceneObject>()->Rotated() += &_onObjectRotated;
		GetOwner<SceneObject>()->Moved() += &_onObjectMoved;
	}

	void CollisionComponent::UnloadResources(Ogre::SceneManager* ogreMgr)
	{
		GetOwner<SceneObject>()->Moved() -= &_onObjectRotated;
		GetOwner<SceneObject>()->Rotated() -= &_onObjectMoved;
		GetOwner<SceneObject>()->Spawned() -= &_onObjectSpawned;
		GetOwner<SceneObject>()->Despawned() -= &_onObjectDespawned;
	}

	float CollisionComponent::GetDistanceToOtherObject(PhysicalObject* target)
	{
		CollisionComponent* targetComponent = target->FindComponent<CollisionComponent>();
		_ASSERT(targetComponent != 0);

		return _shape.DistanceTo(targetComponent->GetCollisionShape());
	}

	void CollisionComponent::ObjectRotated(SceneObject* )
	{
		Vector3 halfSize(
			GetOwner<PhysicalObject>()->GetOrientationAbsolute() * 
			GetOwner<PhysicalObject>()->GetHalfSize());

		_orientedBounds[0] = Vector2(-halfSize.x, -halfSize.z);
		_orientedBounds[1] = Vector2(halfSize.x, -halfSize.z);
		_orientedBounds[2] = Vector2(-halfSize.x, halfSize.z);
		_orientedBounds[3] = Vector2(halfSize.x, halfSize.z);

		_shape.SetExtends(GetOwner<PhysicalObject>()->GetSize());
	}

	void CollisionComponent::ObjectMoved(SceneObject* target)
	{
		_shape.SetCenter(GetOwner<SceneObject>()->GetPositionAbsolute());
	}

	void CollisionComponent::ObjectSpawned(SceneObject* obj)
	{
		GameInterfaces::GetPathingSystem()->AddPathingObject(
			GetOwner<PhysicalObject>());
	}

	void CollisionComponent::ObjectDespawned(SceneObject* obj)
	{
		GameInterfaces::GetPathingSystem()->RemovePathingObject(
			GetOwner<PhysicalObject>());
	}
}
