/*
	Name: ExampleWithErrorCheck
	Desc: BGNL ��� ���� �ڵ��Դϴ�. ��ټ��� ���� üũ�� ������ �ڵ��Դϴ�.
		  �� �ڵ忡���� �帧�� �����ϰ�, ������ ����� ���� ������ üũ�ؾ��մϴ�.
*/
#include <stdio.h>

#ifdef _DEBUG

// Debug�� �����ϴ� ���
#pragma comment(lib, "BGNL_debug.lib")

#else

// Release�� �����ϴ� ���
#pragma comment(lib, "BGNL.lib")

#endif

#include <Network.h>

// Dummy Functions
void MakeMapData(char* const mapData){}
void MakeAttackPos(int* const x, int* const y){}
void HandleMyAttackResult(const int attackResult, const int x, const int y){}
void HandleOpositionAttackResult(const int attackResult, const int x, const int y){}

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

	/*
		** ������ ����
		������ IP�� ��Ʈ�� ���ϳ� �����ȴ�.
	*/
	const char* ip = "127.0.0.1";
	const unsigned short port = 10000;

	network.Connect(ip, port);

	/*
		** �̸� ����
		�ִ� ���̴� MAX_NAME_LEN-1 == 15����.
	*/
	const wchar_t name[MAX_NAME_LEN] = L"�������";

	network.SubmitName(name);
	network.GetPacketType(&type);

	/*
		** ���� ���� ���
		PKT_SC_GAME_START ��Ŷ�� ��ٸ���.
	*/
	network.GetPacketType(&type);

	/*
		** ���� ����
		�� ������� ���� ������� nȸ �ݺ��Ѵ�.
		��� ������ ������ PKT_SC_ALL_OVER ��Ŷ�� ���´�.
	*/
	bool allOver = false;
	while (!allOver)
	{
		/*
			** �� ����
			�ڽ��� ��ġ�� �� �����͸� ������ �����Ѵ�.
			�� �����ʹ� char�� 32ũ�� �迭�̴�.
			�� �����Ϳ��� ����� ��ǥ�� ������� ����.
			��ǥ�� 0~7�� ���� 2���̸�,
			Aircraft 5��, Battleship 4��, Cruiser 3��, Destroyer 2���� ��ǥ�� ������.
			�� ��ǥ�� 16���̸� �̴� �� 32���� ������ �ȴ�.
		*/
		char mapData[MAP_SIZE] = { 0, };

		MakeMapData(mapData);
		network.SubmitMap(mapData);
		network.GetPacketType(&type);

		/*
			** ���� ����
			�� ���ʶ�� ���� ��ġ�� �����Ѵ�.
			���ʰ� ������ �����ڿ� ���� ��ġ, ���� ����� �޴´�.
			�� ������ ������ PKT_SC_GAME_OVER ��Ŷ�� ���´�.
		*/
		bool gameOver = false;
		while (!gameOver)
		{
			network.GetPacketType(&type);

			switch (type)
			{
				// �� ����
			case PKT_SC_MY_TURN:
			{
				/*
					** ���� ��ġ ����
					x, y�� 0~7 ������ �����̴�.
				*/
				int x, y;
				MakeAttackPos(&x, &y);
				network.SubmitAttack(x, y);
				network.GetPacketType(&type);	// OK�� ��ٸ��� �κ�
				break;
			}
				// ���� ���
			case PKT_SC_ATTACK_RESULT:
			{
				Network::AttackResult attackResult;
				network.GetAttackResult(&attackResult);
				if (attackResult.isMine)
					puts("���� ���:");
				else
					puts("�ǰ� ���:");
				printf_s("X: %d , Y: %d , RESULT: %s\n", attackResult.x, attackResult.y, ATTACK_RESULT_STR[attackResult.attackResult]);
				HandleOpositionAttackResult(attackResult.attackResult, attackResult.x, attackResult.y);
				break;
			}
				// ���� ����
			case PKT_SC_GAME_OVER:
			{
				Network::GameResult gameResult;
				network.GetGameResult(&gameResult);
				if (gameResult.isWinner)
					puts("�¸�!!!");
				else
					puts("����...");
				printf_s("�� ��: %d\n", gameResult.turns);
				gameOver = true;
				break;
			}
			}
		}

		network.GetPacketType(&type);
		if (type == PKT_SC_NEXT_GAME)
		{
			puts("���� ������ �غ����ּ���.");
		}
		else if (type == PKT_SC_ALL_OVER)
		{
			Network::FinalResult finalResult;
			network.GetFinalResult(&finalResult);
			puts("��� ����");
			printf_s("�¸� Ƚ��: %d, ��� �� ��: %.1f", finalResult.winCount, finalResult.avgTurns);

			allOver = true;
		}
	}


	/*
	** ���� ����
	*/
	network.Disconnect();

}