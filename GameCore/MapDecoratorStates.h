#pragma once

#include "SceneObjectStateMachine.h"

namespace FlagRTS
{
	class MapDecorator;

	// MD idle state :
	// if loop idle animation is set it loops it
	// if timed animatiion is set it registers it to be started at intervals 
	class MapDecoratorIdleState : public SceneObjectState
	{
	private:
		MapDecorator* _owner;

	public:
		MapDecoratorIdleState(MapDecorator* owner);

		void Begin();

		void End();

		void Update(float ms);
		
		const char* GetName();

		size_t GetType();
	};
}