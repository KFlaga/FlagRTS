#include "Movers.h"
#include "SceneObject.h"

namespace FlagRTS
{
	LineMover::LineMover()
	{
		SetFinalType(GetTypeId<LineMover>());
		SetFinalTypeName("Mover"); // Every mover have same final type name
	}

	LineMover::LineMover(XmlNode* node) :
		IMover(node)
	{
		SetFinalType(GetTypeId<LineMover>());
		SetFinalTypeName("Mover");

		// Line mover needs only velocity
		_velocity = XmlUtility::XmlGetFloat(node->first_node("Velocity"), "value");
	}

	float LineMover::Move(SceneObject* object, float moveTime, float frameTime)
	{
		object->Move( Vector3(0.0f, 0.f, 1.f) * _velocity * frameTime,  
			Ogre::Node::TS_LOCAL );
		return _velocity * frameTime;
	}

	AcceleratedLineMover::AcceleratedLineMover()
	{
		SetFinalType(GetTypeId<LineMover>());
		SetFinalTypeName("Mover");
	}

	AcceleratedLineMover::AcceleratedLineMover(XmlNode* node) :
		IMover(node)
	{
		SetFinalType(GetTypeId<LineMover>());
		SetFinalTypeName("Mover");
		
		_maxVelocity = XmlUtility::XmlGetFloat(node->first_node("MaxVelocity"), "value") * 1e-3f;
		_initialVelocity = XmlUtility::XmlGetFloat(node->first_node("InitialVelocity"), "value") * 1e-3f;
		_acceleration = XmlUtility::XmlGetFloat(node->first_node("Acceleration"), "value") * 1e-6f;
		UpdateAccelarationTime();
	}

	float AcceleratedLineMover::Move(SceneObject* object, float moveTime, float frameTime)
	{
		float distance;
		if( moveTime + frameTime < _accTime )
		{
			// ds = v0*dt + 0.5 * a * ((t+dt)^2 - t^2) = v0*dt + 0.5 * a * (dt^2 + 2*t*dt) = dt(v0+0.5a(dt+2t))
			distance = frameTime *_initialVelocity + 
				0.5f * _acceleration * ( frameTime* frameTime + 2*frameTime*moveTime );
		}
		else if(moveTime > _accTime)
		{
			// Constant velocity from now on
			distance = _maxVelocity * frameTime; 
		}
		else
		{
			// Finished acceleating :
			// Treat it as 2 part movement -> one with 1st equation and frame time = t_acc - t_move
			distance = frameTime *( _initialVelocity + 0.5f * 
				_acceleration * ( frameTime + 2*(_accTime - moveTime)));
			// 2nd with constant speed vmax in time t_move + t_f - t_acc
			distance += _maxVelocity * (moveTime + frameTime - _accTime); 
		}

		object->Move( Vector3(0.0f, 0.f, 1.f) * distance,  
			Ogre::Node::TS_LOCAL );
		return distance;
	}

	void AcceleratedLineMover::UpdateAccelarationTime()
	{
		float vDiff = _maxVelocity - _initialVelocity;
		if( vDiff * _acceleration > 0.f )
		{
			// Acceleration and velocity difference have same sign - so good
			// mover will be able to reach vmax in finite time
			_accTime = vDiff / _acceleration;
		}
		else
		{
			// As setting vmax < vinital && accel > 0 is senseless, disregard such cases
			// and assume infinite acceleration is intended
			_accTime = 1e9f;
		}
	}
}