#pragma once

namespace FlagRTS
{
	namespace SelectionFlags
	{
		// Set in SceneObjects to determine which ones should be selected ( also works for hover )
		// They are set for Ogre MovableObjects query flags
		enum SelectionFlags : unsigned int
		{
			None = 0,
			Selectable = 1 << 1, // This must be set for the unit to be selected
			Hoverable = 1 << 0, // This must be set for the object to accept hover
			// Bits [2-6] are for object type
			Unit = 1 << 2,
			MapDecorator = 1 << 3,
			InteractableMarker = 1 << 4,
			Other = 1 << 6,
			// Bits [7-14] are for unit class
			Military = 1 << 7, // Ordinary unit
			MilitaryUnit = Military | Unit,
			Worker = 1 << 8,  // Unit is worker / resource transporter etc.
			WorkerUnit = Worker | Unit,
			Building = 1 << 9, // Unit is building
			BuildingUnit = Building | Unit,
			Resource = 1 << 10, // Unit is resource
			ResourceUnit = Resource | Unit,

			// Bits [15-26] are for owning player
			PlayerShift = 15,
			Player0 = 1 << PlayerShift,
			Player1 = 1 << (PlayerShift+1),
			Player2 = 1 << (PlayerShift+2),
			Player3 = 1 << (PlayerShift+3),
			Player4 = 1 << (PlayerShift+4),
			Player5 = 1 << (PlayerShift+5),
			Player6 = 1 << (PlayerShift+6),
			Player7 = 1 << (PlayerShift+7),
			PlayerNeutral = 1 << (PlayerShift+8),
			PlayerAny = Player0 | Player1 | Player2 | Player3 | Player4 | Player5 | Player6 | Player7 | PlayerNeutral,
			PlayerNotNeutral = Player0 | Player1 | Player2 | Player3 | Player4 | Player5 | Player6 | Player7,
		};

		inline SelectionFlags GetPlayerFlag(unsigned int num)
		{
			return (SelectionFlags)(1 << (PlayerShift + num));
		}
	}
}