#pragma once
#include "PacketType.h"

#define NAME_LEN 16
#define MAP_SIZE 32	// 16 * 2

class Network
{
public:
	enum NetworkResult
	{
		NETWORK_ERROR	= -1,
		NETWORK_OK		= 0,
		NETWORK_CLOSED			// Recive 계열 함수에서만 반환된다.
	};

public:
	struct AttackResult
	{
		int x;
		int y;
		short attackResult;
		bool isMine;
	};

	struct GameResult
	{
		bool isWinner;
		int turns;
	};

	struct FinalResult
	{
		int winCount;
		float avgTurns;
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
	NetworkResult	GetPacketType(short* const type);
	NetworkResult	GetErrorType(short* const error);
	NetworkResult	GetAttackResult(AttackResult* const data);
	NetworkResult	GetGameResult(GameResult* const data);
	NetworkResult	GetFinalResult(FinalResult* const data);

	// 범용
	NetworkResult	Send(const void* const data, const unsigned int size);
	NetworkResult	Recive(void* const out_data, const unsigned int size);


private:
	WSADATA		m_WSAData;
	SOCKET		m_Socket;

	bool		m_Connected;
	SOCKADDR_IN m_ServerAddr;


};

