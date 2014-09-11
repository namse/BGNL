#pragma once
#include "stdafx.h"
#include "EventListener.h"
class Player : public EventListener
{
public:
	Player(std::wstring name, PlayerNumber player_number);
	~Player();

	ATTACK_RESULT AttackAndGetResult(int x, int y);

	bool IsGameEnd();

	void SetEnemy(Player* enemy)
	{
		enemy_ = enemy;
	}

	void Notify(EventHeader* event);
private:
	bool IsShipDestoryed(MAP_INFO shipType);
private:
	MAP_INFO info_[MAP_WIDTH][MAP_HEIGHT];
	bool didAttacked_[MAP_WIDTH][MAP_HEIGHT];
	Player* enemy_;
	std::wstring name_;
	PlayerNumber player_number_;
};

