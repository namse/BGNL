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

AttackResultTypes Player::AttackAndGetResult(Coord coord)
{
	if (coord.mX < 0 || coord.mX > MAP_WIDTH
		|| coord.mY < 0 || coord.mY > MAP_HEIGHT)
	{
		//OH;;
		std::cout << "ERROR : Attack Range is over, X = " << coord.mX << ", Y = " << coord.mY << std::endl;
		return AR_NONE;
	}
	didAttacked_[coord.mX][coord.mY] = true;

	MapInfo shipType = info_[coord.mX][coord.mY];

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
				return AR_DESTROY_AIRCRAFT;
				break;
			case MI_BATTLESHIP:
				return AR_DESTROY_BATTLESHIP;
				break;
			case MI_CRUISER:
				return AR_DESTROY_CRUISER;
				break;
			case MI_DESTROYER_1:
			case MI_DESTROYER_2:
				return AR_DESTROY_DESTROYER;
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
		&& IsShipDestoryed(MapInfo::MI_DESTROYER_1)
		&& IsShipDestoryed(MapInfo::MI_DESTROYER_2);
}