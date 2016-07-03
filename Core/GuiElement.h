#pragma once

#include "TypeDefs.h"
#include "Pointers.h"
#include "Event.h"

namespace MyGUI
{
	class Widget;
}

namespace FlagRTS
{
	namespace Media
	{
		class SoundSource;
	}

	class GUI;
	class Layout;

	namespace GameGui
	{
		class Tooltip;

		// Base for gui elements -> they can be registered to GUI for updates
		// Wrappers around one MyGUI widget, extends a bit its funcinality
		class GuiElement
		{
		protected:
			MyGUI::Widget* _widget;
			Media::SoundSource* _hoverSound;
			Media::SoundSource* _pressSound;
			Media::SoundSource* _clickSound;
			
			Tooltip* _tooltip;
			float _tooltipTimer;
			float _tooltipShowDelay;
			float _tooltipHideDelay;
			bool _tooltipVisible;
			bool _tooltipEnabled;

			bool _isVisible;
			bool _isEnabled;
			bool _isMouseHover;
			bool _isLeftPressed;
			bool _isRightPressed;
			
			bool _cursorEnabled;
			string _customCursor;
			string _lastCursor;

			Event<GuiElement*, int,int> _mouseHoverStart;
			Event<GuiElement*, int,int> _mouseHoverEnd;
			Event<GuiElement*, int> _mousePressed;
			Event<GuiElement*, int> _mouseReleased;
			Event<GuiElement*, int> _mouseClicked;

		public:
			// Sets widget for this element
			// If widget have UserStrings with keys ClickSoundName and ClickSoundPack, then click sound
			// is set here ( same for HoverSound, PressSound )
			// If have CustomCursor UserString it will be set and custom cursor enabled
			GuiElement(MyGUI::Widget* widget);
			// Loads MyGUI Widget from GUI
			// If widget have UserStrings with keys ClickSoundName and ClickSoundPack, then click sound
			// is set here ( same for HoverSound, PressSound )
			// If have CustomCursor UserString it will be set and custom cursor enabled
			GuiElement(RefPtr<GUI>& gui, RefPtr<Layout>& layout, const string& widgetName);
			~GuiElement();

			virtual void Show();
			virtual void Hide();
			bool IsVisible() const { return _isVisible; }
			
			virtual void Enable();
			virtual void Disable();
			bool IsEnabled() const { return _isEnabled; }

			virtual void Update(float ms);

			template<typename T = MyGUI::Widget>
			T* GetMyGUIWidget() const { return static_cast<T*>(_widget); }

			// Updates size of widget ratio times
			// Also moves widget proportinaly
			virtual void Resize(float ratioHorizontal, float ratioVertical);
			// Updates absolute size of widget
			virtual void Resize(int height, int width);
			// Updates position of widget relative to parent
			virtual void Move(int parentLeft, int parentTop);

			// Sets tooltip and enables it if tooltip != 0
			void SetTooltip(Tooltip* tooltip) { _tooltip = tooltip; }
			Tooltip* GetTooltip() const { return _tooltip; }

			// Enables showing of tooltip after hover
			void EnableTooltip();
			// Disbales tooltip
			void DisableTooltip();
			// Sets delay in showing/hiding tooltip after
			void SetTooltipDuration(float showDelay, float hideDelay)
			{
				_tooltipHideDelay = hideDelay;
				_tooltipShowDelay = showDelay;
			}

			void ShowTooltip();
			void HideTooltip();

			// Enables changing of cursor when mouse hovers over Widget
			void EnableCustomCursor() { _cursorEnabled = true; }
			// Disables changing of cursor when mouse hovers over Widget
			void DisableCustomCursor() { _cursorEnabled = false; }

			bool IsCustomCursorEnabled() const { return _cursorEnabled; }
			// Sets widget custom cursor name ( context remains same )
			void SetCustomCursorName(const string& name)
			{
				_customCursor = name;
			}
			
			const string& GetCustomCursorName() { return _customCursor; }

			void SetHoverSound(const string& name, const string& pack);
			void SetPressSound(const string& name, const string& pack);
			void SetClickSound(const string& name, const string& pack);
		
			// Occurs when mouse enters area over widget
			// Params are this and mouse (x,y) in widget coords
			Event<GuiElement*, int,int>& GetMouseHoverStartedEvent() { return _mouseHoverStart; }
			// Occurs when mouse enters area over widget
			// Params are this ans mouse (x,y) in widget coords
			Event<GuiElement*, int,int>& GetMouseHoverEndedEvent() { return _mouseHoverEnd; }
			// Occurs when mouse button is pressed over widget
			// Param are this and button number
			Event<GuiElement*, int>& GetMousePressedEvent() { return _mousePressed; }
			// Occurs when mouse button is released over widget
			// Param are this and button number
			Event<GuiElement*, int>& GetMouseReleasedEvent() { return _mouseReleased; }
			// Occurs when mouse button is pressed and released over widget
			// Param are this and button number
			Event<GuiElement*, int>& GetMouseClickedEvent() { return _mouseClicked; }

			template<typename T>
			void SetUserData(T data)
			{
				_widget->setUserData(MyGUI::Any(data));
			}

			template<typename T>
			T* GetUserData()
			{
				return _widget->getUserData<T>();
			}

		protected:
			void CheckHover(int mx, int my);
			void CheckPressed();
		};
	}
}