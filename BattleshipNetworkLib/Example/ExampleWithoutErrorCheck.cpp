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
	Network::PacketType type;

	/*
	** 서버에 연결
	서버의 IP와 포트는 당일날 공지된다.
	*/
	const char* ip = "127.0.0.1";
	const unsigned short port = 10000;

	network.Connect(ip, port);

	/*
	** 이름 전송
	최대 길이는 NAME_LEN-1 == 15글자.
	성공시 PT_OK가 온다.
	이미 있는 이름을 쓰면 PT_ERROR와 함께 ERROR_DUPLICATED_NAME이 온다.
	*/
	const wchar_t name[NAME_LEN] = L"와츄고나두";

	network.SubmitName(name);
	network.GetPacketType(&type);

	/*
	** 게임 시작 대기
	PT_START_GAME 패킷을 기다린다.
	*/
	network.GetPacketType(&type);

	/*
	** 게임 시작
	맵 제출부터 게임 종료까지 n회 반복한다.
	모든 게임이 끝나면 PT_ALL_OVER 패킷이 들어온다.
	*/
	bool allOver = false;
	while (!allOver)
	{
		/*
		** 맵 제출
		자신이 배치한 맵 데이터를 서버로 전송한다.
		맵 데이터는 char형 64크기 배열이다.
		맵 데이터에 들어가는 값은 MapData 열거형에 들어있다.
		*/
		char mapData[MAP_SIZE] = { 0, };

		MakeMapData(mapData);
		network.SubmitMap(mapData);
		network.GetPacketType(&type);

		/*
		** 게임 루프
		내 차례라면 공격 위치를 전송한다.
		차례가 끝나면 공격자와 공격 위치, 공격 결과를 받는다.
		한 게임이 끝나면 PT_GAME_OVER 패킷이 들어온다.
		*/
		bool gameOver = false;
		while (!gameOver)
		{
			network.GetPacketType(&type);

			switch (type)
			{
				// 내 차례
			case Network::PT_MY_TURN:
			{
				/*
				** 공격 위치 전송
				x, y는 0~7 사이의 정수이다.
				*/
				int x, y;
				MakeAttackPos(&x, &y);
				network.SubmitAttack(x, y);
				network.GetPacketType(&type);
				break;
			}
				// 공격 결과
			case Network::PT_ATTACK_RESULT:
			{
				Network::AttackResultPacket attackResult;
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
			case Network::PT_GAME_OVER:
			{
				Network::GameResultPacket gameResult;
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
		if (type == Network::PT_NEXT_GAME)
		{
			puts("다음 게임을 준비해주세요.");
		}
		else if (type == Network::PT_ALL_OVER)
		{
			Network::FinalResultPacket finalResult;
			network.GetFinalResult(&finalResult);
			puts("모두 종료");
			printf_s("승리 횟수: %d, 평균 턴 수: %d", finalResult.winCount, finalResult.avgTurns);

			allOver = true;
		}
	}


	/*
	** 연결 종료
	*/
	network.Disconnect();

}