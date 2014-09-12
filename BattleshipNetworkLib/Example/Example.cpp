/*
	Name: ExampleWithErrorCheck
	Desc: BGNL 사용 예시 코드입니다. 실제로 동작하지는 않습니다.
		로직의 흐름만 보시기 바랍니다.
*/

#ifdef _DEBUG

// Debug로 빌드하는 경우
#pragma comment(lib, "BGNL_debug.lib")

#else

// Release로 빌드하는 경우
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
		** 서버에 연결
		서버의 IP와 포트는 당일날 공지된다.
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
			** 이름 전송
			최대 길이는 MAX_NAME_LEN-1 == 15글자.
			성공시 ET_OK가 리턴된다.
			이미 있는 이름을 쓰면 ET_DUPLICATED_NAME이 온다.
		*/
		const wchar_t name[MAX_NAME_LEN] = L"와츄고나두";
	
		error = network.SubmitName(name);
		if (error == ET_DUPLICATED_NAME)
		{
			puts("이미 존재하는 이름입니다.");
			return;
		}

		/*
			** 게임 시작 대기
			PKT_SC_START_GAME 패킷을 기다린다.
		*/
		puts("게임 시작 대기중");
		network.WaitSpecPacket(PKT_SC_GAME_START);

		/*
			** 게임 시작
			맵 제출부터 게임 종료까지 n회 반복한다.
			하나의 게임이 끝나고 다음 게임을 시작해야 한다면 PKT_SC_NEXT_GAME 패킷이
			모든 게임이 끝나면 PKT_SC_ALL_OVER 패킷이 들어온다.
		*/
		bool allOver = false;
		while (!allOver)
		{
			/*
				** 맵 제출
				자신이 배치한 맵 데이터를 서버로 전송한다.
				맵 데이터는 char형 32크기 배열이다.
				Aircraft부터 순서대로 배의 좌표들을 넣는다.
			*/
			Network::MapData mapData;

			while (true)
			{
				MakeMapData(&mapData);

				error = network.SubmitMap(&mapData);
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
						int x, y;
						MakeAttackPos(&x, &y);
						error = network.SubmitAttack(x, y);
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
					Network::AttackResult attackResult;
					network.GetAttackResult(&attackResult);
					if (attackResult.isMine)
						puts("공격 결과:");
					else
						puts("피격 결과:");
					printf_s("X: %d , Y: %d , RESULT: %s\n", attackResult.x, attackResult.y, ATTACK_RESULT_STR[attackResult.attackResult]);
					HandleOpositionAttackResult(attackResult.attackResult, attackResult.x, attackResult.y);
					break;
				}

					// 게임 종료
				case PKT_SC_GAME_OVER:
				{
					Network::GameResult gameResult;
					network.GetGameResult(&gameResult);
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
				Network::FinalResult finalResult;
				network.GetFinalResult(&finalResult);
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
}