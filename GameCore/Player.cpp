#include "Player.h"
#include "InGameSettings.h"
#include <StringUtils.h>

namespace FlagRTS
{
	Player::Player() :
		_isCpu(false),
		_isUsed(false)
	{

	}

	Player::~Player()
	{

	}

	void Player::SetName(const string& name)
	{
		_name = name;
	}

	void Player::SetFaction(const string& faction)
	{
		_faction = faction;
	}

	PlayersInfo::PlayersInfo() :
		_playersCount(0),
		_clientPlayer(0)
	{

	}

	void PlayersInfo::SetPlayersSetting(InGameSettings* settings)
	{
		_playersCount = 0;
		_clientPlayer = settings->ClientPlayer;
		unsigned int plNum = 0;
		for(plNum = 0; plNum < 8; plNum++)
		{
			_playerNumbersMap[plNum] = -1;
		}

		for(plNum = 0; plNum < settings->Players.size(); plNum++)
		{
			_players[plNum] = settings->Players[plNum];
			_playerNumbersMap[settings->Players[plNum]->GetNumber()] = plNum;
			if( settings->Players[plNum]->IsUsed() )
			{
				_playersCount++;
			}
		}

		for(; plNum < 8; plNum++)
		{
			_players[plNum] = xNew0(Player);
			_players[plNum]->SetIsUsed(false);
			// Find next unassgined player and set it in map
			for(unsigned int i = 0; i < 8; ++i)
			{
				if( _playerNumbersMap[i] == -1 )
				{
					_playerNumbersMap[i] = plNum;
				}
			}

		}

		_players[8] = xNew0(Player);
		_players[8]->SetIsCpu(true);
		_players[8]->SetIsUsed(true);
		_players[8]->SetFaction("Neutral");
		_players[8]->SetNumber(8);
		_playerNumbersMap[8] = 8;
		_players[8]->SetStartLocation(Vector2(0.f,0.f));
		_players[8]->SetName("Neutral");
		_players[8]->SetColor(Vector3(0.5f,0.5f,0.5f));
	}

	PlayersInfo::~PlayersInfo()
	{
		// Delete players created here
		for(int i = _playersCount; i < 9; i++)
		{
			xDelete(_players[i]);
		}
	}
}