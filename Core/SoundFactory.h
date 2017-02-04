#pragma once

#include "Sound.h"
#include "ISoundPlayer.h"
#include "Xml.h"

namespace FlagRTS
{
	namespace Media
	{
		// Sound factory creates various sound extensions based on given Xml
		class SoundFactory
		{
		public:
			SoundFactory();
			~SoundFactory();

			Sound* Create(XmlNode* soundNode);
		};

		// Creates sound players with type read from Xml
		class SoundPlayerFactory
		{
		public:
			SoundPlayerFactory();
			~SoundPlayerFactory();

			ISoundPlayer* Create(XmlNode* soundNode);
		};
	}
}