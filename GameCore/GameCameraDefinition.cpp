#include "GameCameraDefinition.h"
#include "GameCamera.h"

namespace FlagRTS
{
	GameCameraDefinition::GameCameraDefinition() :
		SceneObjectDefinition()
	{
		SetFinalType(GetTypeId<GameCamera>());
		SetFinalTypeName("GameCamera");

		_nearClip = 1;
		_farClip = 3000;
		_fovY = Radian(Ogre::Math::PI/4);
		_direction = Vector3(0.0f,0.5f,0.5f).normalisedCopy();
		_maxDirection = Vector3(10.0f,10.0f,10.0f);
		_minDirection = Vector3(-10.0f,-10.0f,-10.0f);
		_useDirection = true;
		_useYawPitch = false;
		_useBoundingBox = false;
		
		_zoomLevels.push_back(CameraZoomLevel(-0.1f, -Radian(Degree(89.f)), 2000.f));
		_zoomLevels.push_back(CameraZoomLevel(0.20f, -Radian(Degree(89.f)), 1200.f));
		_zoomLevels.push_back(CameraZoomLevel(0.80f, -Radian(Degree(60.f)), 200.0f));
		_zoomLevels.push_back(CameraZoomLevel(1.1f, -Radian(Degree(50.f)), 100.0f));
	}

	GameCameraDefinition::GameCameraDefinition(XmlNode* node) :
		SceneObjectDefinition(node)
	{
		SetFinalType(GetTypeId<GameCamera>());
		SetFinalTypeName("GameCamera");

		// Get node specific to GC
		XmlNode* gcNode = node->first_node("GameCamera");
		if(gcNode != 0)
		{
			_nearClip = (float)XmlUtility::XmlGetIntIfExists(gcNode, "NearClip", 1, 8, false);
			_farClip = (float)XmlUtility::XmlGetIntIfExists(gcNode, "FarClip", 3000, 7, false);
			_fovY = XmlUtility::XmlGetFloatIfExists(gcNode, "FOVY", Ogre::Math::PI/4, 4, false);

			_useDirection = XmlUtility::XmlGetFlag(gcNode, "UseDirection", false);
			_useYawPitch = XmlUtility::XmlGetFlag(gcNode, "UseYawPitch", false);
			_useBoundingBox = XmlUtility::XmlGetFlag(gcNode, "UseBoundingBox", false);

			if(_useDirection)
			{
				_direction = XmlUtility::XmlGetXYZ(gcNode->first_node("Direction"));
				_maxDirection = XmlUtility::XmlGetXYZIfExists(gcNode->first_node("MaxDirection"), 
					Vector3(Ogre::Math::POS_INFINITY,Ogre::Math::POS_INFINITY,Ogre::Math::POS_INFINITY));
				_minDirection = XmlUtility::XmlGetXYZIfExists(gcNode->first_node("MinDirection"), 
					Vector3(Ogre::Math::NEG_INFINITY,Ogre::Math::NEG_INFINITY,Ogre::Math::NEG_INFINITY));
			}
			else if(_useYawPitch)
			{
				_yaw = XmlUtility::XmlGetFloat(gcNode->first_node("Yaw"), "value");
				_maxYaw = XmlUtility::XmlGetFloatIfExists(gcNode->first_node("MaxYaw"), "value", 2*Ogre::Math::PI);
				_minYaw = XmlUtility::XmlGetFloatIfExists(gcNode->first_node("MinYaw"), "value", -2*Ogre::Math::PI);

				_pitch = XmlUtility::XmlGetFloat(gcNode->first_node("Pitch"), "value");
				_maxPitch = XmlUtility::XmlGetFloatIfExists(gcNode->first_node("MaxPitch"), "value", 2*Ogre::Math::PI);
				_minPitch = XmlUtility::XmlGetFloatIfExists(gcNode->first_node("MinPitch"), "value", -2*Ogre::Math::PI);
			}

			if(_useBoundingBox)
			{
				_cameraBounds = XmlUtility::XmlGetTLBR(gcNode->first_node("CameraBounds"));
			}
		}
	}
}