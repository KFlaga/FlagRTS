#pragma once

#include "Sound.h"
#include "ISoundPlayer.h"
#include "Xml.h"
#include "IFactory.h"

namespace FlagRTS
{
	namespace Media
	{
		// Sound factory creates various sound extensions based on given Xml
		class SoundFactory : public IFactoryCast<Sound, XmlNode*>
		{
		public:
			SoundFactory();
			~SoundFactory();

			IGameObject* Create(XmlNode* soundNode);
		};

		// Creates sound players with type read from Xml
		class SoundPlayerFactory : public IFactoryCast<ISoundPlayer, XmlNode*>
		{
		public:
			SoundPlayerFactory();
			~SoundPlayerFactory();

			IGameObject* Create(XmlNode* soundNode);
		};
	}
}