#include "GameGuiMinimapPanel.h"
#include <MyGUI\MyGUI.h>
#include <ConfigurationManager.h>
#include <GameWorld.h>
#include <Minimap.h>
#include <InputManager.h>

namespace FlagRTS
{
	using namespace GameGui;

	MinimapPanel::MinimapPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout) :
		_isRightPressed(false),
		_isLeftPressed(false),
		_mouseMapPos(0.f,0.f)
	{
		_mainPanel = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "MinimapPanel"));
		_minimap = static_cast<MyGUI::ImageBox*>(gameGui->FindInLayout(mainLayout, "MinimapPanel_Map"));
	}

	MinimapPanel::~MinimapPanel()
	{

	}

	void MinimapPanel::Show()
	{
		auto tex = GameWorld::GlobalWorld->GetMinimap()->GetRenderTexture();
		_minimap->setImageTexture(tex->getName());
		_mainPanel->setVisible(true);
	}

	void MinimapPanel::Hide()
	{
		_mainPanel->setVisible(false);
	}

	void MinimapPanel::Resize( float ratioHorizontal, float ratioVertical )
	{
		const MyGUI::IntCoord& oldCoord = GetPanel()->getCoord();
		GetPanel()->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));

		const MyGUI::IntCoord& mapCoord = GetMinimap()->getCoord();
		GetMinimap()->setCoord(
			(int)((float)mapCoord.left * ratioHorizontal), (int)((float)mapCoord.top * ratioVertical), 
			(int)((float)mapCoord.width * ratioHorizontal), (int)((float)mapCoord.height * ratioVertical));

	}

	void MinimapPanel::Update(float ms)
	{
		if( IsMouseOverMinimap() )
		{
			bool oldLeft = _isLeftPressed;
			bool oldRight = _isRightPressed;
			Vector2 oldPos(_mouseMapPos);

			_isRightPressed = InputManager::Instance()->GetInputState()->
				CheckMouseButtonIsDown(MouseButtons::Right) > 0;
			_isLeftPressed = !_isRightPressed && InputManager::Instance()->GetInputState()->
				CheckMouseButtonIsDown(MouseButtons::Left) > 0;
			_mouseMapPos = Vector2(
				(float)(InputManager::Instance()->GetMouseScreenPosAbs().x - _minimap->getAbsoluteLeft()) / 
				(float)_minimap->getWidth(), 
				(float)(InputManager::Instance()->GetMouseScreenPosAbs().y-_minimap->getAbsoluteTop()) / 
				(float)_minimap->getHeight());

			// Check right was released
			// If mouse pressed/released in same position set is as clicked
			if( oldRight && !_isRightPressed && !_mouseMoved )
			{
				_onRightClicked.Fire(_mouseMapPos);
			}
			else if( oldLeft && !_isLeftPressed && !_mouseMoved )
			{
				_onLeftClicked.Fire(_mouseMapPos);
			}
			else if (!_isRightPressed && !_isLeftPressed )
			{
				// If no button is pressed reset it movement
				_mouseMoved = false;
			}
			else if(oldPos != _mouseMapPos && (oldLeft || oldRight) )
			{
				// Mouse moved with button pressed
				_mouseMoved = true;
				if(_isRightPressed)
				{
					_onRightDragged.Fire(_mouseMapPos);
				}
				else if(_isLeftPressed)
				{
					_onLeftDragged.Fire(_mouseMapPos);
				}
			}

		}
		else
		{
			_isRightPressed = false;
			_isLeftPressed = false;
		}
	}

	Vector2 MinimapPanel::GetMousePosInWorldCoords() const
	{
		return GameWorld::GlobalWorld->GetMinimap()->ConvertMinimapToWorldCoords(_mouseMapPos);
	}

	bool MinimapPanel::IsMouseOverMinimap() const
	{
		Vector2 mousePos = InputManager::Instance()->GetMouseScreenPosAbs();
		auto mapCoords = _minimap->getAbsoluteCoord();
		return !(mousePos.x < mapCoords.left ||
			mousePos.x > mapCoords.right() ||
			mousePos.y < mapCoords.top ||
			mousePos.y > mapCoords.bottom());
	}
}