/*
	Name: ExampleWithErrorCheck
	Desc: BGNL 사용 예시 코드입니다. 대다수의 에러 체크를 제거한 코드입니다.
		  이 코드에서는 흐름만 참고하고, 실제로 사용할 때는 에러를 체크해야합니다.
*/
#include <stdio.h>

#ifdef _DEBUG

// Debug로 빌드하는 경우
#pragma comment(lib, "BGNL_debug.lib")

#else

// Release로 빌드하는 경우
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
		** 서버에 연결
		서버의 IP와 포트는 당일날 공지된다.
	*/
	const char* ip = "127.0.0.1";
	const unsigned short port = 10000;

	network.Connect(ip, port);

	/*
		** 이름 전송
		최대 길이는 MAX_NAME_LEN-1 == 15글자.
	*/
	const wchar_t name[MAX_NAME_LEN] = L"와츄고나두";

	network.SubmitName(name);
	network.GetPacketType(&type);

	/*
		** 게임 시작 대기
		PKT_SC_GAME_START 패킷을 기다린다.
	*/
	network.GetPacketType(&type);

	/*
		** 게임 시작
		맵 제출부터 게임 종료까지 n회 반복한다.
		모든 게임이 끝나면 PKT_SC_ALL_OVER 패킷이 들어온다.
	*/
	bool allOver = false;
	while (!allOver)
	{
		/*
			** 맵 제출
			자신이 배치한 맵 데이터를 서버로 전송한다.
			맵 데이터는 char형 32크기 배열이다.
			맵 데이터에는 배들의 좌표가 순서대로 들어간다.
			좌표는 0~7인 정수 2개이며,
			Aircraft 5개, Battleship 4개, Cruiser 3개, Destroyer 2개의 좌표를 가진다.
			총 좌표는 16개이며 이는 곧 32개의 정수가 된다.
		*/
		char mapData[MAP_SIZE] = { 0, };

		MakeMapData(mapData);
		network.SubmitMap(mapData);
		network.GetPacketType(&type);

		/*
			** 게임 루프
			내 차례라면 공격 위치를 전송한다.
			차례가 끝나면 공격자와 공격 위치, 공격 결과를 받는다.
			한 게임이 끝나면 PKT_SC_GAME_OVER 패킷이 들어온다.
		*/
		bool gameOver = false;
		while (!gameOver)
		{
			network.GetPacketType(&type);

			switch (type)
			{
				// 내 차례
			case PKT_SC_MY_TURN:
			{
				/*
					** 공격 위치 전송
					x, y는 0~7 사이의 정수이다.
				*/
				int x, y;
				MakeAttackPos(&x, &y);
				network.SubmitAttack(x, y);
				network.GetPacketType(&type);	// OK를 기다리는 부분
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
					puts("배패...");
				printf_s("턴 수: %d\n", gameResult.turns);
				gameOver = true;
				break;
			}
			}
		}

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
	}


	/*
	** 연결 종료
	*/
	network.Disconnect();

}