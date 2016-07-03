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
	namespace GameGui
	{
		// Tooltip is a customisable panel, that popups over other widget
		// after hovering over them
		// Its widget-contents should be added externally ( it stores and shows
		// only one widget - probably bg panel )
		class Tooltip
		{
		public:
			enum PopupPosition
			{
				OverCursor,



			};

		protected:
			MyGUI::Widget* _widget;

			bool _isVisible;



		public:

		};
	}
}