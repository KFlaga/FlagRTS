#pragma once

namespace FlagRTS
{
	namespace KillReasons
	{
		enum KillReasonType : int
		{
			Unknown = 0,
			Vanished, // Killed with no damage source
			Aborted, // Aborted construction
			SelfDestruct,
			FriendlyFire,
			KilledByPlayer,

			KilledByPlayer0 = KilledByPlayer + 0,
			KilledByPlayer1 = KilledByPlayer + 1,
			KilledByPlayer2 = KilledByPlayer + 2,
			KilledByPlayer3 = KilledByPlayer + 3,
			KilledByPlayer4 = KilledByPlayer + 4,
			KilledByPlayer5 = KilledByPlayer + 5,
			KilledByPlayer6 = KilledByPlayer + 6,
			KilledByPlayer7 = KilledByPlayer + 7,
		};

		inline KillReasonType GetKilledByPlayerReason(int player)
		{
			return (KillReasonType)(KilledByPlayer + player);
		}
	
		KillReasonType ParseKillReason(const char* reason);
	}

}