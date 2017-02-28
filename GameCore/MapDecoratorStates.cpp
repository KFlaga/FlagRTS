#include "MapDecoratorStates.h"
#include "MapDecorator.h"
#include "AnimationController.h"

namespace FlagRTS
{
	MapDecoratorIdleState::MapDecoratorIdleState(MapDecorator* owner) :
		SceneObjectState(SceneObjectStates::Idle, "Idle"),
		_owner(owner)
	{

	}

	void MapDecoratorIdleState::Begin()
	{
		// for now start animation
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Idle, "Default");
		_status = StateStatus::RunningNoncritical;
	}

	void MapDecoratorIdleState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
		_status = StateStatus::Ready;
	}
	
	void MapDecoratorIdleState::Interrupt()
	{
		_owner->GetAnimController().EndAllAnimations();
		_status = StateStatus::Ready;
	}
	
	void MapDecoratorIdleState::Resume()
	{
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Idle, "Default");
		_status = StateStatus::RunningNoncritical;
	}

	void MapDecoratorIdleState::Update(float ms)
	{
		// for now update animation
		_owner->GetAnimController().Update(ms);
	}
}