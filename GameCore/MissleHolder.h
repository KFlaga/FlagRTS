#pragma once

#include <Array.h>

namespace FlagRTS
{
	class Missle;
	class MissleDefinition;

	namespace MissleAllocationStrategies
	{
		enum MissleAllocationStrategyType : int
		{
			NoResize = 1<<0, // When storage runs out of missles it just doesnt give new one
			DoubleSize = 1<<1, // Doubles storage size if free missle count is too small, but reduces it again if missle count = size/2 - freeCount
			AddBaseSize = 1<<2, // Adds base size to storage size
			CooldownOnPeak = 1<<3, // Cooldown on reducing size resets when peak is reached
			CooldownOnRequest = 1<<4, // Cooldown on reducing size resets with every missle request
			NoUserResize = 1<<5, // Size and free missles are not dependent on users count
			UsersProportionalResize = 1<<6, // Base size / free missles count is proportional to created units which uses this missle
		};

		MissleAllocationStrategyType ParseMissleAllocationStrategyType(
			const char* resizeName, const char* cooldownName, const char* userResizeName);
	};
	typedef MissleAllocationStrategies::MissleAllocationStrategyType MissleAllocationStrategy;

	// Missle storage for effects :
	// all units should share same MissleHolder for launching missles of given type
	// Missles in storage are not destroyed after impact but stored for later together with
	// SceneNodes -> they are only Spawned / Despawned
	// MissleHolder 
	class MissleHolder
	{
	protected:
		MissleDefinition* _missleDef;
		Array<Missle*> _freeMissles;
		MissleAllocationStrategy _allocStrategy;
		int _currentSize;
		int _baseCapacity; // Base size of storage
		int _ensureFreeMissles; // Minimum amout of free missles stored ( if free missles count < this, then new ones are created )
		float _cooldownTime; // Time after getting which storage can be shrinken -> 
							 // most probably extensive missle producing is during battle, so we want to wait
							 // couple of seconds ( maybe even a minute ) so 
		float _currentCooldown;
		int _users;
		int _misslesPeak;

	public:
		MissleHolder();
		MissleHolder(MissleDefinition* missleDef);
		MissleHolder(MissleDefinition* missleDef, 
			MissleAllocationStrategy allocStrategy, 
			int baseSize, 
			int freeMissles, 
			float cooldown);
		~MissleHolder();

		// Returns new missle to be launched -> it is created, loaded and have SceneNode ( but is not spawned and node is detached )
		Missle* GetMissle();
		// After missle is to be despawned it should be released
		void ReleaseMissle(Missle*);
		// Tells storage that new unit which uses this missle is created
		void AddUser();
		// Tells storage that some user died or sth
		void RemoveUser();

		// Updates cooldown and resizes according to alloc strategy if cooldown = 0
		// ( ideally it would do it on other thread or smaller workload frames )
		void Update(float ms);

		void SetMissleDefinition(MissleDefinition* missleDef) { _missleDef = missleDef; }
		void SetBaseSize(int value) { _baseCapacity = value; }
		void SetFreeMissles(int value) { _ensureFreeMissles = value; }
		void SetCooldown(float value) { _cooldownTime = value; }
		void SetAllocStartegy(MissleAllocationStrategy value) { _allocStrategy = value; }

		void Resize(int newSize);
	};
}