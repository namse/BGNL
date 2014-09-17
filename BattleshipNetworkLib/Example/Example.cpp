/*
	Name: BGNL Example
	Desc: BGNL ��� ���� �ڵ��Դϴ�. �����ϸ� ������ �����մϴ�.
		AI�� ���� ��ġ, ���� �����Դϴ�.
*/

// Debug�� �����ϴ� ���
#ifdef _DEBUG
#pragma comment(lib, "BGNL_debug.lib")

// Release�� �����ϴ� ���
#else
#pragma comment(lib, "BGNL.lib")

#endif

#include <stdio.h>
#include <conio.h>
#include <time.h>
#include <locale.h>
#include <Network.h>
#include <ShipData.h>

// �� Windows.h�� ���⿡ �־�� ��! Network.h���� Windows.h�� ���� ��ũ ���� �߻���!

// Server IP & Port
const char*				IP		= "10.73.42.117";	// �ڱ� ��ǻ�Ϳ��� ���� ���� 127.0.0.1
const unsigned short	PORT	= 9001;

// Game Data
char gEnemyMap[MAP_WIDTH][MAP_HEIGHT];


// �� �Լ����� ���Ŀ� �ڽ��� �Լ���� ��ü�ؾ� �մϴ�.
void	Initialize();
void	MakeMapData(char* const mapData);	// �� �����͸� ���� ����� ���
void	MakeMapData2(char* const mapData);	// ShipData�� ���� ���
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
		** ��Ʈ��ũ �ʱ�ȭ
	*/
	try
	{
		Network::Initialize();
	}
	catch (Network::Exception)
	{
		puts("�ʱ�ȭ ���� ������ �߻��߽��ϴ�.");
		return;
	}

	/*
		** ������ ����
		������ IP�� ��Ʈ�� ���ϳ� �����ȴ�.
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
			puts("������ ���ῡ �����߽��ϴ�.");
			break;
		case Network::PARAMETER_ERROR:
			puts("�Լ��� �μ��� �߸��Ǿ����ϴ�.");
			break;
		}
		return;
	}
	puts("���� ����!");

	try	// ���� ó���� ���� try������ ��� ���Ѵ�.
	{
		/*
			** �̸�&�й� ����
			�ִ� ���̴� MAX_NAME_LEN-1 == 15����.
			������ ET_OK�� ���ϵȴ�.
			�̹� �ִ� �̸��� ���� ET_DUPLICATED_NAME�� �´�.
		*/
		const wchar_t name[MAX_NAME_LEN] = L"�������";
		const int studentID = 141031;
	
		error = network.SubmitName(name, studentID);
		if (error == ET_DUPLICATED_NAME)
		{
			puts("�̹� �����ϴ� �̸��Դϴ�.");
			return;
		}

		/*
			** ���� ���� ���
			������ ���۵Ǹ� ������ ������ ���´�.
			Network::GameStartData ����ü�� ���� ������ �����´�.
		*/
		Network::GameStartData gameStartData;
		puts("���� ���� �����");
		network.WaitForStart(&gameStartData);
		setlocale(LC_ALL, "korean");	// ��� ���� ������ �߰�.
		wprintf_s(L"��Ī�Ǿ����ϴ�. ���� �̸�: %s\n", gameStartData.oppositionName);
		Sleep(1000);
		/*
			** ���� ����
			�� ������� ���� ������� nȸ �ݺ��Ѵ�.
			�ϳ��� ������ ������ ���� ������ �����ؾ� �Ѵٸ� PKT_SC_NEXT_GAME ��Ŷ��
			��� ������ ������ PKT_SC_ALL_OVER ��Ŷ�� ���´�.
		*/
		bool allOver = false;
		while (!allOver)
		{
			// �ڽ��� �ʱ�ȭ �Լ��� ȣ���Ѵ�.
			Initialize();

			/*
				** �� ����
				�ڽ��� ��ġ�� �� �����͸� ������ �����Ѵ�.
				�� �����͸� ����� ������� �� ������ �ִ�.
				�ϳ��� ���� MAP_WIDHT * MAP_HEIGHT ũ���� ���� ����� ���̰�,
				�ϳ��� ShipData ����ü�� �̿��ؼ� ����� ���̴�.
			*/
			char mapData[MAP_SIZE];

			while (true)
			{
				// ���� ���� ����� ���
				MakeMapData(mapData);
				// ShipData Ŭ������ ���� ����� ���
				// MakeMapData2(mapData);

				error = network.SubmitMap(mapData);
				if (error == ET_INVALID_MAP)
					puts("��ȿ���� ���� �� �������Դϴ�.");
				else
					break;
			}

			/*
				** ���� ����
				�� ���ʶ�� ���� ��ġ�� �����Ѵ�.
				���ʰ� ������ �����ڿ� ���� ��ġ, ���� ����� �޴´�.
				�� ������ ������ PKT_SC_GAME_OVER ��Ŷ�� ���´�.
			*/
			bool gameOver = false;
			while (!gameOver)
			{
				error = network.GetPacketType(&type);

				switch (type)
				{
					// ������ �߻��ϴ� ���(������ ���� ����)
				case PKT_SC_ERROR:
					if (error == ET_OPPOSITION_DISCONNECTED)
						puts("������ ������ ���������ϴ�.");
					else
						puts("�� �� ���� �����Դϴ�.");
					return;

					// �� ����
				case PKT_SC_MY_TURN:
				{
					/*
						** ���� ��ġ ����
						x, y�� 0~7 ������ �����̴�.
					*/
					while (true)
					{
						// �ڽ��� ���� ��ġ ���� �Լ��� ����Ѵ�.
						Coord pos = MakeAttackPos();
						error = network.SubmitAttack(pos);
						if (error == ET_INVALID_ATTACK)
							puts("��ȿ���� ���� ���� ��ġ�Դϴ�.");
						else
							break;
					}
					break;
				}

					// ���� ���
				case PKT_SC_ATTACK_RESULT:
				{
					Network::AttackResultData attackResult = network.GetAttackResult();
					if (attackResult.isMine)
					{
						puts("���� ���:");
						// �ڽ��� ���� ��� ó�� �Լ��� ����Ѵ�.
						HandleMyAttackResult(attackResult.attackResult, attackResult.pos.mX, attackResult.pos.mY);
					}
					else
					{
						puts("�ǰ� ���:");
						// �ڽ��� ���� ��� ó�� �Լ��� ����Ѵ�.
						HandleOpositionAttackResult(attackResult.attackResult, attackResult.pos.mX, attackResult.pos.mY);
					}
					printf_s("X: %d, Y: %d, RESULT: %s\n", attackResult.pos.mX, attackResult.pos.mY, ATTACK_RESULT_STR[attackResult.attackResult]);
					break;
				}

					// ���� ����
				case PKT_SC_GAME_OVER:
				{
					Network::GameResultData gameResult = network.GetGameResult();
					if (gameResult.isWinner)
						puts("�¸�!!!");
					else
						puts("�й�...");
					printf_s("�� ��: %d\n", gameResult.turns);
					gameOver = true;
					break;
				}

				default:
					throw Network::UNEXPECTED_PACKET;
					break;
				}
			}

			/*
				** ������ ó��
			*/
			network.GetPacketType(&type);

			if (type == PKT_SC_NEXT_GAME)
			{
				puts("���� ������ �غ����ּ���.");
			}
			else if (type == PKT_SC_ALL_OVER)
			{
				Network::FinalResultData finalResult = network.GetFinalResult();
				puts("��� ����");
				printf_s("�¸� Ƚ��: %d, ��� �� ��: %.1f", finalResult.winCount, finalResult.avgTurns);

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
			puts("��Ʈ��ũ�� ������ �߻��߽��ϴ�.");
			break;
		case Network::SERVER_CLOSED:
			puts("�������� ������ ���������ϴ�.");
			break;
		case Network::PARAMETER_ERROR:
			puts("�Լ��� �μ��� �߸��Ǿ����ϴ�.");
			break;
		case Network::UNEXPECTED_PACKET:
			puts("�������� �߸��� ������ ���۵Ǿ����ϴ�.");
			break;
		default:
			break;
		}
	}


	/*
		** ���� ����
		����� �Ҹ�ÿ��� �ڵ����� Disconnect�� ȣ���Ѵ�.
	*/
	network.Disconnect();

	_getch();
}


void Initialize()
{
	ZeroMemory(gEnemyMap, sizeof(gEnemyMap));
}


// ���� �� �����͸� �����ϴ� ���
void MakeMapData(char* const mapData)
{
	ZeroMemory(mapData, MAP_SIZE);

	int size, sx, sy, dir, listIdx = 0;
	bool placeable;

	// �� ������ġ�� ������ ���� ����� ���
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


// ShipData ����ü�� ����ؼ� �� ������ �����ϴ� ���
void MakeMapData2(char* const mapData)
{
	ShipData shipData;

	char map[MAP_SIZE] = { 0, };

	int size, sx, sy, dir, listIdx = 0;
	bool placeable;
	Coord posArr[MAX_SHIP_LEN];	// ShipData�� ���� ���� ��ġ �迭

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

		// 1.2 ���� ��ǥ �迭�� �����ͼ�...
		// Coord* shipPosArr = shipData.GetShipCoordArray((ShipData::ShipType)type);

		for (int i = 0; i < size && placeable; ++i)
		{
			int x, y;
			Coord coord;
			if (dir == 0) { x = sx + i; y = sy; }
			else  { x = sx; y = sy + i; }
			map[x + y * MAP_WIDTH] = type;

			coord = Coord(x, y);
			// 1. ���� ��ǥ�� �ϳ��� �ִ� ���
			// 1.1. �Լ� ���
			shipData.SetShipCoord((ShipData::ShipType)type, i, coord);
			// 1.2. �迭�� �����ͼ� �ֱ�
			// shipPosArr[i] = coord;

			// 2. Coord �迭�� ����...
			// posArr[i] = coord;
		}

		// 2. ���� ��ǥ�� �迭�� �� ���� �ִ� ���
		// shipData.SetShip((ShipData::ShipType)type, posArr);

		// 3. ���� ���������� ���⸸ �ִ� ���
		// shipData.SetShip((ShipData::ShipType)type, Coord(sx, sy), dir == 0 ? ShipData::DIR_HORIZONTAL : ShipData::DIR_VERTICAL);
	}

	// ���� ��ġ�� �� �����ͷ� ��ȯ
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
	// �ƹ� �͵� ���� �ʴ´�...
}
