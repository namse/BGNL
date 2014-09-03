/*
	Name: ExampleWithErrorCheck
	Desc: BGNL ��� ���� �ڵ��Դϴ�. ����üũ �ڵ尡 ��� �� �����Դϴ�.
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
	Network::NetworkResult result;
	Network::PacketType type;
	Network::ErrorType error;

	/*
		** ������ ����
		������ IP�� ��Ʈ�� ���ϳ� �����ȴ�.
	*/
	const char* ip = "127.0.0.1";
	const unsigned short port = 10000;

	result = network.Connect(ip, port);
	if (result == Network::NETWORK_ERROR)
	{
		puts("������ ���ῡ �����߽��ϴ�.");
		return;
	}

	/*
		** �̸� ����
		�ִ� ���̴� NAME_LEN-1 == 15����.
		������ PT_OK�� �´�.
		�̹� �ִ� �̸��� ���� PT_ERROR�� �Բ� ERROR_DUPLICATED_NAME�� �´�.
	*/
	const wchar_t name[NAME_LEN] = L"�������";
	
	while (true)
	{
		result = network.SubmitName(name);
		if (result == Network::NETWORK_ERROR)
		{
			puts("�̸� ���� ����");
			return;
		}

		result = network.GetPacketType(&type);
		if (result == Network::NETWORK_ERROR)
		{
			puts("��Ŷ �������� ����");
			return;
		}
		else if (result == Network::NETWORK_CLOSED)
		{
			puts("�������� ������ ���������ϴ�.");
			return;
		}

		if (type == Network::PT_OK)
			break;
		else if (type == Network::PT_ERROR)
		{
			network.GetErrorType(&error);
			if (error == Network::ERROR_DUPLICATED_NAME)
				puts("�̹� �����ϴ� �̸��Դϴ�.");
			else
				puts("�� �� ���� �����Դϴ�.");
		}
		else
		{
			puts("���� �߸��Ǿ���. �� ���� ���������߰ھ�.");
			return;
		}
	}

	/*
		** ���� ���� ���
		PT_START_GAME ��Ŷ�� ��ٸ���.
	*/
	result = network.GetPacketType(&type);
	if (result == Network::NETWORK_ERROR)
	{
		puts("��Ŷ �������� ����");
		return;
	}
	else if (result == Network::NETWORK_CLOSED)
	{
		puts("�������� ������ ���������ϴ�.");
		return;
	}
	// ���� ���� ��Ŷ�� �;��Ѵ�. ���� ���� ��Ŷ�� �ƴϸ� ������ �ִ� �Ŵ�.
	if (type != Network::PT_GAME_START)
	{
		puts("���� �߸��Ǿ���. �� ���� ���������߰ھ�.");
		return;
	}

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

		while (true)
		{
			MakeMapData(mapData);

			result = network.SubmitMap(mapData);
			if (result == Network::NETWORK_ERROR)
			{
				puts("�� ������ ���� ����");
				return;
			}

			result = network.GetPacketType(&type);
			if (result == Network::NETWORK_ERROR)
			{
				puts("��Ʈ��ũ ����");
				return;
			}
			else if (result == Network::NETWORK_CLOSED)
			{
				puts("�������� ������ ���������ϴ�.");
				return;
			}

			if (type == Network::PT_OK)
				break;
			else if (type == Network::PT_ERROR)
			{
				network.GetErrorType(&error);
				if (error == Network::ERROR_INVALID_MAP)
					puts("��ȿ���� ���� �� �������Դϴ�.");
				else
					puts("�� �� ���� �����Դϴ�.");
			}
			else
			{
				puts("���� �߸��Ǿ���. �� ���� ���������߰ھ�.");
				return;
			}
		}

		/*
			** ���� ����
			�� ���ʶ�� ���� ��ġ�� �����Ѵ�.
			���ʰ� ������ �����ڿ� ���� ��ġ, ���� ����� �޴´�.
			�� ������ ������ PT_GAME_OVER ��Ŷ�� ���´�.
		*/
		bool gameOver = false;
		while (!gameOver)
		{
			result = network.GetPacketType(&type);
			if (result == Network::NETWORK_ERROR)
			{
				puts("��Ŷ �������� ����");
				return;
			}
			else if (result == Network::NETWORK_CLOSED)
			{
				puts("�������� ������ ���������ϴ�.");
				return;
			}

			switch (type)
			{
				// ������ �߻��ϴ� ���(������ ���� ����)
			case Network::PT_ERROR:
				network.GetErrorType(&error);
				if (error == Network::ERROR_OPPOSITION_DISCONNECTED)
					puts("������ ������ ���������ϴ�.");
				else
					puts("�� �� ���� �����Դϴ�.");
				return;
				// �� ����
			case Network::PT_MY_TURN:
			{
				/*
					** ���� ��ġ ����
					x, y�� 0~7 ������ �����̴�.
				*/
				while (true)
				{
					int x, y;
					MakeAttackPos(&x, &y);
					result = network.SubmitAttack(x, y);
					if (result == Network::NETWORK_ERROR)
					{
						puts("���� ��ġ ���� ����");
						return;
					}

					result = network.GetPacketType(&type);
					if (result == Network::NETWORK_ERROR)
					{
						puts("��Ŷ �������� ����");
						return;
					}
					else if (result == Network::NETWORK_CLOSED)
					{
						puts("�������� ������ ���������ϴ�.");
						return;
					}

					if (type == Network::PT_OK)
						break;
					if (type == Network::PT_ERROR)
					{
						network.GetErrorType(&error);
						if (error == Network::ERROR_INVALID_ATTACK)
							puts("��ȿ���� ���� ���� ��ġ�Դϴ�.");
						else
							puts("�� �� ���� �����Դϴ�.");
					}
				}
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

			default:
				printf_s("�߸��� ��Ŷ�� ���Խ��ϴ�: %d\n", type);
				break;
			}
		}

		result = network.GetPacketType(&type);
		if (result == Network::NETWORK_ERROR)
		{
			puts("��Ŷ �������� ����");
			return;
		}
		else if (result == Network::NETWORK_CLOSED)
		{
			puts("�������� ������ ���������ϴ�.");
			return;
		}

		if (type == Network::PT_NEXT_GAME)
		{
			puts("���� ������ �غ����ּ���.");
		}
		else if (type == Network::PT_ALL_OVER)
		{
			Network::FinalResultPacket finalResult;
			network.GetFinalResult(&finalResult);
			puts("��� ����");
			printf_s("�¸� Ƚ��: %d, ��� �� ��: %.1f", finalResult.winCount, finalResult.avgTurns);

			allOver = true;
		}
		else
		{
			puts("���� �߸��Ǿ���. �� ���� ���������߰ھ�.");
			return;
		}
	}


	/*
		** ���� ����
	*/
	network.Disconnect();
}