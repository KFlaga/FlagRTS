#pragma once

#include "IMover.h"

namespace FlagRTS
{
	// Most basic mover -> moves object in straight line with 
	// constant velocity using current orientation of object ( uses Move in local space )
	class LineMover : public IMover
	{
	private:
		float _velocity; // velocity is in parsed in units per sec but stored in units per ms

	public:
		LineMover();
		LineMover(XmlNode* node);

		float Move(SceneObject* object, float moveTime, float frameTime);

		float GetVelocity() const { return _velocity; }
		void SetVelocity(float vel) { _velocity = vel; }
	};

	// Moves object in straight line starting with initial velocity and
	// accelerating untill reach max velocity
	// If acceleration < 0 and max velocity < initial object will slow down
	class AcceleratedLineMover : public IMover
	{
	private:
		float _initialVelocity; // velocity is in parsed in units per sec but stored in units per ms
		float _acceleration; // acceleration is in units per sec^2 but stored in units per ms^2
		float _maxVelocity; 
		float _accTime; // Time to reach max velocity

	public:
		AcceleratedLineMover();
		AcceleratedLineMover(XmlNode* node);

		float Move(SceneObject* object, float moveTime, float frameTime);
		
		float GetMaxVelocity() const { return _maxVelocity; }
		void SetMaxVelocity(float vel) { _maxVelocity = vel; }
		
		float GetInitialVelocity() const { return _initialVelocity; }
		void SetInitialVelocity(float vel) { _initialVelocity = vel; }
		
		float GetAcceleration() const { return _acceleration; }
		void SetAcceleration(float acc) { _acceleration = acc; }

		// Should be called if some path param changed
		void UpdateAccelarationTime();
	};
}