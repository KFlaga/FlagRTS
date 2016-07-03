#pragma once

#include "PhysicalObjectDefinition.h"

namespace FlagRTS
{
	namespace MissleCollsionTypes
	{
		enum MissleCollsionType
		{
			OnTarget,
			OnPoint,
			OnObject
		};
	}
	typedef MissleCollsionTypes::MissleCollsionType MissleCollsionType;

	class Missle;
	class MoverDefintion;
	class PhysicalObject;
	class IEffect;
	class IMover;

	// Missles are ... just missles
	// They are attached to object that fires it and fires event on impact with target
	// There are several types of missles:
	// By collision : 
	// - collide on target -> ingnores unit collision except target
	// - collide on point -> ignores unit collision and impacts on point
	// - coliide on object -> collides with first crossed unit
	// By mover type :
	// - line missle -> flies in streight line
	// - path missle -> flies predefined path ( some equation ( defined by mover ) )
	// - homing missle -> homes on target
	class MissleDefinition : public PhysicalObjectDefinition
	{
	protected:
		IMover* _mover;
		MissleCollsionType _collisionType;
		float _maxRange;

		IEffect* _onImpactEffect;

	public:
		MissleDefinition();
		MissleDefinition(XmlNode* missleNode);

		IEffect* GetOnImpactEffect() { return _onImpactEffect; }
		void SetOnImpactEffect(IEffect* effect) { _onImpactEffect = effect; }

		IMover* GetMover() const { return  _mover; }
		void SetMover(IMover* value) { _mover = value; }
		
		MissleCollsionType GetMissleCollsionType() const { return _collisionType; }
		void SetMissleCollsionType(MissleCollsionType value) { _collisionType = value; }
		
		float GetMaxRange() const { return _maxRange; }
		void SetMaxRange(float value) { _maxRange = value; }
	};

}