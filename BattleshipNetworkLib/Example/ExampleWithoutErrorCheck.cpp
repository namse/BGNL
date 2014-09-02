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
	Network::PacketType type;

	/*
	** ������ ����
	������ IP�� ��Ʈ�� ���ϳ� �����ȴ�.
	*/
	const char* ip = "127.0.0.1";
	const unsigned short port = 10000;

	network.Connect(ip, port);

	/*
	** �̸� ����
	�ִ� ���̴� NAME_LEN-1 == 15����.
	������ PT_OK�� �´�.
	�̹� �ִ� �̸��� ���� PT_ERROR�� �Բ� ERROR_DUPLICATED_NAME�� �´�.
	*/
	const wchar_t name[NAME_LEN] = L"�������";

	network.SubmitName(name);
	network.GetPacketType(&type);

	/*
	** ���� ���� ���
	PT_START_GAME ��Ŷ�� ��ٸ���.
	*/
	network.GetPacketType(&type);

	/*
	** ���� ����
	�� ������� ���� ������� nȸ �ݺ��Ѵ�.
	��� ������ ������ PT_ALL_OVER ��Ŷ�� ���´�.
	*/
	bool allOver = false;
	while (!allOver)
	{
		/*
		** �� ����
		�ڽ��� ��ġ�� �� �����͸� ������ �����Ѵ�.
		�� �����ʹ� char�� 64ũ�� �迭�̴�.
		�� �����Ϳ� ���� ���� MapData �������� ����ִ�.
		*/
		char mapData[MAP_SIZE] = { 0, };

		MakeMapData(mapData);
		network.SubmitMap(mapData);
		network.GetPacketType(&type);

		/*
		** ���� ����
		�� ���ʶ�� ���� ��ġ�� �����Ѵ�.
		���ʰ� ������ �����ڿ� ���� ��ġ, ���� ����� �޴´�.
		�� ������ ������ PT_GAME_OVER ��Ŷ�� ���´�.
		*/
		bool gameOver = false;
		while (!gameOver)
		{
			network.GetPacketType(&type);

			switch (type)
			{
				// �� ����
			case Network::PT_MY_TURN:
			{
				/*
				** ���� ��ġ ����
				x, y�� 0~7 ������ �����̴�.
				*/
				int x, y;
				MakeAttackPos(&x, &y);
				network.SubmitAttack(x, y);
				network.GetPacketType(&type);
				break;
			}
				// ���� ���
			case Network::PT_ATTACK_RESULT:
			{
				Network::AttackResultPacket attackResult;
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
			case Network::PT_GAME_OVER:
			{
				Network::GameResultPacket gameResult;
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
		if (type == Network::PT_NEXT_GAME)
		{
			puts("���� ������ �غ����ּ���.");
		}
		else if (type == Network::PT_ALL_OVER)
		{
			Network::FinalResultPacket finalResult;
			network.GetFinalResult(&finalResult);
			puts("��� ����");
			printf_s("�¸� Ƚ��: %d, ��� �� ��: %d", finalResult.winCount, finalResult.avgTurns);

			allOver = true;
		}
	}


	/*
	** ���� ����
	*/
	network.Disconnect();

}