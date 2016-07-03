#pragma once

#include <DataTypes.h>
#include <Event.h>
#include <GUI.h>
#include <MyGUI\MyGUI_WidgetInput.h>

namespace MyGUI
{
	class Widget;
	class ImageBox;
	class Button;
}

namespace FlagRTS
{
	namespace GameGui
	{
		// Class representing cell 3x4 of buttons, which on press chooses bind command to be ordered
		// ( notifies listener that command was choosen )
		// With change of unit selected unit group panel should be reset and group command buttons
		// should be registered
		class MinimapPanel
		{
		private:
			MyGUI::Widget* _mainPanel;
			MyGUI::ImageBox* _minimap;
			MyGUI::Button* _buttons[3];
			bool _isLeftPressed;
			bool _isRightPressed;
			bool _mouseMoved;
			Vector2 _mouseMapPos;

			Event<Vector2> _onLeftDragged;
			Event<Vector2> _onLeftClicked;
			Event<Vector2> _onRightDragged;
			Event<Vector2> _onRightClicked;

		public:
			MinimapPanel(RefPtr<GUI>& gameGui, RefPtr<Layout>& mainLayout);
			~MinimapPanel();

			MyGUI::Button* GetButton(unsigned short idx) const { return _buttons[idx]; }
			MyGUI::Widget* GetPanel() const { return _mainPanel; }
			MyGUI::ImageBox* GetMinimap() const { return _minimap; }

			void Update(float ms);

			void Show();
			void Hide();

			void Resize( float ratioHorizontal, float ratioVertical );

			// Returns true if mouse is hovering over minimap
			bool IsMouseOverMinimap() const;
			// If mouse is over minimap, returns its position in minimap coords [0-1]
			const Vector2& GetMousePosInMinimapCoords() const
			{
				return _mouseMapPos;
			}
			// If mouse is over minimap, returns its position in game world coords (x,z)
			Vector2 GetMousePosInWorldCoords() const;
			// If mouse is over minimap, returns true if button was pressed in current frame
			bool IsLeftButtonPressedOnMinimap() const
			{
				return _isLeftPressed;
			}
			// If mouse is over minimap, returns true if button is pressed and mouse moved 
			// while it was pressed
			bool IsLeftButtonDraggedOnMinimap() const
			{
				return _isLeftPressed && _mouseMoved;
			}
			// If mouse is over minimap, returns true if button was pressed in current frame
			// Right button have higher priority than left and will override it
			bool IsRightButtonPressedOnMinimap()
			{
				return _isRightPressed;
			}
			// If mouse is over minimap, returns true if button is pressed and mouse moved 
			// while it was pressed
			// Right button have higher priority than left and will override it
			bool IsRightButtonDraggedOnMinimap() const
			{
				return _isRightPressed && _mouseMoved;
			}

			// Event occurs when mouse is moved over minimap with left button pressed
			// Parameter is mouse position in minimap coords
			Event<Vector2>& MinimapLeftButtonDragged()
			{
				return _onLeftDragged;
			}
			// Event occurs when left button is clicked over minimap ( pressed/released in same position )
			// Parameter is mouse position in minimap coords
			Event<Vector2>& MinimapLeftButtonClicked()
			{
				return _onLeftClicked;
			}
			// Event occurs when mouse is moved over minimap with left button pressed
			// Parameter is mouse position in minimap coords
			Event<Vector2>& MinimapRightButtonDragged()
			{
				return _onRightDragged;
			}
			// Event occurs when left button is clicked over minimap ( pressed/released in same position )
			// Parameter is mouse position in minimap coords
			Event<Vector2>& MinimapRightButtonClicked()
			{
				return _onRightClicked;
			}

		private:
			void OnButtonClicked(MyGUI::Widget* button);
		};
	}
}