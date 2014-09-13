#pragma once
#include "stdafx.h"
class Player
{
public:
	Player(std::wstring name, PlayerNumber player_number);
	~Player();

	AttackResult AttackAndGetResult(int x, int y);

	bool IsGameEnd();


	void SetMap(char info[MAP_WIDTH][MAP_HEIGHT])
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			for (int y = 0; y < MAP_HEIGHT; y++)
			{
				info_[x][y] = (MapInfo)info[x][y];
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

	void InitTurns()
	{
		turns_ = 0;
	}
	void CheckTurn()
	{
		turns_++;
	}
	void AddWinTotalTurns()
	{
		win_total_turns_ += turns_;
	}
	void AddWinCount()
	{
		win_count_++;
	}
	int GetAverageTurns()
	{
		if (win_count_ == 0)
			return 0;
		else
			return win_total_turns_ / win_count_;
	}

	int GetTurns() { return turns_; }
	int GetWinCount() { return win_count_; }
private:
	bool IsShipDestoryed(MapInfo shipType);
private:
	MapInfo info_[MAP_WIDTH][MAP_HEIGHT];
	bool didAttacked_[MAP_WIDTH][MAP_HEIGHT];
	std::wstring name_;
	PlayerNumber player_number_;
	PlayerState player_state_;
	PlayerNumber enemy_number_;

	int turns_;
	int win_total_turns_;
	int win_count_;
};

