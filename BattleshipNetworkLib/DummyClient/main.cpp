#include <stdio.h>
#include <wchar.h>
#include <process.h>
#include <time.h>

#ifdef _DEBUG
#pragma comment(lib, "BGNL_debug.lib")
#else
#pragma comment(lib, "BGNL.lib")
#endif
#include "Network.h"
#include <Windows.h>

#define CLIENT_NUM 16

struct IPAndPort
{
	char ip[16];
	unsigned short port;
};

CRITICAL_SECTION cs;

unsigned int _stdcall ClientThread(void* param);

void main()
{
	HANDLE hThread[CLIENT_NUM] = { 0, };
	IPAndPort ipNport = { 0, };

	InitializeCriticalSection(&cs);

	for (int i = 0; i < CLIENT_NUM; ++i)
	{
		hThread[i] = (HANDLE)_beginthreadex(NULL, 0, ClientThread, &ipNport, 0, NULL);
		if (hThread[i] == INVALID_HANDLE_VALUE)
			continue;
	}

	WaitForMultipleObjects(CLIENT_NUM, hThread, TRUE, INFINITE);

	for (HANDLE& handle : hThread)
	{
		CloseHandle(handle);
	}

	DeleteCriticalSection(&cs);
}

void PlaceShip(char* const shipPosList)
{
	static const int shipSizeArr[] = { 5, 4, 3, 2, 2 };

	char map[64] = { 0, };
	int size, sx, sy, dir, listIdx = 0;
	bool placeable;
	ZeroMemory(map, 8 * 8);

	for (int k = 1; k <= 5; ++k)
	{
		size = shipSizeArr[k];
		placeable = true;
		dir = rand() % 2;
		if (dir == 0) // hori
		{
			sx = rand() % (8 - size);
			sy = rand() % 8;
		}
		else // vert
		{
			sx = rand() % 8;
			sy = rand() % (8 - size);
		}

		for (int i = 0; i < size && placeable; ++i)
		{
			if (dir == 0 && map[sx + i + sy * 8])
				placeable = false;
			else if (dir == 1 && map[sx + (sy + i) * 8])
				placeable = false;
		}

		if (!placeable)
			continue;

		for (int i = 0; i < size && placeable; ++i)
		{
			int x, y;
			if (dir == 0) { x = sx + i; y = sy; }
			else  { x = sx; y = sy + i; }
			map[x+y*8] = k;
			shipPosList[listIdx++] = x;
			shipPosList[listIdx++] = y;
		}
	}
}

void Log(const char* const message)
{
	EnterCriticalSection(&cs);
	fprintf_s(stderr, "%u: %s\n", GetCurrentThreadId(), message);
	LeaveCriticalSection(&cs);
}

unsigned int _stdcall ClientThread(void* param)
{
	IPAndPort* ipNport = (IPAndPort*)param;
	wchar_t name[MAX_NAME_LEN] = { 0, };
	char shipPosList[32] = { 0, };
	char enemyMap[8*8] = { 0, };

	srand((unsigned)time(NULL) ^ GetCurrentThreadId()<< 2);

	swprintf_s(name, L"NAME%d", rand()%100);

	Log("스레드 시작됨");

	{
		Network network;

		try
		{
			network.Connect(ipNport->ip, ipNport->port);
		}
		catch (Network::Exception)
		{
			Log("연결 실패");
			return 0;
		}

		Log("연결됨");

		try
		{
			if (network.SubmitName(name) == ET_DUPLICATED_NAME)
			{
				Log("이름 중복");
				return 0;
			}

			network.WaitSpecPacket(PKT_SC_GAME_START);

			Log("게임 시작됨");

			bool allOver = false;
			while (!allOver)
			{
				Log("게임 준비");
				PlaceShip(shipPosList);
				if (network.SubmitMap(shipPosList) == ET_INVALID_MAP)
				{
					Log("맵 생성에 문제 있음");
					return 0;
				}
			}
		}
		catch (Network::Exception)
		{
			Log("에러");
			return 0;
		}

	}

	return 0;
}
