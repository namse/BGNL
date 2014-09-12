/*
	Name: ExampleWithErrorCheck
	Desc: BGNL ��� ���� �ڵ��Դϴ�. ������ ���������� �ʽ��ϴ�.
		������ �帧�� ���ñ� �ٶ��ϴ�.
*/

#ifdef _DEBUG

// Debug�� �����ϴ� ���
#pragma comment(lib, "BGNL_debug.lib")

#else

// Release�� �����ϴ� ���
#pragma comment(lib, "BGNL.lib")

#endif

#include <stdio.h>
#include <Network.h>

// Dummy Functions
void MakeMapData(void* const mapData){}
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
	ErrorType error;

	/*
		** ������ ����
		������ IP�� ��Ʈ�� ���ϳ� �����ȴ�.
	*/
	const char* ip = "127.0.0.1";
	const unsigned short port = 10000;

	try
	{
		network.Connect(ip, port);
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
			** �̸� ����
			�ִ� ���̴� MAX_NAME_LEN-1 == 15����.
			������ ET_OK�� ���ϵȴ�.
			�̹� �ִ� �̸��� ���� ET_DUPLICATED_NAME�� �´�.
		*/
		const wchar_t name[MAX_NAME_LEN] = L"�������";
	
		error = network.SubmitName(name);
		if (error == ET_DUPLICATED_NAME)
		{
			puts("�̹� �����ϴ� �̸��Դϴ�.");
			return;
		}

		/*
			** ���� ���� ���
			PKT_SC_START_GAME ��Ŷ�� ��ٸ���.
		*/
		puts("���� ���� �����");
		network.WaitSpecPacket(PKT_SC_GAME_START);

		/*
			** ���� ����
			�� ������� ���� ������� nȸ �ݺ��Ѵ�.
			�ϳ��� ������ ������ ���� ������ �����ؾ� �Ѵٸ� PKT_SC_NEXT_GAME ��Ŷ��
			��� ������ ������ PKT_SC_ALL_OVER ��Ŷ�� ���´�.
		*/
		bool allOver = false;
		while (!allOver)
		{
			/*
				** �� ����
				�ڽ��� ��ġ�� �� �����͸� ������ �����Ѵ�.
				�� �����ʹ� char�� 32ũ�� �迭�̴�.
				Aircraft���� ������� ���� ��ǥ���� �ִ´�.
			*/
			Network::MapData mapData;

			while (true)
			{
				MakeMapData(&mapData);

				error = network.SubmitMap(&mapData);
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
						int x, y;
						MakeAttackPos(&x, &y);
						error = network.SubmitAttack(x, y);
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
				Network::FinalResult finalResult;
				network.GetFinalResult(&finalResult);
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
}