#include "UnitSoundSet.h"
#include <AudioManager.h>

namespace FlagRTS
{
	using namespace Media;
	using namespace UnitSounds;

	const char* SoundActionNames[] =
	{
		"OnSelected1",
		"OnSelected2",
		"OnSelected3",
		"OnMoveCommand1",
		"OnMoveCommand2",
		"OnMoveCommand3",
		"OnAttackCommand1",
		"OnAttackCommand2",
		"OnAttackCommand3",
		"OnOtherCommand1",
		"OnOtherCommand2",
		"OnOtherCommand3",
		"OnEnemySpotted",
		"OnConstructed1",
		"OnConstructed2",
		"OnDied1",
		"OnDied2"
	};

	const char* DefSoundsNames[] =
	{
		"Def_OnSelected",
		"Def_OnSelected",
		"Def_OnSelected",
		"Def_OnMove",
		"Def_OnMove",
		"Def_OnMove",
		"Def_OnAttack",
		"Def_OnAttack",
		"Def_OnAttack",
		"Def_OnMove",
		"Def_OnMove",
		"Def_OnMove",
		"Def_EnemySpotted",
		"Def_Constructed",
		"Def_Constructed",
		"Def_Died",
		"Def_Died"
	};

	const char* DefSoundPack = "GameDefaults";

	Media::Sound* UnitSounds::GetDefaultSound(SoundAction action)
	{
		return AudioManager::Instance()->FindSound(
			DefSoundsNames[action], DefSoundPack);
	}

	SoundAction UnitSounds::ParseSoundAction(const char* actionName)
	{
		for(int i = 0; i < SoundAction::DefSoundActionsCount; ++i)
		{
			if( strcmp(actionName, SoundActionNames[i]) == 0)
				return (SoundAction)i;
		}
		return SoundAction::Invalid;
	}
}