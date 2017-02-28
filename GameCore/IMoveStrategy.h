#pragma once

#include "ObjectDefinition.h"

namespace FlagRTS
{
	class Unit;

	// Moves/rotates unit in order to reach the target
	// Each startegy should be global, as they don't track unit's state ( and don't need to ),
	// only some results of last call, so it inherits from ObjectDefinition and can be retrieved from DefCreator
	// Unlike IMover they don't specify path, but only a way to reach given target
	class IMoveStrategy : public ObjectDefinition
	{
	protected:
		float _remSqDistance;
		float _remAngle;

	public:
		IMoveStrategy() 
		{ }

		IMoveStrategy(XmlNode* xml) :
			ObjectDefinition(xml)
		{ }

		virtual void Move(
			Unit* movedUnit,
			const Vector3& target,
			const float ms) = 0;

		void ResetRemainingDistance()
		{
			_remSqDistance = 1e18f;
			_remAngle = 1e6f;
		}

		// Returns distance remaining to target ( from last call to Move() )
		float GetRemainingSquaredDistance()
		{
			return _remSqDistance;
		}

		// Returns angle remaining to face target in radians ( from last call to Move() )
		float GetRemainingAngle()
		{
			return _remAngle;
		}
	};
}