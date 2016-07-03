#include "UnitProperties.h"

namespace FlagRTS
{
	namespace ArmorType
	{
		AT Parse(const std::string& atString)
		{
			if(atString.compare("Light") == 0)
			{
				return Light;
			}
			else if(atString.compare("Heavy") == 0)
			{
				return Heavy;
			}
			else if(atString.compare("Magical") == 0)
			{
				return Magical;
			}
			else if(atString.compare("Hero") == 0)
			{
				return Hero;
			}
			else if(atString.compare("Fortified") == 0)
			{
				return Fortified;
			}
			else
			{
				return None;
			}
		}
	}

	namespace SizeType
	{
		ST Parse(const std::string& fvString)
		{
			if(fvString.compare("Building") == 0)
			{
				return Building;
			}
			else if(fvString.compare("Large") == 0)
			{
				return Large;
			}
			else if(fvString.compare("Massive") == 0)
			{
				return Massive;
			}
			else
			{
				return Small;
			}
		}
	}

	namespace BodyType
	{

		BT Parse(const std::string& fvString)
		{
			if(fvString.compare("Building") == 0)
			{
				return Building;
			}
			else if(fvString.compare("Mechanical") == 0)
			{
				return Mechanical;
			}
			else if(fvString.compare("Magical") == 0)
			{
				return Magical;
			}
			else if(fvString.compare("Etheral") == 0)
			{
				return Etheral;
			}
			else if(fvString.compare("Undead") == 0)
			{
				return Undead;
			}
			else
			{
				return Biological;
			}
		}
	}
}