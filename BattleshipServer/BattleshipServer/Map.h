#pragma once
#include "stdafx.h"

class Map
{
public:
	Map();
	~Map();

	ATTACK_RESULT AttackAndGetResult(int x, int y);

private:
	bool IsShipDestoryed(MAP_INFO shipType);
private:
	MAP_INFO info_[MAP_WIDTH][MAP_HEIGHT];
	bool didAttacked_[MAP_WIDTH][MAP_HEIGHT];
};

