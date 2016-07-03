#pragma once

#include <TypeDefs.h>
#include <Array.h>
#include <Event.h>
#include <OgreTexture.h>
#include <OgreVector2.h>
#include <MyGUI\MyGUI_MouseButton.h>

namespace MyGUI
{
	class ImageBox;
	class Widget;
}

namespace FlagRTS
{
	// Minimap is quad with terrain texture
	// Creates own SceneManager for managing MinimapUnits ( which are represented by Billboards )
	// Contains BillboardSets with units of each player + other icons
	// All this is rendered to one texture
	class MapPreview
	{
	private:
		MyGUI::ImageBox* _imageWidget;
		Ogre::SceneManager* _ogreMgr;
		Ogre::Camera* _renderCamera;
		Ogre::SceneNode* _cameraNode;
		Ogre::SceneNode* _bgNode;
		Ogre::ManualObject* _bgPlane;
		Ogre::SceneNode* _billboardNode;
		int _startingPositionsCount;
		Ogre::SceneNode* _billNode;
		Ogre::BillboardSet* _startPositionsBillboardSet;
		Array<Ogre::Billboard*> _startPositions;
		Ogre::BillboardSet* _highlightsBillboardSet;
		Array<Ogre::Billboard*> _highlights;
		Ogre::TexturePtr _finalTexture;
		Ogre::Vector2 _worldSize;
		Ogre::Vector2 _startPositionSize;

		Event<int> _startPositionLeftClicked;
		Event<int> _startPositionRightClicked;

	public:
		MapPreview(MyGUI::ImageBox* imageBox);
		~MapPreview();

		void Update(float ms);
	
		void SetNewMap(
			const Vector2& mapSize,
			const string& terrainImagePath,
			const string& terrainImageName,
			int startingPositionsCount);

		void MoveStartingPosition(int number, const Vector2& pos);
		void SetStartingPositionColor(int number, const Vector3& color);
		void DisableStartingPosition(int number); // Moves start pos away -> to enable again just call MoveStartingPosition
		void EnableStartPositionHighlight(int number);
		void DisableStartPositionHighlight(int number);

		// Begins rendering map to texture
		void Show();
		// Ends rendering map to texture
		void Hide();

		// Resets map image / start locs
		void ResetMap();

		const Ogre::TexturePtr& GetRenderTexture() const { return _finalTexture; }

		Event<int>& GetStartPositionLeftClickedEvent() { return _startPositionLeftClicked; }
		Event<int>& GetStartPositionRightClickedEvent() { return _startPositionRightClicked; }

	private:
		void CreateRenderCamera();
		void CreateTerrainPlane();
		void CreateBillboards();
		void CreateTerrainMaterial();
		void LoadTerrainTexture(const string& terrainImagePath,
			const string& terrainImageName);

		void MousePressed(MyGUI::Widget*, int, int, MyGUI::MouseButton);
	};
}