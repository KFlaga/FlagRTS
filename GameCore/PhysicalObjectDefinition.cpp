#include "PhysicalObjectDefinition.h"
#include "AnimationDefinitionFactory.h"
#include "SelectionFlags.h"
#include <PathFindingUniformGridObstacle.h>
#include <OgreMeshManager.h>
#include <Exception.h>
#include "SceneObjectState.h"
#include "PathingSystem.h"
#include "CollisionGroup.h"
#include "CollisionShapes.h"
#include "GameWorld.h"
#include "MainGameObjectPool.h"

namespace FlagRTS
{
	PhysicalObjectDefinition::PhysicalObjectDefinition() :
		SceneObjectDefinition(),
		_animations(0),
		_footprint(0),
		_havePhysics(false),
		_portraitDef(0),
		_useDefaultPortraitPosition(true),
		_portraitPosition(Vector3::ZERO),
		_portraitOrientation(Quaternion::IDENTITY)
	{

	}

	PhysicalObjectDefinition::PhysicalObjectDefinition(XmlNode* node) :
		SceneObjectDefinition(node),
		_modelName(""),
		_meshScale(0.f,0.f,0.f),
		_ingameSize(0.f,0.f,0.f),
		_animations(xNew0(AnimationDefinitionMap)),
		_footprint(0),
		_havePhysics(false),
		_portraitDef(0),
		_useDefaultPortraitPosition(true),
		_portraitPosition(Vector3::ZERO),
		_portraitOrientation(Quaternion::IDENTITY)
	{
		// Get node specific to PO
		XmlNode* poNode = node->first_node("PhysicalObject");
		if(poNode != 0)
		{
			XmlNode* entityNode = poNode->first_node("Mesh");
			if(entityNode != 0)
			{
				_modelName = XmlUtility::XmlGetString(entityNode, "name");
				_modelGroup = XmlUtility::XmlGetString(entityNode, "group");
			}

			XmlNode* sizeNode = poNode->first_node("Size");
			if(sizeNode != 0)
				_ingameSize = XmlUtility::XmlGetXYZ(sizeNode);

			// Load mesh
			auto meshMgr = Ogre::MeshManager::getSingletonPtr();
			Ogre::MeshPtr mesh;
			try
			{
				mesh = meshMgr->getByName(_modelName, _modelGroup);
			}
			catch(...) { }

			if(mesh.isNull())
			{
				try
				{
					mesh = meshMgr->load(_modelName, _modelGroup, 
						Ogre::HardwareBuffer::HBU_STATIC, 
						Ogre::HardwareBuffer::HBU_STATIC);
				}
				catch(Ogre::Exception& e)
				{
					CastException((string("Failed to load mesh. Def name = ") +
						GetName() + ". Model name = " + _modelName + "Reson: " + e.getDescription()).c_str());
				}
			}


			// Find mesh bbox and scaling
			auto meshBounds = mesh->getBounds().getSize();
			_meshScale = _ingameSize /meshBounds; 

			XmlNode* animsNode = poNode->first_node("Animations");
			if(animsNode != 0)
				ParsePOAnimations(animsNode);

			XmlNode* flagsNode = poNode->first_node("Flags");
			if(flagsNode != 0)
				ParsePOFlags(flagsNode);

			XmlNode* pathingNode = poNode->first_node("Pathing");
			if(pathingNode != 0)
			{
				_havePhysics = true;
				ParsePathing(pathingNode);
			}

			XmlNode* portaitNode = poNode->first_node("PortraitObject");
			if(portaitNode != 0)
			{
				ParseDisplayObject(portaitNode);
			}

			_playerMaterialName = XmlUtility::XmlGetStringIfExists(
				poNode->first_node("PlayerColorMaterial"), "name", "");
		}
	}

	PhysicalObjectDefinition::~PhysicalObjectDefinition()
	{
		xDeleteSafe(_footprint);
		//	xDeleteSafe(_physicsDefinition);
		if(_animations != 0)
		{
			for(auto asetIt = _animations->begin(), end = _animations->end();
				asetIt != end; ++asetIt)
			{
				for(auto animIt = asetIt->second->begin(), end = asetIt->second->end();
					animIt != end; ++animIt)
				{
					xDelete(animIt->Value);
				}
				xDelete(asetIt->second);
			}
			xDelete(_animations);
		}
	}

	void PhysicalObjectDefinition::ParsePOFlags(XmlNode* flagsNode)
	{
		_isSelectable = XmlUtility::XmlGetFlag(flagsNode, "IsSelectable");
		if(_isSelectable)
			_selectionFlags |= SelectionFlags::Selectable;
		_isHoverable = XmlUtility::XmlGetFlag(flagsNode, "IsHoverable");
		if(_isHoverable)
			_selectionFlags |= SelectionFlags::Hoverable;
	}

	void PhysicalObjectDefinition::ParsePOProperties(XmlNode* propsNode)
	{

	}

	void PhysicalObjectDefinition::ParsePOAnimations(XmlNode* animsNode)
	{
		AnimationDefinitionFactory animFactory;

		// Parse all states
		XmlNode* stateNode = animsNode->first_node("State",5);
		while(stateNode != 0)
		{
			const char* stateName = XmlUtility::XmlGetString(stateNode,"name",4);
			size_t stateNum = SceneObjectStates::Unknown;
			stateNum = SceneObjectStates::ParseStateType(stateName);

			if(stateNum == SceneObjectStates::Unknown)
			{
				CastException((string("Not supported state name in animations. Def name = ") +
					GetName() + ". State name = " + stateName).c_str());
			}

			AnimationDefinitionsSet* animsForState(xNew0(AnimationDefinitionsSet));
			// Parse all animation for this state
			XmlNode* animNode = stateNode->first_node("Animation",9);
			while(animNode != 0)
			{
				AnimationDefinition* anim = 0;

				try
				{
					anim = animFactory.CreateCast(animNode);
				}
				catch(std::exception& e)
				{
					CastException((string("Failed to create animation definition. Def name = ") +
						GetName() + ". State name = " + stateName + "Reason: " + e.what()).c_str());
				}

				animsForState->push_back(anim);

				animNode = animNode->next_sibling("Animation",9);
			}

			_animations->insert(std::make_pair(
				stateNum, animsForState));

			stateNode = stateNode->next_sibling("State",5);
		}
	}

	void PhysicalObjectDefinition::ParsePathing(XmlNode* pathingNode)
	{
		_addToPathingGraph = XmlUtility::XmlGetBoolIfExists(pathingNode->first_node("AddToGraph"), "value", true);

		_pathingGroup = 0;
		XmlNode* pathingGroupsNode = pathingNode->first_node("PathingGroups",13);
		if(pathingGroupsNode != 0)
		{
			XmlNode* groupNode =  pathingGroupsNode->first_node("Group",5);
			while(groupNode != 0)
			{
				_pathingGroup |= CollisionGroups::ParseCollisionGroupType(
					XmlUtility::XmlGetString(groupNode, "name", 4));
				groupNode = groupNode->next_sibling();
			}
		}

		_pathingBlocked = 0;
		pathingGroupsNode = pathingNode->first_node("BlockedGroups",13);
		if(pathingGroupsNode != 0)
		{
			XmlNode* groupNode =  pathingGroupsNode->first_node("Group",5);
			while(groupNode != 0)
			{
				_pathingBlocked |= CollisionGroups::ParseCollisionGroupType(
					XmlUtility::XmlGetString(groupNode, "name", 4));
				groupNode = groupNode->next_sibling();
			}
		}

		_pathingShape = CollisionShapes::ParseCollisionShapeType(XmlUtility::XmlGetStringIfExists(
			pathingNode->first_node("Shape",5), "value", "Box"));

		bool useFootprint = XmlUtility::XmlGetFlag(
			pathingNode, "UseFootprint");
		if(useFootprint)
		{
			XmlNode* footprintNode = pathingNode->first_node("Footprint", 9);
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
						PathFinding::CollisionFilter(_pathingGroup, _pathingBlocked)) :
						_footprint->SetAt(row, col, 
						PathFinding::CollisionFilter(0));
				}
				rowNode = rowNode->next_sibling();
			}
		}
	}

	void PhysicalObjectDefinition::ParseDisplayObject(XmlNode* portraitNode)
	{
		string defName = XmlUtility::XmlGetString(portraitNode, "definition");
		string defType = XmlUtility::XmlGetString(portraitNode, "type");

		XmlNode* positionNode = portraitNode->first_node("Position");
		if(positionNode != 0)
		{
			_portraitPosition = XmlUtility::XmlGetXYZ(positionNode);
			_useDefaultPortraitPosition = false;
		}

		XmlNode* orientNode = portraitNode->first_node("Orientation");
		if(orientNode != 0)
		{
			_portraitOrientation = XmlUtility::XmlGetQuaternion(orientNode);
		}

		auto getPortraitDefinition = [this, defName, defType]() 
		{
			_portraitDef = static_cast<PhysicalObjectDefinition*>(
				GameWorld::GlobalWorld->GetSceneObjectDefinition(defType, defName));
		};
		MainGameObjectPool::GlobalPool->OnAllDefinitionsLoaded() +=
			xNew1(DelegateEventHandler<decltype(getPortraitDefinition)>, getPortraitDefinition);
	}
}