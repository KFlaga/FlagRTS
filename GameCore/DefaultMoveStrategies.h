#pragma once

#include "IMoveStrategy.h"

namespace FlagRTS
{
	class Unit;

	// Unit cannot move currently
	class NoMoveMoveStrategy : public IMoveStrategy
	{
	public:
		NoMoveMoveStrategy();
		NoMoveMoveStrategy(XmlNode* defNode);

		void Move(
			Unit* movedUnit,
			const Vector3& target,
			const float ms);
	};

	// Most basic moving :
	// - first rotate to target
	// - if rotated, move toward target with constant velocity
	class FaceAndGoMoveStrategy : public IMoveStrategy
	{
		Radian _requriedFaceAngle; // Max angle to target to begin move
		float _yOffset; // If unit is flying choose offset height from terrain level

	public:
		FaceAndGoMoveStrategy();
		FaceAndGoMoveStrategy(XmlNode* defNode);
		~FaceAndGoMoveStrategy();

		void Move(
			Unit* movedUnit,
			const Vector3& target,
			const float ms);

		void SetHeightOffset(const float offset) { _yOffset = offset; }
		float GetHeightOffset() const { return _yOffset; }
		
		void SetMaxFaceAngle(const Radian angle) { _requriedFaceAngle = angle; }
		Radian GetMaxFaceAngle() const { return _requriedFaceAngle; }

	private:
		void MoveUnit(Unit* unit, const Vector3& target, 
		const Vector3& direction, const float ms, Vector3& position);
	};

	// Only rotates toward given direction
	class RotateOnlyStrategy : public IMoveStrategy
	{
		Radian _requriedFaceAngle;

	public:
		RotateOnlyStrategy();
		RotateOnlyStrategy(XmlNode* defNode);

		// Makes unit face towards point specifed bu 'target'
		void Move(
			Unit* movedUnit,
			const Vector3& target,
			const float ms);
		
		void SetMaxFaceAngle(const Radian angle) { _requriedFaceAngle = angle; }
		Radian GetMaxFaceAngle() const { return _requriedFaceAngle; }
	};
}