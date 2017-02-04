#include "ModelMarkerDefinition.h"
#include <OgreMeshManager.h>
#include "SceneObjectState.h"
#include <Exception.h>
#include "ModelMarker.h"

namespace FlagRTS
{
	ModelMarkerDefinition::ModelMarkerDefinition() :
		SceneMarkerDefinition()
	{
		SetFinalType(GetTypeId<ModelMarker>());
		SetFinalTypeName("ModelMarker");
	}

	ModelMarkerDefinition::ModelMarkerDefinition(XmlNode* node) :
		SceneMarkerDefinition(node),
		_modelName(""),
		_meshScale(0.f,0.f,0.f),
		_ingameSize(0.f,0.f,0.f)
	{
		SetFinalType(GetTypeId<ModelMarker>());
		SetFinalTypeName("ModelMarker");

		// Get node specific to PO
		XmlNode* poNode = node->first_node("SceneMarker")->first_node("ModelMarker");
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

		}
	}
}