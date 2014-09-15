#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include "PacketType.h"

typedef short PacketType;
typedef short ErrorType;

class Network
{
public:
	enum Exception
	{
		NETWORK_ERROR,			// 범용 Error
		SERVER_CLOSED,
		PARAMETER_ERROR,
		UNEXPECTED_PACKET,		// WaitSpecPacket함수만 발생하는 예외
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

	struct MapData
	{
		Coord aircraft[AIRCRAFT_LENGTH];
		Coord battleship[BATTLESHIP_LENGTH];
		Coord cruiser[CRUISER_LENGTH];
		Coord destroyer1[DESTROYER_LENGTH];
		Coord destroyer2[DESTROYER_LENGTH];
	};


public:
	Network();
	~Network();

	static void Initialize();
	void	Connect(const char* const ip, const unsigned short port);
	void	Disconnect();

	// Send 계열
	ErrorType	SubmitName(const wchar_t* const name);
	ErrorType	SubmitMap(const void* const mapData);
	ErrorType	SubmitAttack(const int x, const int y);

	// Recive 계열
	ErrorType	GetPacketType(PacketType* const type);
	ErrorType	WaitSpecPacket(const PacketType type);
	void	GetAttackResult(AttackResult* const data);
	void	GetGameResult(GameResult* const data);
	void	GetFinalResult(FinalResult* const data);

	// 범용
	void	Send(const void* const data, const unsigned int size);
	void	Recive(void* const out_data, const unsigned int size);

private:
	struct WinSockIntializer
	{
		WinSockIntializer()
		{
			WSADATA wsadata = { 0, };
			if (WSAStartup(MAKEWORD(2, 2), &wsadata))
			{
				throw NETWORK_ERROR;
			}
		}
		~WinSockIntializer()
		{
			WSACleanup();
		}
	};

private:
	// WSADATA		m_WSAData;
	SOCKET		m_Socket;

	bool		m_Connected;
	SOCKADDR_IN m_ServerAddr;

};

