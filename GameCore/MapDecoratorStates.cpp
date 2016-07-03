#include "MapDecoratorStates.h"
#include "MapDecorator.h"
#include "AnimationController.h"

namespace FlagRTS
{
	MapDecoratorIdleState::MapDecoratorIdleState(MapDecorator* owner) :
		_owner(owner)
	{

	}

	void MapDecoratorIdleState::Begin()
	{
		// for now start animation
		_owner->GetAnimController().ChangeAnimation(
			SceneObjectStates::Idle, "Default");
	}

	void MapDecoratorIdleState::End()
	{
		_owner->GetAnimController().EndAllAnimations();
	}

	void MapDecoratorIdleState::Update(float ms)
	{
		// for now update animation
		_owner->GetAnimController().Update(ms);
	}

	const char* MapDecoratorIdleState::GetName()
	{
		return "Idle";
	}

	size_t MapDecoratorIdleState::GetType()
	{
		return SceneObjectStates::Idle;
	}
}