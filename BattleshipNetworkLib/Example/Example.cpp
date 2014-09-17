/*
	Name: BGNL Example
	Desc: BGNL 사용 예시 코드입니다. 빌드하면 실제로 동작합니다.
		AI는 랜덤 배치, 랜덤 공격입니다.
*/

// Debug로 빌드하는 경우
#ifdef _DEBUG
#pragma comment(lib, "BGNL_debug.lib")

// Release로 빌드하는 경우
#else
#pragma comment(lib, "BGNL.lib")

#endif

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <Network.h>
#include <ShipData.h>

// ※ Windows.h는 여기에 넣어야 됨! Network.h위에 Windows.h가 들어가면 링크 에러 발생함!

// Server IP & Port
const char*				IP		= "10.73.42.117";	// 자기 컴퓨터에서 돌릴 때는 127.0.0.1
const unsigned short	PORT	= 9001;

// Game Data
char gEnemyMap[MAP_WIDTH][MAP_HEIGHT];


// 이 함수들은 이후에 자신의 함수들로 대체해야 합니다.
void	Initialize();
void	MakeMapData(char* const mapData);	// 맵 데이터를 직접 만드는 방식
void	MakeMapData2(char* const mapData);	// ShipData를 쓰는 방식
Coord	MakeAttackPos();
void	HandleMyAttackResult(const int attackResult, const int x, const int y);
void	HandleOpositionAttackResult(const int attackResult, const int x, const int y);


// Attack result string
const char* const ATTACK_RESULT_STR[] = {
	"No Result",
	"Miss",
	"Hit",
	"Aircraft Destroyed",
	"Battleship Destroyed",
	"Cruiser Destroyed",
	"Destroyer Destroyed",
};


void main()
{
	Network network;
	PacketType type;
	ErrorType error;

	srand((unsigned int)time(NULL));

	/*
		** 네트워크 초기화
	*/
	try
	{
		Network::Initialize();
	}
	catch (Network::Exception)
	{
		puts("초기화 도중 문제가 발생했습니다.");
		return;
	}

	/*
		** 서버에 연결
		서버의 IP와 포트는 당일날 공지된다.
	*/

	try
	{
		network.Connect(IP, PORT);
	}
	catch (Network::Exception ex)
	{
		switch (ex)
		{
		case Network::NETWORK_ERROR:
			puts("서버와 연결에 실패했습니다.");
			break;
		case Network::PARAMETER_ERROR:
			puts("함수의 인수가 잘못되었습니다.");
			break;
		}
		return;
	}
	puts("접속 성공!");

	try	// 예외 처리를 위해 try문으로 모두 감싼다.
	{
		/*
			** 이름&학번 전송
			최대 길이는 MAX_NAME_LEN-1 == 15글자.
			성공시 ET_OK가 리턴된다.
			이미 있는 이름을 쓰면 ET_DUPLICATED_NAME이 온다.
		*/
		const wchar_t name[MAX_NAME_LEN] = L"와츄고나두";
		const int studentID = 141031;
	
		error = network.SubmitName(name, studentID);
		if (error == ET_DUPLICATED_NAME)
		{
			puts("이미 존재하는 이름입니다.");
			return;
		}

		/*
			** 게임 시작 대기
			게임이 시작되면 상대방의 정보가 들어온다.
			Network::GameStartData 구조체로 상대방 정보를 가져온다.
		*/
		Network::GameStartData gameStartData;
		puts("게임 시작 대기중");
		network.WaitForStart(&gameStartData);
		setlocale(LC_ALL, "korean");	// 출력 문제 때문에 추가.
		wprintf_s(L"매칭되었습니다. 상대방 이름: %s\n", gameStartData.oppositionName);
		Sleep(1000);
		/*
			** 게임 시작
			맵 제출부터 게임 종료까지 n회 반복한다.
			하나의 게임이 끝나고 다음 게임을 시작해야 한다면 PKT_SC_NEXT_GAME 패킷이
			모든 게임이 끝나면 PKT_SC_ALL_OVER 패킷이 들어온다.
		*/
		bool allOver = false;
		while (!allOver)
		{
			// 자신의 초기화 함수를 호출한다.
			Initialize();

			/*
				** 맵 제출
				자신이 배치한 맵 데이터를 서버로 전송한다.
				맵 데이터를 만드는 방법에는 두 가지가 있다.
				하나는 직접 MAP_WIDHT * MAP_HEIGHT 크기의 맵을 만드는 것이고,
				하나는 ShipData 구조체를 이용해서 만드는 것이다.
			*/
			char mapData[MAP_SIZE];

			while (true)
			{
				// 직접 맵을 만드는 방법
				MakeMapData(mapData);
				// ShipData 클래스로 맵을 만드는 방법
				// MakeMapData2(mapData);

				error = network.SubmitMap(mapData);
				if (error == ET_INVALID_MAP)
					puts("유효하지 않은 맵 데이터입니다.");
				else
					break;
			}

			/*
				** 게임 루프
				내 차례라면 공격 위치를 전송한다.
				차례가 끝나면 공격자와 공격 위치, 공격 결과를 받는다.
				한 게임이 끝나면 PKT_SC_GAME_OVER 패킷이 들어온다.
			*/
			bool gameOver = false;
			while (!gameOver)
			{
				error = network.GetPacketType(&type);

				switch (type)
				{
					// 에러가 발생하는 경우(상대방의 접속 종료)
				case PKT_SC_ERROR:
					if (error == ET_OPPOSITION_DISCONNECTED)
						puts("상대방의 접속이 끊어졌습니다.");
					else
						puts("알 수 없는 에러입니다.");
					return;

					// 내 차례
				case PKT_SC_MY_TURN:
				{
					/*
						** 공격 위치 전송
						x, y는 0~7 사이의 정수이다.
					*/
					while (true)
					{
						// 자신의 공격 위치 제작 함수를 사용한다.
						Coord pos = MakeAttackPos();
						error = network.SubmitAttack(pos);
						if (error == ET_INVALID_ATTACK)
							puts("유효하지 않은 공격 위치입니다.");
						else
							break;
					}
					break;
				}

					// 공격 결과
				case PKT_SC_ATTACK_RESULT:
				{
					Network::AttackResultData attackResult = network.GetAttackResult();
					if (attackResult.isMine)
					{
						puts("공격 결과:");
						// 자신의 공격 결과 처리 함수를 사용한다.
						HandleMyAttackResult(attackResult.attackResult, attackResult.pos.mX, attackResult.pos.mY);
					}
					else
					{
						puts("피격 결과:");
						// 자신의 공격 결과 처리 함수를 사용한다.
						HandleOpositionAttackResult(attackResult.attackResult, attackResult.pos.mX, attackResult.pos.mY);
					}
					printf_s("X: %d, Y: %d, RESULT: %s\n", attackResult.pos.mX, attackResult.pos.mY, ATTACK_RESULT_STR[attackResult.attackResult]);
					break;
				}

					// 게임 종료
				case PKT_SC_GAME_OVER:
				{
					Network::GameResultData gameResult = network.GetGameResult();
					if (gameResult.isWinner)
						puts("승리!!!");
					else
						puts("패배...");
					printf_s("턴 수: %d\n", gameResult.turns);
					gameOver = true;
					break;
				}

				default:
					throw Network::UNEXPECTED_PACKET;
					break;
				}
			}

			/*
				** 종료후 처리
			*/
			network.GetPacketType(&type);

			if (type == PKT_SC_NEXT_GAME)
			{
				puts("다음 게임을 준비해주세요.");
			}
			else if (type == PKT_SC_ALL_OVER)
			{
				Network::FinalResultData finalResult = network.GetFinalResult();
				puts("모두 종료");
				printf_s("승리 횟수: %d, 평균 턴 수: %.1f", finalResult.winCount, finalResult.avgTurns);

				allOver = true;
			}
			else
				throw Network::UNEXPECTED_PACKET;
		}
	}
	catch (Network::Exception ex)
	{
		switch (ex)
		{
		case Network::NETWORK_ERROR:
			puts("네트워크에 문제가 발생했습니다.");
			break;
		case Network::SERVER_CLOSED:
			puts("서버와의 연결이 끊어졌습니다.");
			break;
		case Network::PARAMETER_ERROR:
			puts("함수의 인수가 잘못되었습니다.");
			break;
		case Network::UNEXPECTED_PACKET:
			puts("서버에서 잘못된 정보가 전송되었습니다.");
			break;
		default:
			break;
		}
	}


	/*
		** 연결 종료
		참고로 소멸시에도 자동으로 Disconnect를 호출한다.
	*/
	network.Disconnect();

	_getch();
}


void Initialize()
{
	ZeroMemory(gEnemyMap, sizeof(gEnemyMap));
}


// 직접 맵 데이터를 생성하는 방법
void MakeMapData(char* const mapData)
{
	ZeroMemory(mapData, MAP_SIZE);

	int size, sx, sy, dir, listIdx = 0;
	bool placeable;

	// 배 랜덤배치로 지도를 직접 만드는 방식
	for (int type = MD_NONE + 1; type < MD_END; ++type)
	{
		size = ShipData::SHIP_LEN[type];
		while (true)
		{
			placeable = true;
			dir = rand() % 2;
			if (dir == 0) // hori
			{
				sx = rand() % (MAP_WIDTH - size);
				sy = rand() % MAP_HEIGHT;
			}
			else // vert
			{
				sx = rand() % MAP_WIDTH;
				sy = rand() % (MAP_HEIGHT - size);
			}

			for (int i = 0; i < size && placeable; ++i)
			{
				if (dir == 0 && mapData[sx + i + sy * MAP_WIDTH])
					placeable = false;
				else if (dir == 1 && mapData[sx + (sy + i) * MAP_WIDTH])
					placeable = false;
			}

			if (placeable)
				break;
		}

		for (int i = 0; i < size && placeable; ++i)
		{
			int x, y;
			if (dir == 0) { x = sx + i; y = sy; }
			else  { x = sx; y = sy + i; }
			mapData[x + y * MAP_WIDTH] = type;
		}
	}
}


// ShipData 구조체를 사용해서 맵 데이터 생성하는 방법
void MakeMapData2(char* const mapData)
{
	ShipData shipData;

	char map[MAP_SIZE] = { 0, };

	int size, sx, sy, dir, listIdx = 0;
	bool placeable;
	Coord posArr[MAX_SHIP_LEN];	// ShipData에 넣을 배의 위치 배열

	for (int type = MD_NONE + 1; type < MD_END; ++type)
	{
		size = ShipData::SHIP_LEN[type];
		while (true)
		{
			placeable = true;
			dir = rand() % 2;
			if (dir == 0) // hori
			{
				sx = rand() % (MAP_WIDTH - size);
				sy = rand() % MAP_HEIGHT;
			}
			else // vert
			{
				sx = rand() % MAP_WIDTH;
				sy = rand() % (MAP_HEIGHT - size);
			}

			for (int i = 0; i < size && placeable; ++i)
			{
				if (dir == 0 && map[sx + i + sy * MAP_WIDTH])
					placeable = false;
				else if (dir == 1 && map[sx + (sy + i) * MAP_WIDTH])
					placeable = false;
			}

			if (placeable)
				break;
		}

		// 1.2 배의 좌표 배열을 가져와서...
		// Coord* shipPosArr = shipData.GetShipCoordArray((ShipData::ShipType)type);

		for (int i = 0; i < size && placeable; ++i)
		{
			int x, y;
			Coord coord;
			if (dir == 0) { x = sx + i; y = sy; }
			else  { x = sx; y = sy + i; }
			map[x + y * MAP_WIDTH] = type;

			coord = Coord(x, y);
			// 1. 배의 좌표를 하나씩 넣는 방법
			// 1.1. 함수 사용
			shipData.SetShipCoord((ShipData::ShipType)type, i, coord);
			// 1.2. 배열을 가져와서 넣기
			// shipPosArr[i] = coord;

			// 2. Coord 배열을 만들어서...
			// posArr[i] = coord;
		}

		// 2. 배의 좌표를 배열로 한 번에 넣는 방법
		// shipData.SetShip((ShipData::ShipType)type, posArr);

		// 3. 배의 시작지점과 방향만 넣는 방법
		// shipData.SetShip((ShipData::ShipType)type, Coord(sx, sy), dir == 0 ? ShipData::DIR_HORIZONTAL : ShipData::DIR_VERTICAL);
	}

	// 배의 배치를 맵 데이터로 변환
	shipData.ToMapData(mapData);
}


Coord MakeAttackPos()
{
	Coord pos;
	do
	{
		pos.mX = rand() % MAP_WIDTH;
		pos.mY = rand() % MAP_HEIGHT;
	} while (gEnemyMap[pos.mX][pos.mY]);

	return pos;
}


void HandleMyAttackResult(const int attackResult, const int x, const int y)
{
	gEnemyMap[x][y] = 1;
}

void HandleOpositionAttackResult(const int attackResult, const int x, const int y)
{
	// 아무 것도 하지 않는다...
}
