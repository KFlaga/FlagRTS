#pragma once

#include <SceneObjectState.h>

namespace FlagRTS
{
	class PhysicalObject;

	// In this state package is just created and is moving on the mine transmitter
	// Package is playing transform animation to move itself
	class DllExclusive PackageMovingOnTransmissionBeltState : public SceneObjectState
	{
	private:
		PhysicalObject* _owner;

	public:
		PackageMovingOnTransmissionBeltState(PhysicalObject* owner);

		void Begin();
		void End();
		void Update(float ms);
	};
	
	// In this state package is picked up by mine's crane
	// Package is playing transform animation to move itself
	class DllExclusive PackageBeingTransportedByMineCraneState : public SceneObjectState
	{
	private:
		PhysicalObject* _owner;

	public:
		PackageBeingTransportedByMineCraneState(PhysicalObject* owner);

		void Begin();
		void End();
		void Update(float ms);
	};
}