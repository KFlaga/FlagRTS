#pragma once

#include <string>

namespace FlagRTS
{
	namespace ArmorType
	{
		enum AT
		{
			None = 0,
			Light,
			Heavy,
			Magical,
			Hero,
			Fortified
		};

		AT Parse(const std::string& atString);
	}

	namespace SizeType
	{
		enum ST
		{
			Small = 0,
			Large,
			Massive,
			Building
		};

		ST Parse(const std::string& stString);
	}

	namespace BodyType
	{
		enum BT
		{
			Biological = 0,
			Mechanical,
			Magical,
			Etheral,
			Undead,
			Building
		};

		BT Parse(const std::string& fvString);
	}
}