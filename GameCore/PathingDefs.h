#pragma once

#include "DataTypes.h"

namespace FlagRTS
{
	namespace Axes
	{
		enum Axis : int
		{
			X = 0,
			Y,
			Z
		};
	}
}

#define PI 3.14159f
#define TWO_PI 6.28319f
#define PI_SQUARED 9.86960f
// Max error between to float values ( in vertices ) for them to be
// considered equal. Value relative to actual scene object sizes ( like 0.1% - 1% of small objects )
// Actual shapes are enlarged by this value in checks
#define VERTEX_ERROR 0.01f
#define VERTEX_ERROR_NEGATIVE -0.01f
// Vertex error for squared values ( squared lengths, squared distances, areas )
#define VERTEX_ERROR_SQUARED 1.0f
#define VERTEX_ERROR_SQUARED_NEGATIVE -1.0f
// Max relative error for value to be considered 0
#define RELATIVE_ERROR 1e-6f
#define RELATIVE_ERROR_NEGATIVE -1e-6f
#define IsCloseToZero(val) val < RELATIVE_ERROR && val > RELATIVE_ERROR_NEGATIVE
#define IsCloseToZeroV(val) val < VERTEX_ERROR && val > VERTEX_ERROR_NEGATIVE
#define IsCloseToZeroSV(val) val < VERTEX_ERROR_SQUARED && val > VERTEX_ERROR_SQUARED_NEGATIVE
