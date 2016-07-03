#include "GuiElement.h"
#include <MyGUI\MyGUI_Button.h>
#include "AudioManager.h"
#include "GUI.h"
#include "SoundSource.h"
#include "SoundChannel.h"
#include "InputManager.h"
#include "CursorManager.h"

namespace FlagRTS
{
	using namespace GameGui;

	GuiElement::GuiElement(MyGUI::Widget* widget) :
		_widget(widget),
		_hoverSound(0),
		_pressSound(0),
		_clickSound(0),
		_tooltip(0),
		_tooltipVisible(false),
		_tooltipEnabled(false),
		_tooltipTimer(0),
		_tooltipShowDelay(1.f),
		_tooltipHideDelay(0.f),
		_isLeftPressed(false),
		_isRightPressed(false),
		_isMouseHover(false),
		_cursorEnabled(false),
		_customCursor("Default")
	{
		_isEnabled = _widget->isEnabled();
		_isVisible = _widget->isVisible();

		string hoverSoundName = _widget->getUserString("HoverSoundName");
		string hoverSoundPack = _widget->getUserString("HoverSoundPack");
		string pressSoundName = _widget->getUserString("PressSoundName");
		string pressSoundPack = _widget->getUserString("PressSoundPack");
		string clickSoundName = _widget->getUserString("ClickSoundName");
		string clickSoundPack = _widget->getUserString("ClickSoundPack");
		string cursor = _widget->getUserString("CustomCursor");

		if( hoverSoundName.size() > 0 && hoverSoundPack.size() > 0)
		{
			SetHoverSound(hoverSoundName, hoverSoundPack);
		}
		if( pressSoundName.size() > 0 && pressSoundPack.size() > 0)
		{
			SetPressSound(pressSoundName, pressSoundPack);
		}
		if( clickSoundName.size() > 0 && clickSoundPack.size() > 0)
		{
			SetClickSound(clickSoundName, clickSoundPack);
		}
		if( cursor.size() > 0)
		{
			SetCustomCursorName(cursor);
			EnableCustomCursor();
		}
	}

	GuiElement::GuiElement(RefPtr<GUI>& gui, RefPtr<Layout>& layout, const string& widgetName) :
		_widget(0),
		_hoverSound(0),
		_pressSound(0),
		_clickSound(0),
		_tooltip(0),
		_tooltipVisible(false),
		_tooltipEnabled(false),
		_tooltipTimer(0),
		_tooltipShowDelay(1.f),
		_tooltipHideDelay(0.f),
		_isLeftPressed(false),
		_isRightPressed(false),
		_isMouseHover(false),
		_cursorEnabled(false),
		_customCursor("Default")
	{
		_widget = gui->FindInLayout(layout, widgetName);
		_isEnabled = _widget->isEnabled();
		_isVisible = _widget->isVisible();

		string hoverSoundName = _widget->getUserString("HoverSoundName");
		string hoverSoundPack = _widget->getUserString("HoverSoundPack");
		string pressSoundName = _widget->getUserString("PressSoundName");
		string pressSoundPack = _widget->getUserString("PressSoundPack");
		string clickSoundName = _widget->getUserString("ClickSoundName");
		string clickSoundPack = _widget->getUserString("ClickSoundPack");
		string cursor = _widget->getUserString("CustomCursor");

		if( hoverSoundName.size() > 0 && hoverSoundPack.size() > 0)
		{
			SetHoverSound(hoverSoundName, hoverSoundPack);
		}
		if( pressSoundName.size() > 0 && pressSoundPack.size() > 0)
		{
			SetPressSound(pressSoundName, pressSoundPack);
		}
		if( clickSoundName.size() > 0 && clickSoundPack.size() > 0)
		{
			SetClickSound(clickSoundName, clickSoundPack);
		}
		if( cursor.size() > 0)
		{
			SetCustomCursorName(cursor);
			EnableCustomCursor();
		}
	}

	GuiElement::~GuiElement()
	{

	}

	void GuiElement::Update(float ms)
	{
		auto& mousePos = InputManager::Instance()->GetMouseScreenPosAbs();
		CheckHover((int)mousePos.x, (int)mousePos.y);
		if( _isEnabled )
		{
			CheckPressed();
		}

		// Check tooltip timer
		if( _tooltipEnabled && _tooltipVisible && 
			!_isMouseHover && _tooltipTimer <= 0.f )
		{
			HideTooltip();
		}
		else if( _tooltipEnabled &&	!_tooltipVisible &&
			_isMouseHover && _tooltipTimer <= 0.f )
		{
			ShowTooltip();
		}

		_tooltipTimer -= ms;
	}

	void GuiElement::Show()
	{
		_isVisible = true;
		_widget->setVisible(true);
	}

	void GuiElement::Hide()
	{
		_isVisible = false;
		_widget->setVisible(false);
	}

	void GuiElement::Enable()
	{
		_isEnabled = true;
		_widget->setEnabled(true);
	}

	void GuiElement::Disable()
	{
		_isEnabled = false;
		_widget->setEnabled(false);
	}

	void GuiElement::Resize(float ratioHorizontal, float ratioVertical)
	{
		const MyGUI::IntCoord& oldCoord = _widget->getCoord();
		_widget->setCoord(
			(int)((float)oldCoord.left * ratioHorizontal), (int)((float)oldCoord.top * ratioVertical), 
			(int)((float)oldCoord.width * ratioHorizontal), (int)((float)oldCoord.height * ratioVertical));
	}

	void GuiElement::Resize(int height, int width)
	{
		_widget->setSize(height, width);
	}

	void GuiElement::Move(int parentLeft, int parentTop)
	{
		_widget->setPosition(parentLeft, parentTop);
	}

	void GuiElement::EnableTooltip()
	{
		_tooltipEnabled = true;
		if( _isMouseHover )
		{
			_tooltipTimer = _tooltipShowDelay;
		}
	}

	void GuiElement::DisableTooltip()
	{
		_tooltipEnabled = false;
		HideTooltip();
	}

	void GuiElement::ShowTooltip()
	{
		if(_tooltip != 0)
		{

			_tooltipVisible = true;
		}
	}

	void GuiElement::HideTooltip()
	{
		if(_tooltip != 0)
		{

			_tooltipVisible = false;
		}
	}

	void GuiElement::SetHoverSound(const string& name, const string& pack)
	{
		if(_hoverSound != 0)
			Media::AudioManager::Instance()->DestroySoundSource(_hoverSound);

		_hoverSound = Media::AudioManager::Instance()->CreateSoundSource(name, pack);
	}

	void GuiElement::SetPressSound(const string& name, const string& pack)
	{
		if(_pressSound != 0)
			Media::AudioManager::Instance()->DestroySoundSource(_pressSound);

		_pressSound = Media::AudioManager::Instance()->CreateSoundSource(name, pack);
	}

	void GuiElement::SetClickSound(const string& name, const string& pack)
	{
		if(_pressSound != 0)
			Media::AudioManager::Instance()->DestroySoundSource(_clickSound);

		_clickSound = Media::AudioManager::Instance()->CreateSoundSource(name, pack);
	}

	void GuiElement::CheckHover(int mx, int my)
	{
		auto widgetCoords = _widget->getAbsoluteCoord();

		// Check if mouse is within widget bounds
		bool hover = !(widgetCoords.left > mx ||
			widgetCoords.right() < mx ||
			widgetCoords.top > my ||
			widgetCoords.bottom() < my);

		if( hover && !_isMouseHover )
		{
			// Hover begins
			// Play hover sound
			if(_hoverSound != 0)
			{
				auto ch1 = Media::AudioManager::Instance()->
					GetChannel( Media::SoundChannel::CH_Gui );
				ch1->Play( _hoverSound, true );
			}
			// Change cursor
			if(_cursorEnabled)
			{
				_lastCursor = CursorManager::Instance()->GetCurrentCursorState();
				CursorManager::Instance()->SetCurrentCursorState(_customCursor);
			}
			// Start tooltip timer
			if( !_tooltipVisible )
				_tooltipTimer = _tooltipShowDelay;

			_mouseHoverStart.Fire(this, mx - widgetCoords.left, my - widgetCoords.top);
		}
		else if( !hover && _isMouseHover )
		{
			// Hover ends
			// Change cursor
			if(_cursorEnabled)
			{
				CursorManager::Instance()->SetCurrentCursorState(_lastCursor);
			}
			// Start tooltip timer
			if( _tooltipVisible )
				_tooltipTimer = _tooltipHideDelay;

			_mouseHoverEnd.Fire(this, mx - widgetCoords.left, my - widgetCoords.top);
		}

		_isMouseHover = hover;
	}

	void GuiElement::CheckPressed()
	{
		if( _isMouseHover )
		{
			if( InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Right) )
			{
				_isRightPressed = true;

				if(_pressSound != 0)
				{
					auto ch1 = Media::AudioManager::Instance()->
						GetChannel( Media::SoundChannel::CH_Gui );
					ch1->Play( _pressSound, true );
				}

				_mousePressed.Fire(this, MouseButtons::Right);
			}
			else if( InputManager::Instance()->CheckMouseWasReleased(MouseButtons::Right) )
			{
				bool oldPressed = _isRightPressed;
				_isRightPressed = false;
				_mouseReleased.Fire(this, MouseButtons::Right);
				if(oldPressed == true)
				{
					if(_clickSound != 0)
					{
						auto ch1 = Media::AudioManager::Instance()->
							GetChannel( Media::SoundChannel::CH_Gui );
						ch1->Play( _clickSound, true );
					}

					_mouseClicked.Fire(this, MouseButtons::Right);
				}

			}

			if( InputManager::Instance()->CheckMouseWasPressed(MouseButtons::Left) )
			{
				_isLeftPressed = true;

				if(_pressSound != 0)
				{
					auto ch1 = Media::AudioManager::Instance()->
						GetChannel( Media::SoundChannel::CH_Gui );
					ch1->Play( _pressSound, true );
				}

				_mousePressed.Fire(this, MouseButtons::Left);
			}
			else if( InputManager::Instance()->CheckMouseWasReleased(MouseButtons::Left) )
			{
				bool oldPressed = _isLeftPressed;
				_isLeftPressed = false;
				_mouseReleased.Fire(this, MouseButtons::Left);
				if(oldPressed == true)
				{
					if(_clickSound != 0)
					{
						auto ch1 = Media::AudioManager::Instance()->
							GetChannel( Media::SoundChannel::CH_Gui );
						ch1->Play( _clickSound, true );
					}

					_mouseClicked.Fire(this, MouseButtons::Left);
				}
			}
		}
		else
		{
			_isRightPressed = false;
			_isLeftPressed = false;
		}
	}

}
