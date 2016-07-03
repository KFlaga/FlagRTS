#include "MissleHolder.h"
#include "Missle.h"
#include "GameWorld.h"

namespace FlagRTS
{
	MissleHolder::MissleHolder() :
		_missleDef(0),
		_baseCapacity(0),
		_ensureFreeMissles(0),
		_cooldownTime(0.f),
		_currentSize(0),
		_currentCooldown(0.f),
		_misslesPeak(0)
	{ }

	MissleHolder::MissleHolder(MissleDefinition* missleDef) :
		_missleDef(missleDef),
		_baseCapacity(0),
		_ensureFreeMissles(0),
		_cooldownTime(0.f),
		_currentSize(0),
		_currentCooldown(0.f),
		_users(0),
		_misslesPeak(0)
	{ }

	MissleHolder::MissleHolder(MissleDefinition* missleDef, 
		MissleAllocationStrategy allocStrategy, 
		int baseSize, 
		int freeMissles, 
		float cooldown) :
	_missleDef(missleDef),
		_baseCapacity(baseSize),
		_ensureFreeMissles(freeMissles),
		_cooldownTime(cooldown),
		_currentSize(0),
		_currentCooldown(0.f),
		_users(0),
		_misslesPeak(0)
	{
		
	}

	MissleHolder::~MissleHolder()
	{

	}

	Missle* MissleHolder::GetMissle()
	{
		if( _freeMissles.size() == 0 )
		{
			if( (_allocStrategy & MissleAllocationStrategies::DoubleSize) != 0 )
			{
				Resize(_currentSize * 2 );
			}
			else if( (_allocStrategy & MissleAllocationStrategies::AddBaseSize) != 0 )
			{
				Resize(_currentSize + _baseCapacity);
			}
			else
			{
				_ASSERT(false); // Should not happen
				Resize(_currentSize + 1);
			}
		}

		if( (_allocStrategy & MissleAllocationStrategies::CooldownOnRequest) != 0 )
		{
			_currentCooldown = _cooldownTime;
		}
		else if( (int)(_currentSize - _freeMissles.size()) >= _misslesPeak )
		{
			_misslesPeak = _currentSize - _freeMissles.size();
			_currentCooldown = _cooldownTime;
		}

		Missle* missle = _freeMissles.back();
		_freeMissles.pop_back();
		return missle;
	}

	void MissleHolder::ReleaseMissle(Missle* missle)
	{
		missle->Despawn();
		missle->GetSceneNode()->getParent()->removeChild(missle->GetSceneNode());

		_freeMissles.push_back(missle);
	}

	void MissleHolder::AddUser()
	{
		++_users;
		if( (_allocStrategy & MissleAllocationStrategies::UsersProportionalResize) ||
			_users == 1)
		{
			// Add baseCount to missle count, or more if not enough free missles exists
			Resize(_currentSize + std::max(_baseCapacity, 
				(int)(_ensureFreeMissles * _users - _freeMissles.size())));
		}
	}

	void MissleHolder::RemoveUser()
	{
		--_users;
		if( (_allocStrategy & MissleAllocationStrategies::UsersProportionalResize) ||
			_users == 0 )
		{
			// Remove baseCount to missle count, or less if not enough free missles exists
			Resize(_currentSize - std::min(_baseCapacity, 
				(int)(_ensureFreeMissles * _users - _freeMissles.size())));
		}
	}

	void MissleHolder::Update(float ms)
	{
		if( _currentCooldown > 0.f )
		{
			_currentCooldown -= ms;
			if( _currentCooldown <= 0.f )
			{
				if( (_allocStrategy & MissleAllocationStrategies::UsersProportionalResize) != 0 )
				{
					// Ensure that :
					// 1) _currentSize >= _baseCount * users
					// 2) _freeSize >= _freeCount * users 
					Resize( std::max(_baseCapacity * _users, 
						(int)(_currentSize + _ensureFreeMissles * _users - _freeMissles.size())));
				}
				else
				{
					Resize( std::max(_baseCapacity, 
						(int)(_currentSize + _ensureFreeMissles - _freeMissles.size())));
				}
				_misslesPeak = 0;
			}
		}
	}

	void MissleHolder::Resize(int newSize)
	{
		if(newSize > _currentSize)
		{
			// Need to create some additional Missles
			int toCreate = newSize - _currentSize;
			int freeSize = _freeMissles.size();
			_freeMissles.resize(freeSize + toCreate);

			for(int i = 0; i < toCreate; ++i)
			{
				Missle* missle = static_cast<Missle*>(
					GameWorld::GlobalWorld->CreateSceneObject(_missleDef, NEUTRAL_PLAYERNUM));
				GameWorld::GlobalWorld->SpawnSceneObject(missle, 
					SpawnInfo(Quaternion::IDENTITY, Vector3::ZERO, false));
				// Have missle spawned - detach it for now
				missle->Despawn();
				if(missle->GetSceneNode()->getParent() != 0 )
					missle->GetSceneNode()->getParent()->removeChild(missle->GetSceneNode());
				_freeMissles[freeSize + i] = missle;
			}
		}
		else if(newSize < _currentSize)
		{
			// Need to destroy some missles
			int freeSize = _freeMissles.size();
			int toRemove = std::min(freeSize, _currentSize - newSize);

			for(int i = 1; i <= toRemove; ++i)
			{
				Missle* missle = _freeMissles[freeSize - i];
				GameWorld::GlobalWorld->QueueDespawnSceneObject(missle);
				GameWorld::GlobalWorld->QueueDestroySceneObject(missle);
			}

			_freeMissles.resize(freeSize - toRemove);
		}
		_currentSize = newSize;
	}

	namespace MissleAllocationStrategies
	{
		std::map<string, MissleAllocationStrategyType>& GetAllocationStrategyNamesMap()
		{
			static std::map<string, MissleAllocationStrategyType> map;
			map["NoResize"] = NoResize;
			map["DoubleSize"] = DoubleSize;
			map["AddBaseSize"] = AddBaseSize;
			map["CooldownOnPeak"] = CooldownOnPeak;
			map["CooldownOnRequest"] = CooldownOnRequest;
			map["NoUserResize"] = NoUserResize;
			map["UsersProportionalResize"] = UsersProportionalResize;
			return map;
		}

		MissleAllocationStrategyType ParseMissleAllocationStrategyType(
			const char* resizeName, const char* cooldownName, const char* userResizeName)
		{
			static std::map<string, MissleAllocationStrategyType>& map = GetAllocationStrategyNamesMap();
			int allocStrategy = 0;
			auto typeIt = map.find(resizeName);
			if(typeIt != map.end())
				allocStrategy |= typeIt->second;

			typeIt = map.find(cooldownName);
			if(typeIt != map.end())
				allocStrategy |= typeIt->second;

			typeIt = map.find(userResizeName);
			if(typeIt != map.end())
				allocStrategy |= typeIt->second;

			return (MissleAllocationStrategyType)allocStrategy;
		}
	};
}