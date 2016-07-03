#include "CameraController.h"
#include <InputManager.h>
#include <GameCameraDefinition.h>
#include <SpawnInfo.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <SystemSettings.h>
#include <CameraSettings.h>
#include <KeyBindings.h>
#include <RenderSystemManager.h>
#include <AudioManager.h>
#include <GameWorld.h>
#include <Minimap.h>

namespace FlagRTS
{
	CameraController::CameraController(Ogre::SceneManager* ogreMgr, Ogre::RenderWindow* window, uint8 vp) :
		_ogreMgr(ogreMgr), 
		_window(window), 
		_viewportNum(vp),
		_activeCamera(0)
	{


		_isFixed = false;
		_isMiddlePressRotating = true;
		_isMiddlePressTranslating = false;
		_isMouseMoveRotating = false;
		_isMouseMoveTransating = true;
		_isScrollZooming = true;

	}

	CameraController::~CameraController()
	{

	}

	void CameraController::Update(float ms)
	{
		auto input = InputManager::Instance();
		float secs = ms/1000.0f;
		float scrollZoomMult = 1.5f - _activeCamera->GetZoomValue();

		if(_isFixed || // If camera is fixed, then no changes occur
			::GetForegroundWindow() != (HWND)RenderSystemManager::Instance()->GetHwnd()) // Or if its not foreground window
			return;

		if( (_isMiddlePressRotating || _isMiddlePressTranslating)
			&& (input->CheckMouseButtonIsDown(MouseButtons::Middle)) )
		{
			auto move = input->GetMouseScreenPosChange();
			if(_isMiddlePressTranslating)
			{
				MoveCamera(Vector3( 
					move.x * SystemSettings::GetCameraSettings().MouseScrollRate*scrollZoomMult, 0.0f, 0.0f));

				MoveCamera(Vector3( 
					0.0f, move.y * SystemSettings::GetCameraSettings().MouseScrollRate*scrollZoomMult, 0.0f));		
			}
			else
			{

			}
		}
		else if(_isMouseMoveRotating)
		{
			auto move = input->GetMouseScreenPosChange();
			PitchCamera( Radian(move.x * SystemSettings::GetCameraSettings().MousePitchRate) );
			RollCamera( Radian(move.y * SystemSettings::GetCameraSettings().MouseYawRate) );
		}
		else if(_isMouseMoveTransating)
		{
			auto mpos = input->GetMouseScreenPos();
			// In windowed mode, move only if mouse is inside window
			if (RenderSystemManager::Instance()->IsFullscreen() ||
				InputManager::Instance()->IsMouseWithinClient())
			{
				if(mpos.x < 0.01f && mpos.x > -0.02f )
				{
					MoveCamera(Vector3( 
						-secs*SystemSettings::GetCameraSettings().EdgeScrollRate*scrollZoomMult, 0.0f, 0.0f));		
				}
				else if(mpos.x > 0.99f && mpos.x < 1.02f )
				{
					MoveCamera(Vector3( 
						secs*SystemSettings::GetCameraSettings().EdgeScrollRate*scrollZoomMult, 0.0f, 0.0f));	
				}
				if(mpos.y <  0.01f && mpos.y > -0.02f )
				{
					MoveCamera(Vector3( 
						0.0f, 0.0f, -secs*SystemSettings::GetCameraSettings().EdgeScrollRate*scrollZoomMult));	
				}
				else if(mpos.y > 0.99f && mpos.y < 1.02f )
				{
					MoveCamera(Vector3( 
						0.0f, 0.0f, secs*SystemSettings::GetCameraSettings().EdgeScrollRate*scrollZoomMult));	
				}
			}
		}

		if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraCenter]))
		{
			Quaternion q;
			q.FromAngleAxis(Radian(Degree(90)), Vector3(-1.0f, 0.0f, 0.0f));
			SetCameraOrientation(q);

			SetCameraPosition(Vector3(400,800,400));
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraLeft]))
		{
			MoveCamera(Vector3( 
				-secs*SystemSettings::GetCameraSettings().KeyScrollRate, 0.0f, 0.0f));	
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraRight]))
		{
			MoveCamera(Vector3( 
				secs*SystemSettings::GetCameraSettings().KeyScrollRate, 0.0f, 0.0f));	
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraUp]))
		{
			MoveCamera(Vector3( 
				0.0f, 0.0f, -secs*SystemSettings::GetCameraSettings().KeyScrollRate ));	
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraDown]))
		{
			MoveCamera(Vector3( 
				0.0f, 0.0f, secs*SystemSettings::GetCameraSettings().KeyScrollRate));	
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraPitchUp]))
		{
			Quaternion q;
			q.FromAngleAxis(Radian(secs*SystemSettings::GetCameraSettings().KeyPitchRate), Vector3(1.0f, 0.0f, 0.0f));
			RotateCamera(q);
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraPitchDown]))
		{
			Quaternion q;
			q.FromAngleAxis(Radian(secs*SystemSettings::GetCameraSettings().KeyPitchRate), Vector3(-1.0f, 0.0f, 0.0f));
			RotateCamera(q);
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraYawLeft]))
		{
			Quaternion q;
			q.FromAngleAxis(Radian(secs*SystemSettings::GetCameraSettings().KeyYawRate), Vector3(0.0f, 1.0f, 0.0f));
			_activeCamera->Rotate( q, Ogre::Node::TS_WORLD );
		}
		else if(input->CheckKeyIsDown(SystemSettings::GetKeyBindings().
			KeyActionMap[KeyActions::KA_CameraYawRight]))
		{
			Quaternion q;
			q.FromAngleAxis(Radian(secs*SystemSettings::GetCameraSettings().KeyYawRate), Vector3(0.0f, -1.0f, 0.0f));
			_activeCamera->Rotate( q, Ogre::Node::TS_WORLD );
		}

		if(_isScrollZooming)
		{
			float scroll = input->GetInputState()->GetScrollChange();
			if(scroll > 1e-3 || scroll < -1e-3)
				_activeCamera->Zoom(scroll * SystemSettings::GetCameraSettings().ZoomRate);
		}
	}

	void CameraController::BeginCameraRender()
	{
		_window->removeViewport(_viewportNum);
		// Create and spawn fake camera
		GameCameraDefinition* dummyDef(xNew0(GameCameraDefinition));
		RefPtr<SpawnInfo> si(xNew0(SpawnInfo));
		si->SetSpawnPosition(Vector3(400,800,400));
		// With no rotation global camera direction = (0,0,-1)
		Quaternion q;
		q.FromAngleAxis(Radian(Degree(90)), Vector3(-1.0f, 0.0f, 0.0f));
		si->SetSpawnOrentation(q);

		GameCamera* camera(xNew1(GameCamera,dummyDef));

		camera->LoadResources(_ogreMgr);
		SceneNode* camNode = _ogreMgr->getRootSceneNode()->createChildSceneNode();
		camera->SetSceneNode(camNode);
		camera->SetOrientation(q);
		camera->Spawn();
		camera->SetGlobalYaw(false);
		camera->SetPosition(si->GetSpawnPosition());
		SetActiveCamera(camera);

		Media::AudioManager::Instance()->SetListenerNode(camNode);
	}

	void CameraController::EndCameraRender()
	{
		GameWorld::GlobalWorld->GetMinimap()->SetActiveCamera(0);
		GameWorld::GlobalWorld->GetMinimap()->SetDrawCameraBounds(false);

		_window->removeViewport(_viewportNum);
		Media::AudioManager::Instance()->SetListenerNode(0);
	}

	void CameraController::SetActiveCamera(GameCamera* camera)
	{
		if(_activeCamera != 0)
			_activeCamera->DetachCameraFromWindow(_window);
		_activeCamera = camera;
		_activeCamera->AttachCameraToWindow(_window, _viewportNum);

		GameWorld::GlobalWorld->GetMinimap()->SetActiveCamera(camera);
		GameWorld::GlobalWorld->GetMinimap()->SetDrawCameraBounds(true);

		_cameraChanged.Fire(camera);
	}

}