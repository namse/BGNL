#pragma once

#define NAME_LEN 16
#define MAP_SIZE 64

class Network
{
public:
	enum NetworkResult
	{
		NETWORK_ERROR	= -1,
		NETWORK_OK		= 0,
		NETWORK_CLOSED			// Recive 계열 함수에서만 반환된다.
	};

	enum PacketType
	{
		PT_ERROR		= -1,
		PT_NONE			= 0,
		PT_OK,
		PT_SUBMIT_NAME,
		PT_GAME_START,
		PT_SUBMIT_MAP,
		PT_MY_TURN,
		PT_SUBMIT_ATTACK,
		PT_ATTACK_RESULT,
		PT_GAME_OVER,
		PT_NEXT_GAME,
		PT_ALL_OVER,
	};

	enum ErrorType
	{
		ERROR_NONE,
		ERROR_OPPOSITION_DISCONNECTED,
		ERROR_DUPLICATED_NAME,
		ERROR_INVALID_MAP,
		ERROR_INVALID_ATTACK,
	};

	enum AttackResult
	{
		RESULT_NONE,
		RESULT_MISS,
		RESULT_HIT,
		RESULT_DESTROY_A,
		RESULT_DESTROY_B,
		RESULT_DESTROY_C,
		RESULT_DESTROY_D,
	};

	enum MapData
	{
		MAP_NONE,
		MAP_SHIP_A,
		MAP_SHIP_B,
		MAP_SHIP_C,
		MAP_SHIP_D1,
		MAP_SHIP_D2
	};


public:

	struct AttackResultPacket
	{
		int x;
		int y;
		AttackResult attackResult;
		bool isMine;
	};

	struct GameResultPacket
	{
		bool isWinner;
		int turns;
	};

	struct FinalResultPacket
	{
		int winCount;
		int avgTurns;
	};


public:
	Network();
	~Network();

	NetworkResult	Connect(const char* const ip, const unsigned short port);
	void			Disconnect();

	// Send 계열
	NetworkResult	SubmitName(const wchar_t* const name);
	NetworkResult	SubmitMap(const char* const mapData);
	NetworkResult	SubmitAttack(const int x, const int y);

	// Recive 계열
	NetworkResult	GetPacketType(PacketType* const type);
	NetworkResult	GetErrorType(ErrorType* const error);
	NetworkResult	GetAttackResult(AttackResultPacket* const data);
	NetworkResult	GetGameResult(GameResultPacket* const data);
	NetworkResult	GetFinalResult(FinalResultPacket* const data);

	// 범용
	NetworkResult	Send(const void* const data, const unsigned int size);
	NetworkResult	Recive(void* const out_data, const unsigned int size);


private:
	WSADATA		m_WSAData;
	SOCKET		m_Socket;

	bool		m_Connected;
	SOCKADDR_IN m_ServerAddr;


};

