#include <crtdbg.h>
#include "ShipData.h"


const int ShipData::SHIP_LEN[MAX_SHIP_NUM + 1] = {
	0, AIRCRAFT_LEN, BATTLESHIP_LEN, CRUISER_LEN, DESTROYER_LEN, DESTROYER_LEN
};


ShipData::ShipData()
{
	memset(ship, 0, sizeof(ship));
}

Coord* ShipData::GetShipCoordArray(const ShipType shipType)
{
	return ship[shipType];
}

void ShipData::SetShipCoord(const ShipType shipType, const int index, const Coord coord)
{
	_ASSERT(index >= 0 && index < SHIP_LEN[shipType]);
	ship[shipType][index] = coord;
}

void ShipData::SetShip(const ShipType shipType, const Coord* const coordArr)
{
	_ASSERT(coordArr);
	for (int i = 0; i < SHIP_LEN[shipType]; ++i)
		ship[shipType][i] = coordArr[i];
}

void ShipData::SetShip(const ShipType shipType, const Coord startCoord, const Direction dir)
{
	Coord pos = startCoord;
	for (int i = 0; i < SHIP_LEN[shipType]; ++i)
	{
		ship[shipType][i] = pos;

		if (dir == DIR_HORIZONTAL)
			++pos.mX;
		else
			++pos.mY;
	}
}

void ShipData::ToMapData(char* const mapData)
{
	memset(mapData, 0, MAP_SIZE);
	for (int type = MD_NONE + 1; type < MAX_SHIP_NUM; ++type)
	{
		Coord* arr = ship[type];
		for (int j = 0; j < SHIP_LEN[type]; ++j)
		{
			Coord pos = arr[j];
			mapData[pos.mX + pos.mY * MAP_WIDTH] = type;
		}
	}
}