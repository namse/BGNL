#include "stdafx.h"
#include "Player.h"


Player::Player(std::wstring name, PlayerNumber player_number)
	:name_(name), player_number_(player_number)
{
	memset(info_, MI_EMPTY, sizeof(info_));
	memset(didAttacked_, false, sizeof(didAttacked_));
}


Player::~Player()
{
}


void Player::Notify(EventHeader* event)
{

}

ATTACK_RESULT Player::AttackAndGetResult(int x, int y)
{
	if (x < 0 || x > MAP_WIDTH
		|| y < 0 || y > MAP_HEIGHT)
	{
		//OH;;
		std::cout << "ERROR : Attack Range is over, X = " << x << ", Y = " << y << std::endl;
		return AR_NONE;
	}
	didAttacked_[x][y] = true;

	MAP_INFO shipType = info_[x][y];

	if (shipType == MI_EMPTY)
	{
		return AR_MISS;
	}
	else
	{
		if (IsShipDestoryed(shipType) == true)
		{
			switch (shipType)
			{
			case MI_AIRCRAFT:
				return AR_DESTORY_AIRCRAFT;
				break;
			case MI_BATTLESHIP:
				return AR_DESTORY_BATTLESHIP;
				break;
			case MI_CRUSIER:
				return AR_DESTORY_CRUSIER;
				break;
			case MI_DESTORYER_1:
			case MI_DESTORYER_2:
				return AR_DESTORY_DESTORYER;
				break;
			default:
				return AR_NONE;
				break;
			}
		}
		else
		{
			return AR_HIT;
		}
	}
}

bool Player::IsShipDestoryed(MAP_INFO shipType)
{
	if (shipType == MI_EMPTY)
		return false;

	for (int x = 0; x < MAP_WIDTH; x++)
	{
		for (int y = 0; y < MAP_HEIGHT; y++)
		{
			// MAKE SURE - if Coord X,Y attacked,
			// did you check 'didAttacked_[X][Y]' before call this function?
			// you should change value -> { didAttacked_[X][Y] = true; }like this.
			if (info_[x][y] == shipType
				&& didAttacked_[x][y] == false)
				return false;
		}
	}

	return true;
}
