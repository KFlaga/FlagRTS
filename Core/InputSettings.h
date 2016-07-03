#pragma once

#include "TypeDefs.h"

namespace FlagRTS
{
	class InputSettings
	{
	public:
		float ClickInterval; // If press/release occurs within interval its click
		float DoubleClickInterval; // If press/release occurs within interval after one click its double-click
		float MouseSensivity;

		InputSettings() :
			ClickInterval(1000.0f),
			DoubleClickInterval(1000.0f),
			MouseSensivity(2.0f)
		{

		}
	};
}