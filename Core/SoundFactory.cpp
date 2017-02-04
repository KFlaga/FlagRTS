#include "SoundFactory.h"
#include "LogManager.h"
#include "Memory.h"
#include "OggSound.h"
#include "WavSound.h"
#include "Exception.h"
#include "SoundPlayers.h"

namespace FlagRTS
{
	using namespace Media;

	SoundFactory::SoundFactory()
	{

	}

	SoundFactory::~SoundFactory()
	{

	}

	Sound* SoundFactory::Create(XmlNode* soundNode)
	{
		Sound* sound = 0;

		char* name = soundNode->first_attribute("Name", 4, false)->value();
		char* pack = soundNode->first_attribute("Pack", 4, false)->value();
		char* file = soundNode->first_attribute("File", 4, false)->value();
		char* extName = soundNode->first_attribute("Ext", 3, false)->value();
		if( strcmp(extName, "Ogg") == 0 )
		{
			sound = xNew3(OggSound,name, pack, file);
		}
		else if( strcmp(extName, "Wav") == 0 )
		{
			sound = xNew3(WavSound,name, pack, file);
		}
		else
			CastException_d((string("Bad sound extension: ") + extName).c_str());

		return sound;
	}

	SoundPlayerFactory::SoundPlayerFactory()
	{

	}

	SoundPlayerFactory::~SoundPlayerFactory()
	{

	}

	ISoundPlayer* SoundPlayerFactory::Create(XmlNode* soundNode)
	{
		ISoundPlayer* soundplayer = 0;

		char* playerName = soundNode->first_attribute("Player", 6, false)->value();
		if( strcmp(playerName, "Single") == 0 )
		{
			soundplayer = xNew0(SingleSoundPlayer);
		}
		else if( strcmp(playerName, "SingleNoStop") == 0 )
		{
			soundplayer = xNew0(SingleNoStopSoundPlayer);
		}
		else if( strcmp(playerName, "Music") == 0 )
		{
			soundplayer = xNew0(MusicSoundPlayer);
		}
		else if( strcmp(playerName, "Revolver") == 0 )
		{
			// Revolver sound needs extra attribute -> max_plays
			int maxPlays = XmlUtility::XmlGetIntIfExists(soundNode, "MaxPlays", 1);
			soundplayer = xNew1(RevolverSoundPlayer, maxPlays);
		}
		else if( strcmp(playerName, "None") == 0 )
		{
			soundplayer = 0;
		}
		else
		{
			CastException_d((string("Bad sound player type: ") + playerName).c_str());
		}

		return soundplayer;
	}
}