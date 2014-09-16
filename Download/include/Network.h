#pragma once

#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h>
#include "PacketType.h"

typedef short PacketType;
typedef short ErrorType;
typedef short AttackResult;

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
	struct GameStartData
	{
		wchar_t oppositionName[MAX_NAME_LEN];
		int oppositionStudentID;
	};

	struct AttackResultData
	{
		Coord pos;
		AttackResult attackResult;
		bool isMine;
	};

	struct GameResultData
	{
		bool isWinner;
		int turns;
	};

	struct FinalResultData
	{
		int winCount;
		float avgTurns;
	};

public:
	Network();
	~Network();

	static void			Initialize();

	void				Connect(const char* const ip, const unsigned short port);
	void				Disconnect();

	// Send 계열
	ErrorType			SubmitName(const wchar_t* const name, const int studentID);
	ErrorType			SubmitMap(const char* const mapData);
	ErrorType			SubmitAttack(const Coord pos);

	// Recive 계열
	ErrorType			GetPacketType(PacketType* const type);
	ErrorType			WaitSpecPacket(const PacketType type);

	void				WaitForStart(GameStartData* const data);
	AttackResultData	GetAttackResult();
	GameResultData		GetGameResult();
	FinalResultData		GetFinalResult();

	// 범용
	void				Send(const void* const data, const unsigned int size);
	void				Recive(void* const out_data, const unsigned int size);

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

