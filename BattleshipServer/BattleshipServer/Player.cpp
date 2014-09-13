#include "stdafx.h"
#include "Player.h"


Player::Player(std::wstring name, PlayerNumber player_number)
	:name_(name), player_number_(player_number), player_state_(PS_WAIT), enemy_number_(-1)
	, turns_(-1), win_total_turns_(0), win_count_(0)
{
	memset(info_, MI_EMPTY, sizeof(info_));
	memset(didAttacked_, false, sizeof(didAttacked_));
}


Player::~Player()
{
}

AttackResult Player::AttackAndGetResult(int x, int y)
{
	if (x < 0 || x > MAP_WIDTH
		|| y < 0 || y > MAP_HEIGHT)
	{
		//OH;;
		std::cout << "ERROR : Attack Range is over, X = " << x << ", Y = " << y << std::endl;
		return AR_NONE;
	}
	didAttacked_[x][y] = true;

	MapInfo shipType = info_[x][y];

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
			case MI_CRUISER:
				return AR_DESTORY_CRUISER;
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

bool Player::IsShipDestoryed(MapInfo shipType)
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


void Player::SetState(PlayerState state)
{
	switch (state)
	{
	case PS_WAIT:
	{
		player_state_ = state;
	}break;
	case PS_READY:
	{
		player_state_ = state;
	}break;
	case PS_WAIT_MAP:
	{
		player_state_ = state;
	}break;
	case PS_ON_GAME:
	{
		player_state_ = state;
	}break;
	default:
		break;
	}
}


bool Player::IsGameEnd()
{
	return IsShipDestoryed(MapInfo::MI_AIRCRAFT)
		&& IsShipDestoryed(MapInfo::MI_BATTLESHIP)
		&& IsShipDestoryed(MapInfo::MI_CRUISER)
		&& IsShipDestoryed(MapInfo::MI_DESTORYER_1)
		&& IsShipDestoryed(MapInfo::MI_DESTORYER_2);
}