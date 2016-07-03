#pragma once

#include <Array.h>

namespace FlagRTS
{
	namespace Media
	{
		class Sound;
		class SoundSource;
	}

	typedef Array<Media::SoundSource*> UnitSoundSet;

	// Set of default Unit sounds. Each unit may have custom sound set directly in
	// definition file w/o needs of additional event. If custom is not specified
	// standard one is used. Most sounds have few versions - one is choosen randomly
	// If only one is set as custom it will be set for every version
	namespace UnitSounds
	{
		enum SoundAction : int
		{
			Invalid = -1,
			OnSelected1 = 0,
			OnSelected2,
			OnSelected3,
			OnMoveCommand1,
			OnMoveCommand2,
			OnMoveCommand3,
			OnAttackCommand1,
			OnAttackCommand2,
			OnAttackCommand3,
			OnOtherCommand1,
			OnOtherCommand2,
			OnOtherCommand3,
			OnEnemySpotted,
			OnConstructed1,
			OnConstructed2,
			OnDied1,
			OnDied2,

			DefSoundActionsCount
		};

		Media::Sound* GetDefaultSound(SoundAction action);
		SoundAction ParseSoundAction(const char* actionName);
	}

}