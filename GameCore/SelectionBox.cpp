#include "SelectionBox.h"
//#include <Ogre\OgreManualObject.h>
#include <Exception.h>
#include "GameWorld.h"
#include "Map.h"
#include <MyGUI\MyGUI.h>

namespace FlagRTS
{

	Rect3::Rect3()
	{ }

	Rect3::Rect3(const Vector3& tl, const Vector3& tr,
		const Vector3& bl, const Vector3& br) :
	_topLeft(tl),
		_topRight(tr),
		_botLeft(bl),
		_botRight(br)
	{
		Update();
	}

	void Rect3::Update()
	{
		// Find rotation : 
		// - get BL - TL and normalize
		Vector3 top = _topRight - _topLeft;
		top.normalise();
		// - rotation angle -a = atan2(left.x,left.z) ( a rotates
		// (1,0) vector to top = (x,z) = (cos(a),sin(a)), we need
		// reversed rotation ). Rotation matrix = 
		// [ c(-a),0,s(-a); 0,1,0; -s(-a),0,-c(-a) ]
		// Becouse c(-a)=c(a)=left.x, s(-a)=-s(a)=-left.z, angle need not to
		// be computed. Ok, set this matrix:
		_rotation[0][0]  = top.x;
		_rotation[0][1] = 0.0f;
		_rotation[0][2]  = top.z;
		_rotation[1][0] = 0.0f;
		_rotation[1][1] = 1.0f;
		_rotation[1][2] = 0.0f;
		_rotation[2][0] = -top.z;
		_rotation[2][1] = 0.0f;
		_rotation[2][2] = top.x;
		// Finally find box coords in rotated world coords
		Vector3 tlr = Rotate(_topLeft);
		Vector3 brr = Rotate(_botRight);
		// Becouse in rotated coords order of corners can change, find correct one
		_topLeftRot.x = std::min(tlr.x, brr.x);
		_topLeftRot.y = tlr.y;
		_topLeft.z = std::min(tlr.z,brr.z);
		_botRightRot.x = std::min(tlr.x, brr.x);
		_botRightRot.y = brr.y;
		_botRightRot.z = std::min(tlr.z,brr.z);
	}

	bool Rect3::HitTest(const Vector3& point) const
	{
		// First rotate point
		Vector3 rPoint(Rotate(point));
		// Perform standaed rectangle check now
		if(rPoint.x > _botRightRot.x || 
			rPoint.x < _topLeftRot.x ||
			rPoint.z > _botRightRot.z ||
			rPoint.z < _topLeftRot.z )
			return false;

		return true;
	}

	SelectionBox::SelectionBox(GameCamera* camera) :
		_box(0),
		_camera(camera),
		_isSelecting(false),
		_terrainBox(xNew0(Rect3)),
		_onCameraMoved(this)
	{
		CreateBox();
		camera->Moved().AddHandler(&_onCameraMoved);
	}

	SelectionBox::SelectionBox() :
		_box(0),
		_camera(0),
		_isSelecting(false),
		_terrainBox(xNew0(Rect3)),
		_onCameraMoved(this)
	{
		CreateBox();
	}

	SelectionBox::~SelectionBox()
	{
		//_ogreMgr->destroyManualObject(_box);
		_box->detachFromWidget();
		MyGUI::Gui::getInstancePtr()->destroyWidget(_box);
		if(_camera != 0)
			_camera->Moved() -= &_onCameraMoved;
		xDelete(_terrainBox);
	}

	void SelectionBox::ChangeActiveCamera(GameCamera* camera)
	{
		if(_camera != 0)
			_camera->Moved() -= &_onCameraMoved;
		_camera = camera;
		_camera->Moved() += &_onCameraMoved;
	}

	void SelectionBox::StartSelection(const Ogre::Vector2& mousePos)
	{
		// If selecting already exception in debug - should not happen
		if ( _isSelecting )
		{
			CastException_d("SelectionBox : attemp to start selection on working box");
			return;
		}

		_box->setVisible(true);
		_isSelecting = true;

	//	auto ray = _camera->GetOgreCamera()->getCameraToViewportRay(mousePos.x, mousePos.y);
	//	_terrainStart = Map::CurrentMap->GetTerrainInterscetion(ray);

		_startPos = mousePos;

		UpdateSelection(mousePos);
	}

	void SelectionBox::EndSelection()
	{
		// If not selecting exception in debug - should not happen
		if ( !_isSelecting )
		{
			CastException_d("SelectionBox : attemp to end selection on not working box");
			return;
		}

		_box->setVisible(false);
		_isSelecting = false;
		FindTerrainCoords();
	}

	void SelectionBox::AbortSelection()
	{
		_box->setVisible(false);
		_isSelecting = false;
	}

	void SelectionBox::FindTerrainCoords()
	{
		int width = _camera->GetCameraViewport()->getActualWidth();
		int height = _camera->GetCameraViewport()->getActualHeight();

		float left = (float)_box->getLeft()/(float)width;
		float top =  (float)_box->getTop()/(float)height;
		float right = (float)(_box->getLeft() + _box->getWidth())/(float)width;
		float bot = (float)(_box->getTop() + _box->getHeight())/(float)height;

		auto ray = _camera->GetOgreCamera()->getCameraToViewportRay(left, top);
		_terrainBox->SetTopLeft(GameWorld::GlobalWorld->GetMap()->GetTerrainInterscetion(ray));

		ray = _camera->GetOgreCamera()->getCameraToViewportRay(right, top);
		_terrainBox->SetTopRight(GameWorld::GlobalWorld->GetMap()->GetTerrainInterscetion(ray));

		ray = _camera->GetOgreCamera()->getCameraToViewportRay(left, bot);
		_terrainBox->SetBotLeft(GameWorld::GlobalWorld->GetMap()->GetTerrainInterscetion(ray));

		ray = _camera->GetOgreCamera()->getCameraToViewportRay(right, bot);
		_terrainBox->SetBotRight(GameWorld::GlobalWorld->GetMap()->GetTerrainInterscetion(ray));

		_terrainBox->Update();
	}

	void SelectionBox::UpdateSelection(const Ogre::Vector2& mousePos)
	{
		// If not selecting exception in debug - should not happen
		if ( !_isSelecting )
		{
			CastException_d("SelectionBox : attemp to update selection on not working box");
			return;
		}

		Vector2 topLeft(std::min(mousePos.x, _startPos.x), std::min(mousePos.y, _startPos.y));
		Vector2 botRight(std::max(mousePos.x, _startPos.x), std::max(mousePos.y, _startPos.y));

		_box->setRealCoord( topLeft.x, topLeft.y, botRight.x - topLeft.x, botRight.y - topLeft.y);

		/*auto ray = _camera->GetOgreCamera()->getCameraToViewportRay(mousePos.x, mousePos.y);
		_terrainEnd = Map::CurrentMap->GetTerrainInterscetion(ray);

		_box->clear();
		_box->estimateVertexCount(5);
		_box->begin("Ogre", Ogre::RenderOperation::OT_LINE_STRIP, "Materials");
		_box->position(_terrainStart.x, 0, _terrainStart.z);
		_box->position(_terrainEnd.x, 0, _terrainStart.z);
		_box->position(_terrainEnd.x, 0, _terrainEnd.z);
		_box->position(_terrainStart.x, 0, _terrainEnd.z);
		_box->position(_terrainStart.x, 0, _terrainStart.z);
		_box->end();

		_box->setBoundingBox(Ogre::AxisAlignedBox::BOX_INFINITE);*/
	}

	void SelectionBox::CreateBox()
	{
		/*_box = _ogreMgr->createManualObject();
		_box->setRenderQueueGroup(Ogre::RENDER_QUEUE_OVERLAY);
		_box->setVisible(false);
		_box->setQueryFlags(0);
		_ogreMgr->getRootSceneNode()->createChildSceneNode()->attachObject(_box);*/

		_box = MyGUI::Gui::getInstancePtr()->createWidget<MyGUI::Widget>("Border",
			0, 0, 1, 1, MyGUI::Align::Center, "Main");
		_box->setVisible(false);
	}


	void SelectionBox::CameraMoved(SceneObject* camera)
	{
		// When camera is moved, move selection box as well, so its
		// real coords remains the same
	}
}