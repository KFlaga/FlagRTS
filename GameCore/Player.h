#pragma once

#include "DataTypes.h"

namespace FlagRTS
{
	struct InGameSettings;

	namespace _PlayerRelations
	{
		enum Relations : uint8
		{
			Own = 0,
			Ally,
			Enemy,
			Truce,
			Neutral,
		};
	}
	typedef _PlayerRelations::Relations PlayerRelations; 

	class Player
	{
	protected:
		string _name;
		PlayerRelations _relations[9];
		string _faction;
		Vector3 _color;
		bool _isUsed;
		bool _isCpu;
		Vector2 _startingLocation;
		uint8 _cpuDifficulty;
		uint8 _number;

	public:
		Player();
		~Player();

		const string& GetName() const { return _name; }
		// Name is copied and deleted in destructor
		void SetName(const string& name);

		// True if player is actaully playing in current map
		bool IsUsed() const { return _isUsed; }
		void SetIsUsed(bool value) { _isUsed = value; }

		bool IsCpu() const { _isCpu; }
		void SetIsCpu(bool value) { _isCpu = value; }

		PlayerRelations GetRelationWith(uint8 other) const { return _relations[other]; }
		PlayerRelations SetRelationWith(uint8 other, PlayerRelations relation) { return _relations[other] = relation; }
	
		uint8 GetNumber() const { return _number; }
		void SetNumber(uint8 num) { _number = num; }

		const string& GetFaction() const { return _faction; }
		// Faction is copied and deleted in destructor
		void SetFaction(const string& faction);

		const Vector2& GetStartLocation() const { return _startingLocation; }
		void SetStartLocation(const Vector2& loc) { _startingLocation = loc; }

		const Vector3& GetColor() const { return _color; }
		void SetColor(const Vector3& color) { _color = color; }
	};

	class PlayersInfo
	{
	protected:
		int _playersCount; // Only standard players counts [1-8]
		int _clientPlayer;
		Player* _players[9];
		int _playerNumbersMap[9]; // Maps in-game player number with array index

	public:
		PlayersInfo();
		~PlayersInfo();

		void SetPlayersSetting(InGameSettings* settings);

		int GetPlayersCount()
		{
			return _playersCount;
		}

		Player* GetPlayer(const int num) const
		{
			return _players[_playerNumbersMap[num]];
		}

		int GetClientPlayer() const { return _clientPlayer; }

		bool IsOwned(int player)
		{
			return player == _clientPlayer;
		}

		bool IsAlly(int player, int potentialAlly) const
		{
			return GetPlayer(player)->GetRelationWith(potentialAlly) == 
				PlayerRelations::Ally;
		}

		bool IsNeutral(int player, int potentialNautral) const
		{
			return GetPlayer(player)->GetRelationWith(potentialNautral) == 
				PlayerRelations::Neutral;
		}

		bool IsEnemy(int player, int potentialEnemy) const
		{
			return GetPlayer(player)->GetRelationWith(potentialEnemy) == 
				PlayerRelations::Enemy;
		}
	};
}