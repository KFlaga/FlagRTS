#include "PackageStates.h"
#include <PhysicalObject.h>
#include <AnimationController.h>

namespace FlagRTS
{
	DllExclusive PackageMovingOnTransmissionBeltState::PackageMovingOnTransmissionBeltState(PhysicalObject* owner) :
		_owner(owner)
	{ }

	DllExclusive void PackageMovingOnTransmissionBeltState::Begin()
	{
		// just play translation animation
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "BeltTransport", false);
	}

	DllExclusive void PackageMovingOnTransmissionBeltState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void PackageMovingOnTransmissionBeltState::Update(float ms)
	{
		_owner->GetAnimController().Update(ms);
	}

	DllExclusive PackageBeingTransportedByMineCraneState::PackageBeingTransportedByMineCraneState(PhysicalObject* owner) :
		_owner(owner)
	{ }

	DllExclusive void PackageBeingTransportedByMineCraneState::Begin()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Internal, "CraneTransport", false);
	}

	DllExclusive void PackageBeingTransportedByMineCraneState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	DllExclusive void PackageBeingTransportedByMineCraneState::Update(float ms)
	{
		_owner->GetAnimController().Update(ms);
	}

}