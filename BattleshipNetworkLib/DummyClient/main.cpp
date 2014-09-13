/*
	Name: Dummy Client
	Desc: 더미 클라이언트. AI는 그냥 랜덤 배치 랜덤 공격.
		한 번에 여러 클라이언트를 생성한다. CLIENT_NUM 만큼 클라이언트를 만든다.
		사용법은 DummyClient <IP> <PORT>
*/


#include <stdio.h>
#include <wchar.h>
#include <process.h>
#include <time.h>
#include <conio.h>

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

void main(const int argc, const char* const * const argv)
{
	HANDLE hThread[CLIENT_NUM] = { 0, };
	IPAndPort ipNport = { 0, };

	if (argc != 3)
	{
		printf_s("USAGE: %s <IP> <PORT>\n", argv[1]);
		return;
	}

	strcpy_s(ipNport.ip, argv[1]);
	ipNport.port = atoi(argv[2]);

	Network::Initialize();

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

	puts("끝");
	_getch();
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

void MakeAttackPos(const char* const enemyMap, int* const x, int* const y)
{
	do
	{
		*x = rand() % 8;
		*y = rand() % 8;
	} while (!enemyMap[*x + (*y) * 8]);
}

void HandleOpositionAttackResult(char* const enemyMap, const short result, const int x, const int y)
{
	enemyMap[x + y * 8] = 1;
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

	swprintf_s(name, L"NAME%d", GetCurrentThreadId());

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
				PacketType type;
				ErrorType error;

				Log("게임 준비");
				PlaceShip(shipPosList);
				if (network.SubmitMap(shipPosList) == ET_INVALID_MAP)
				{
					Log("맵 생성에 문제 있음");
					return 0;
				}

				bool gameOver = false;

				while (!gameOver)
				{
					error = network.GetPacketType(&type);
					switch (type)
					{
					case PKT_SC_ERROR:
						Log("상대방 접속 종료");
						return 0;

					case PKT_SC_MY_TURN:
					{
						/*
						** 공격 위치 전송
						x, y는 0~7 사이의 정수이다.
						*/
						while (true)
						{
							int x, y;
							MakeAttackPos(enemyMap, &x, &y);
							error = network.SubmitAttack(x, y);
							if (error == ET_INVALID_ATTACK)
								Log("유효하지 않은 공격");
							else
								break;
						}
						break;
					}

					case PKT_SC_ATTACK_RESULT:
					{
						Network::AttackResult attackResult;
						network.GetAttackResult(&attackResult);
						if (attackResult.isMine)
							HandleOpositionAttackResult(enemyMap, attackResult.attackResult, attackResult.x, attackResult.y);
						break;
					}

					case PKT_SC_GAME_OVER:
					{
						Network::GameResult gameResult;
						network.GetGameResult(&gameResult);
						gameOver = true;
						break;
					}

					default:
						throw Network::UNEXPECTED_PACKET;
						break;
					}
				}

				network.GetPacketType(&type);

				if (type == PKT_SC_ALL_OVER)
				{
					Network::FinalResult finalResult;
					network.GetFinalResult(&finalResult);
					allOver = true;
				}
				else
					throw Network::UNEXPECTED_PACKET;
			}
		}
		catch (Network::Exception)
		{
			Log("에러");
			return 0;
		}

		Log("게임종료");

	}

	return 0;
}
