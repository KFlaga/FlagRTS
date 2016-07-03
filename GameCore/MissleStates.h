#pragma once

#include "SceneObjectStateMachine.h"

namespace FlagRTS
{
	class Missle;

	// Missle idle state, does nothing
	class MissleIdleState : public SceneObjectState
	{
	private:
		Missle* _owner;

	public:
		MissleIdleState(Missle* owner);
		void Update(float ms);

		const char* GetName();
		size_t GetType();
	};

	// Missle idle state :
	// plays (Move,Default) animation if such exists
	// updates mover
	class MissleMoveState : public SceneObjectState
	{
	private:
		Missle* _owner;
		float _moveTime;
		float _totalDistance;

	public:
		MissleMoveState(Missle* owner);

		void Begin();
		void End();
		void Update(float ms);
		const char* GetName();
		size_t GetType();
	};

	// Missle dying state -> changed to on impact
	// plays (Dying,Default) animation if such exists
	// When anim ends ( or there's no anim ) states switches to Idle
	// When state ends ( so on above switch ) Missle is released
	class MissleDyingState : public SceneObjectState
	{
	private:
		Missle* _owner;

	public:
		MissleDyingState(Missle* owner);

		void Begin();
		void End();
		void Update(float ms);		
		const char* GetName();
		size_t GetType();
	};
}