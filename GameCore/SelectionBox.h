#pragma once

#include "DataTypes.h"
#include <OgreVector2.h>
#include <OgreVector3.h>
#include "GameCamera.h"
#include <Event.h>

namespace Ogre
{
	class ManualObject;
	class SceneManager;
}

namespace MyGUI
{
	class Widget;
}

namespace FlagRTS
{
	class GameCamera;
	class SceneObject;

	// Simple struct representing flat rect with 4 corners and some
	// utility functions. Assumes y is constant
	class Rect3
	{
	private:
		// Corners in normal coords
		Vector3 _topLeft;
		Vector3 _topRight;
		Vector3 _botLeft;
		Vector3 _botRight;
		// Corners in rotated world coords
		Vector3 _topLeftRot;
		Vector3 _botRightRot;
		// Rotation matrix, rotates around y-axis
		// Transforms local coors system to global one, for hit-test purpose
		float _rotation[3][3]; 

	public:
		Rect3();
		Rect3(const Vector3& tl, const Vector3& tr,
			const Vector3& bl, const Vector3& br);

		// Computes new rotation after corners are changed
		void Update();

		const Vector3& GetTopLeft() const { return _topLeft; }
		void SetTopLeft(const Vector3& tl) { _topLeft = tl; }

		const Vector3& GetTopRight() const { return _topRight; }
		void SetTopRight(const Vector3& tr) { _topRight = tr; }

		const Vector3& GetBotLeft() const { return _botLeft; }
		void SetBotLeft(const Vector3& bl) { _botLeft = bl; }

		const Vector3& GetBotRight() const { return _botRight; }
		void SetBotRight(const Vector3& br) { _botRight = br; }

		Vector3 Rotate(const Vector3& point) const
		{
			return Vector3( 
				_rotation[0][0] * point.x + _rotation[0][2] * point.z ,
				point.y,
				_rotation[2][0] * point.x + _rotation[2][2] * point.z);
		}

		bool HitTest(const Vector3& point) const;
	};

	class SelectionBox
	{	
		//Ogre::SceneManager* _ogreMgr;

		// Custom object representing selection box, that is shown on screen
		//Ogre::ManualObject* _box;
		MyGUI::Widget* _box;
		// Current in-game camera, used for casting box to terrain coords
		GameCamera* _camera;

		// Position where selection started
		Vector2 _startPos;
		//Vector3 _terrainStart;
		//Vector3 _terrainEnd;

		// Position of sbox corners on terrain
		Rect3* _terrainBox;

		// If true, player is selecting and box is visible
		bool _isSelecting;

	public:
		// Creates selection box rendered with given camera
		SelectionBox(GameCamera* camera);
		// Creates selection box without camera, which need to be set 
		// prior to using sbox
		//SelectionBox(Ogre::SceneManager* ogreMgr);
		SelectionBox();
		~SelectionBox();

		void ChangeActiveCamera(GameCamera* camera);

		// Shows selection box on screen and saves its start coords
		void StartSelection(const Vector2& mousePos);

		// Ends selection, hides box and saves world ( terrain ) coords
		// of box corners
		void EndSelection();

		// Ends selection and hides box, do not save box coords
		void AbortSelection();

		Rect3* GetSelectionTerrainRect() const { return _terrainBox; }

		// Returns corner fo selection box in terrain coords
		const Vector3& GetTopLeft() const { return _terrainBox->GetTopLeft(); }
		// Returns corner fo selection box in terrain coords
		const Vector3& GetBotRight() const { return _terrainBox->GetBotRight(); }

		bool HitTest(const Vector3& point) const { return _terrainBox->HitTest(point); }

		//Ogre::ManualObject* GetBox()  const { return _box; }
		MyGUI::Widget* GetBox() const { return _box; }

		// Updates second corner of selection box
		void UpdateSelection(const Vector2& mousePos);

		bool IsSelecting() const {return _isSelecting; }

	private:
		void FindTerrainCoords();
		void CreateBox();

		class OnCameraMovedEH : public EventHandler<SceneObject*>
		{
		private:
			SelectionBox* _parent;
		public:
			OnCameraMovedEH(SelectionBox* sbox) : _parent(sbox) { }
			void operator()(SceneObject* camera) { _parent->CameraMoved(camera); }
		} _onCameraMoved;

		void CameraMoved(SceneObject* camera);
		friend OnCameraMovedEH;
	};
}