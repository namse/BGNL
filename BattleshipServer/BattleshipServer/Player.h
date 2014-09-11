#pragma once
#include "stdafx.h"
class Player
{
public:
	Player(std::wstring name, PlayerNumber player_number);
	~Player();

	ATTACK_RESULT AttackAndGetResult(int x, int y);

	bool IsGameEnd();


	void SetMap(char info[MAP_WIDTH][MAP_HEIGHT])
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				info_[x][y] = (MAP_INFO)info[x][y];
			}
		}
		memset(didAttacked_, false, sizeof(didAttacked_));
	}

	void SetState(PlayerState state);
	void SetEnemy(PlayerNumber enemyNumber)
	{
		enemy_number_ = enemyNumber;
	}
	PlayerState GetState() { return player_state_; }
private:
	bool IsShipDestoryed(MAP_INFO shipType);
private:
	MAP_INFO info_[MAP_WIDTH][MAP_HEIGHT];
	bool didAttacked_[MAP_WIDTH][MAP_HEIGHT];
	std::wstring name_;
	PlayerNumber player_number_;
	PlayerState player_state_;
	PlayerNumber enemy_number_;
};

