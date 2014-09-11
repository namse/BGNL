#pragma once
#include "stdafx.h"
class Player
{
public:
	Player(std::wstring name, PlayerNumber player_number);
	~Player();

	ATTACK_RESULT AttackAndGetResult(int x, int y);

	bool IsGameEnd();


	void SetMap(MAP_INFO info[MAP_WIDTH][MAP_HEIGHT])
	{
		memcpy(info_, info, sizeof(info_));
		memset(didAttacked_, false, sizeof(didAttacked_));
	}

private:
	bool IsShipDestoryed(MAP_INFO shipType);
private:
	MAP_INFO info_[MAP_WIDTH][MAP_HEIGHT];
	bool didAttacked_[MAP_WIDTH][MAP_HEIGHT];
	std::wstring name_;
	PlayerNumber player_number_;
};

