#include "DefaultMoveStrategies.h"
#include "Unit.h"
#include "GameWorld.h"
#include "Map.h"

namespace FlagRTS
{
	NoMoveMoveStrategy::NoMoveMoveStrategy()
	{ 
		SetFinalType(GetTypeId<NoMoveMoveStrategy>());
		SetFinalTypeName("MoveStrategy"); // All MoveStartegies have same final type name
	}

	NoMoveMoveStrategy::NoMoveMoveStrategy(XmlNode* xml) :
		IMoveStrategy(xml)
	{ 
		SetFinalType(GetTypeId<NoMoveMoveStrategy>());
		SetFinalTypeName("MoveStrategy"); // All MoveStartegies have same final type name
	}

	void NoMoveMoveStrategy::Move(
		Unit* movedUnit,
		const Vector3& target,
		const float ms)
	{
		_remSqDistance = (target - movedUnit->GetPositionAbsolute()).squaredLength();
		Vector3 targetDirection(target - movedUnit->GetPositionAbsolute());
		targetDirection.y = 0.0f;
		Vector3 currentDirection(movedUnit->GetDirectionAbsolute());
		Radian angleToTargetDir = currentDirection.angleBetween(targetDirection);
		_remAngle = angleToTargetDir.valueRadians();
	}

	FaceAndGoMoveStrategy::FaceAndGoMoveStrategy() :
		_yOffset(0.f),
		_requriedFaceAngle(0.f)
	{
		SetFinalType(GetTypeId<FaceAndGoMoveStrategy>());
		SetFinalTypeName("MoveStrategy"); // All MoveStartegies have same final type name
	}

	FaceAndGoMoveStrategy::FaceAndGoMoveStrategy(XmlNode* node) :
		IMoveStrategy(node)
	{
		SetFinalType(GetTypeId<FaceAndGoMoveStrategy>());
		SetFinalTypeName("MoveStrategy"); // All MoveStartegies have same final type name

		_yOffset = XmlUtility::XmlGetFloatIfExists(node->first_node("MoveHeight"), "value", 0.f);
		_requriedFaceAngle = Radian(Degree(XmlUtility::XmlGetFloatIfExists(node->first_node("MaxFaceAngle"), "value", 0.f)));
	}

	FaceAndGoMoveStrategy::~FaceAndGoMoveStrategy()
	{

	}

	inline void FaceAndGoMoveStrategy::MoveUnit(Unit* unit, const Vector3& target, 
		const Vector3& direction1, const float ms, Vector3& position)
	{
		// Get max distance unit can move
		float maxDist = unit->GetSpeed() * ms;
		// Get distance to target
		float posDiff = (target - position).squaredLength();
		if( maxDist*maxDist >= posDiff)
		{
			// If distance is smaller than max possible distance, then move there
			position = target;
			_remSqDistance = 0.f;
		}
		else
		{
			const Vector3& direction = unit->GetDirectionAbsolute();
			// If couldn't reach the target move as much as unit can
			position += direction * maxDist;
			_remSqDistance = posDiff - maxDist*maxDist;
		}
		// Ensure unit is on the ground
		position.y = GameWorld::GlobalWorld->GetMap()->
			GetTerrainHeight(position.x, position.z) + _yOffset;
		unit->SetPosition(position);
	}

	void FaceAndGoMoveStrategy::Move(
		Unit* movedUnit,
		const Vector3& targetPos,
		const float ms)
	{
		Vector3 position(movedUnit->GetPositionAbsolute());
		position.y = 0.f;
		Vector3 target(targetPos.x, 0.f, targetPos.z);

		// Find direction
		Vector3 targetDirection(target - position);
		targetDirection.y = 0.0f;
		Vector3 currentDirection(movedUnit->GetDirectionAbsolute());
		currentDirection.y = 0.0f;
		// Get angle to rotate about -y to achieve target rotation
		Radian angleToTargetDir = currentDirection.angleBetween(targetDirection);
		// On very small angle just move towards target ( on very large objects it may seem 
		// with high speed it may look like its not moving forward -> to check )
		if(angleToTargetDir < _requriedFaceAngle)
		{
			MoveUnit(movedUnit, target, targetDirection, ms, position);
			_remAngle = angleToTargetDir.valueRadians();
			return;
		}

		// Get rotation direction ( or rather angle sign ) :
		// rotate target by -a, where a is angle between curDir = (x0,y0) and xaxis = (1,0) -> ( x0 = cosa, z0 = sina )
		// if rotated target z < 0, angle is positive. rot.tar.z = xt*sina + zt*cosa = xt*z0 + zt*x0
		int sign = -targetDirection.x*currentDirection.z + 
			targetDirection.z*currentDirection.x > 0 ? -1 : 1;
		// Rotate with max rotation speed toward target
		Radian maxRot(Degree(movedUnit->GetRotationSpeed() * ms));
		Radian actualRotation = std::min( angleToTargetDir, maxRot );
		movedUnit->Rotate(Vector3(0.0f,sign*1.0f,0.0f), 
			angleToTargetDir < maxRot ? angleToTargetDir : maxRot );
		angleToTargetDir -= actualRotation;
		// Look again if we are in range
		if(angleToTargetDir < _requriedFaceAngle)
		{
			MoveUnit(movedUnit, target, targetDirection, ms, position);
		}
		else
		{
			_remSqDistance = (target - position).squaredLength();
		}
		_remAngle = angleToTargetDir.valueRadians();
	}

	RotateOnlyStrategy::RotateOnlyStrategy() :
		_requriedFaceAngle(0.f)
	{
		SetFinalType(GetTypeId<RotateOnlyStrategy>());
		SetFinalTypeName("MoveStrategy"); // All MoveStartegies have same final type name
	}

	RotateOnlyStrategy::RotateOnlyStrategy(XmlNode* node) :
		IMoveStrategy(node)
	{
		SetFinalType(GetTypeId<RotateOnlyStrategy>());
		SetFinalTypeName("MoveStrategy"); // All MoveStartegies have same final type name

		_requriedFaceAngle = Radian(Degree(XmlUtility::XmlGetFloatIfExists(node->first_node("MaxFaceAngle"), "value", 0.f)));
	}

	void RotateOnlyStrategy::Move(
		Unit* movedUnit,
		const Vector3& targetPos,
		const float ms)
	{
		Vector3 targetDirection = targetPos - movedUnit->GetPositionAbsolute();
		targetDirection.y = 0.f;

		Vector3 currentDirection = movedUnit->GetDirectionAbsolute();
		currentDirection.y = 0.f;
		Radian angleToTargetDir = currentDirection.angleBetween(targetDirection);

		int sign = -targetDirection.x*currentDirection.z + 
			targetDirection.z*currentDirection.x > 0 ? -1 : 1;
		// Rotate with max rotation speed toward target
		Radian rotation = std::min(angleToTargetDir, Radian(Degree(movedUnit->GetRotationSpeed() * ms)));
		movedUnit->Rotate(Vector3(0.0f,sign*1.0f,0.0f), rotation);
		angleToTargetDir -= rotation;
		_remAngle = angleToTargetDir.valueRadians();
		_remSqDistance = movedUnit->GetPositionAbsolute().squaredDistance(targetPos);
	}
}
