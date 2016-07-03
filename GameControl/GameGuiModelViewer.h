#pragma once

#include <ModelTexture.h>
#include <map>

namespace MyGUI
{
	class ImageBox;
	class Widget;
}

namespace Ogre
{
	class SceneManager;
}

namespace FlagRTS
{
	class PhysicalObjectDefinition;
	class PhysicalObject;

	namespace GameGui
	{
		// Shows object
		// Creates own SceneManager and ModelTexture, draws texture from ModelTexture in MyGui Canvas
		class ModelViewer
		{
			std::map<PhysicalObjectDefinition*, PhysicalObject*> _displayObjects;
			MyGUI::ImageBox* _mainPanel;
			PhysicalObject* _viewedObject;
			Ogre::SceneManager* _sceneMgr;
			ModelTexture _model;

		public:
			// Creates Canvas and render texture with given size
			// It needs to be moved to correct position after creation
			ModelViewer(int texWidth, int texHeight, MyGUI::Widget* parent = 0);
			~ModelViewer();
			
			void Update(float ms);

			MyGUI::ImageBox* GetPanel() { return _mainPanel; }

			void SetVisible(bool value);
			bool IsVisible() const;

			// Sets object to be viewed
			// It creates object and stores it, creates one copy of object per object kind
			// and resuses it if object is to be displayed again
			void SetViewedObject(PhysicalObjectDefinition* objectDef);

			PhysicalObject* GetViewedObject() const
			{
				return _viewedObject;
			}
			
			void Resize( float ratioHorizontal, float ratioVertical );

			void SetObjectPosition(const Vector3& pos);
			void SetObjectOrientation(const Quaternion& q);
		};
	}
}