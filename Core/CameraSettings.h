#pragma once

#include "TypeDefs.h"

namespace FlagRTS
{
#define _CS_PI_ 3.1416f

	class CameraSettings
	{
	public:
		float KeyScrollRate; // Speed of camera movement using arrow keys ( in world units per second )
		float KeyPitchRate; // Speed of changing pitch by using keys ( in radian per sec )
		float KeyYawRate; // Speed of changing yaw by using keys ( in radian per sec )
		float MouseScrollRate; // Speed of camera movement using middle mouse button ( if applicable ) ( in world units per screen moved )
		float EdgeScrollRate; // Speed of camera movement when mouse near edge of screen ( in world units per second )
		float MousePitchRate; // Speed of changing pitch by using mouse ( if applicable ) ( in radian per screen ) 
		float MouseYawRate; // Speed of changing pitch by using mouse ( if applicable ) ( in radian per screen )
		float ZoomRate;

		// Sets default values
		CameraSettings() :
			KeyScrollRate(1200.0f),
			KeyPitchRate(_CS_PI_ / 6.f ),
			KeyYawRate(_CS_PI_ / 3.f),
			MouseScrollRate(1000.0f),
			EdgeScrollRate(400.0f),
			MousePitchRate(_CS_PI_ / 3.0f),
			MouseYawRate(_CS_PI_ / 2.0f ),
			ZoomRate(1.f / 1200.f)
		{ }
	};
}