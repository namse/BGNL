#include "stdafx.h"
#include "Map.h"


Map::Map()
{
	memset(info_, MAP_INFO::EMPTY, sizeof(info_));
	memset(didAttacked_, false, sizeof(didAttacked_));
}


Map::~Map()
{
}

ATTACK_RESULT Map::AttackAndGetResult(int x, int y)
{
	if (x < 0 || x > MAP_WIDTH
		|| y < 0 || y > MAP_HEIGHT)
	{
		//OH;;
		std::cout << "ERROR : Attack Range is over, X = " << x << ", Y = " << y << std::endl;
		return ATTACK_RESULT::NONE;
	}
	didAttacked_[x][y] = true;

	MAP_INFO shipType = info_[x][y];

	if (shipType == MAP_INFO::EMPTY)
	{
		return ATTACK_RESULT::MISS;
	}
	else
	{
		if (IsShipDestoryed(shipType) == true)
		{
			switch (shipType)
			{
			case AIRCRAFT:
				return ATTACK_RESULT::DESTORY_AIRCRAFT;
				break;
			case BATTLESHIP:
				return ATTACK_RESULT::DESTORY_BATTLESHIP;
				break;
			case CRUSIER:
				return ATTACK_RESULT::DESTORY_CRUSIER;
				break;
			case DESTORYER_1:
			case DESTORYER_2:
				return ATTACK_RESULT::DESTORY_DESTORYER;
				break;
			default:
				return ATTACK_RESULT::NONE;
				break;
			}
		}
		else
		{
			return ATTACK_RESULT::HIT;
		}
	}
}

bool Map::IsShipDestoryed(MAP_INFO shipType)
{
	if (shipType == MAP_INFO::EMPTY)
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
