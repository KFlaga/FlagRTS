#include "SceneObjectProperties.h"

namespace FlagRTS
{
	namespace FogVisibility
	{
		FogVisibility Parse(const std::string& fvString)
		{
			if(fvString.compare("Visible") == 0)
			{
				return Visible;
			}
			else if(fvString.compare("Dimmed") == 0)
			{
				return Dimmed;
			}
			else
			{
				return Hidden;
			}
		}
	}

	namespace CollisionType
	{
		CollisionType Parse(const std::string& fvString)
		{
			if(fvString.compare("Ground") == 0)
			{
				return Ground;
			}
			else if(fvString.compare("Air") == 0)
			{
				return Air;
			}
			else if(fvString.compare("GroundAndAir") == 0)
			{
				return GroundAndAir;
			}
			else
			{
				return None;
			}
		}
	}

	namespace PlacementType
	{
		PlacementType Parse(const std::string& fvString)
		{
			if(fvString.compare("Ground") == 0)
			{
				return Ground;
			}
			else if(fvString.compare("Air") == 0)
			{
				return Air;
			}
			else
			{
				return None;
			}
		}
	}
}