#include <WS2tcpip.h>
#include <Windows.h>
#include <crtdbg.h>

#include "Network.h"


Network::Network()
:m_Socket(INVALID_SOCKET), m_Connected(false)
{
	// ZeroMemory(&m_WSAData, sizeof(m_WSAData));
	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
}


Network::~Network()
{
	if (m_Connected)
		Disconnect();
}


void Network::Initialize()
{
	static WinSockIntializer winSockInit;
}



void Network::Connect(const char* const ip, const unsigned short port)
{
	_ASSERT(ip);
	if (!ip) throw PARAMETER_ERROR;

	/*
	if (WSAStartup(MAKEWORD(2, 2), &m_WSAData))
	{
		throw NETWORK_ERROR;
	}
	*/

	ZeroMemory(&m_Socket, sizeof(m_Socket));
	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));

	int result;

	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket == INVALID_SOCKET)
	{
		throw NETWORK_ERROR;
	}

	m_ServerAddr.sin_family = AF_INET;
//	m_ServerAddr.sin_addr.s_addr = inet_addr(ip);
	InetPton(AF_INET, ip, &m_ServerAddr.sin_addr.s_addr);
	m_ServerAddr.sin_port = htons(port);
	
	result = connect(m_Socket, (sockaddr*)&m_ServerAddr, sizeof(m_ServerAddr));
	if (result == SOCKET_ERROR)
	{
		throw NETWORK_ERROR;
	}

	m_Connected = true;

	
}

void Network::Disconnect()
{
	if (!m_Connected)
		return;

	closesocket(m_Socket);
	m_Connected = false;

	// WSACleanup();
}


// Send 계열
ErrorType Network::SubmitName(const wchar_t* const name)
{
	_ASSERT(name);
	if (!name) throw PARAMETER_ERROR;
	if (!m_Connected) throw NETWORK_ERROR;

	Packet::SubmitNameRequest packet;
	wcscpy_s(packet.mName, name);

	Send(&packet, sizeof(packet));

	return WaitSpecPacket(PKT_SC_OK);
}

ErrorType Network::SubmitMap(const void* const mapData)
{
	_ASSERT(mapData);
	if (!mapData) throw PARAMETER_ERROR;
	if (!m_Connected) throw NETWORK_ERROR;

	Packet::SubmitMapRequest packet;
	memcpy_s(packet.mCoords, SHNIPS_TOTAL_LENGTH, mapData, SHNIPS_TOTAL_LENGTH);

	Send(&packet, sizeof(packet));

	return WaitSpecPacket(PKT_SC_OK);
}

ErrorType Network::SubmitAttack(const int x, const int y)
{
	if (!m_Connected) throw NETWORK_ERROR;
	int pos[2] = { x, y };

	Packet::SubmitAttackRequest packet;
	packet.x = x;
	packet.y = y;

	Send(&packet, sizeof(packet));

	return WaitSpecPacket(PKT_SC_OK);
}


// Recive 계열
ErrorType Network::GetPacketType(PacketType* const type)
{
	if (!m_Connected) throw NETWORK_ERROR;
	
	PacketHeader header;
	Recive(&header, sizeof(PacketHeader));

	*type = header.mType;
	if (*type == PKT_SC_ERROR)
	{
		ErrorType error;
		Recive(&error, sizeof(ErrorType));
		return error;
	}
	else
		return ET_OK;
}

ErrorType Network::WaitSpecPacket(const PacketType type)
{
	if (!m_Connected) throw NETWORK_ERROR;

	PacketType responce;
	ErrorType error;
	error = GetPacketType(&responce);

	if (responce == type)
		return ET_OK;
	else if (responce == PKT_SC_ERROR)
		return error;
	else
		throw UNEXPECTED_PACKET;
}

void Network::GetAttackResult(AttackResult* const data)
{
	_ASSERT(data);
	if (!data) throw PARAMETER_ERROR;
	if (!m_Connected) throw NETWORK_ERROR;

	Packet::AttackResult packet;
	Recive((char*)&packet + sizeof(PacketHeader), sizeof(packet) - sizeof(PacketHeader));
	data->attackResult = packet.mAttackResult;
	data->x = packet.x;
	data->y = packet.y;
	data->isMine = packet.mIsMine;
}

void Network::GetGameResult(GameResult* const data)
{
	_ASSERT(data);
	if (!data) throw PARAMETER_ERROR;
	if (!m_Connected) throw NETWORK_ERROR;

	Packet::GameOverResult packet;
	Recive((char*)&packet + sizeof(PacketHeader), sizeof(packet)-sizeof(PacketHeader));
	data->isWinner = packet.mIsWinner;
	data->turns = packet.mTurns;
}

void Network::GetFinalResult(FinalResult* const data)
{
	_ASSERT(data);
	if (!data) throw PARAMETER_ERROR;
	if (!m_Connected) throw NETWORK_ERROR;

	Packet::AllOverResult packet;
	Recive((char*)&packet + sizeof(PacketHeader), sizeof(packet)-sizeof(PacketHeader));
	data->winCount = packet.mWinCount;
	data->avgTurns = packet.mAverageTruns;
}



void Network::Send(const void* const data, const unsigned int size)
{
	_ASSERT(data);
	if (!data) throw PARAMETER_ERROR;
	if (!m_Connected) throw NETWORK_ERROR;

	unsigned int sentSize = 0;
	int result;

	while (sentSize < size)
	{
		result = send(m_Socket, (char*)data + sentSize, size - sentSize, 0);
		if (result == SOCKET_ERROR)
		{
			throw NETWORK_ERROR;
		}
		else
			sentSize += result;
	}
}

void Network::Recive(void* const out_data, const unsigned int size)
{
	_ASSERT(out_data);
	if (!out_data) throw PARAMETER_ERROR;
	if (!m_Connected) throw NETWORK_ERROR;

	unsigned int recivedSize = 0;
	int result;

	while (recivedSize < size)
	{
		result = recv(m_Socket, (char*)out_data + recivedSize, size - recivedSize, 0);
		if (result == SOCKET_ERROR)
		{
			throw NETWORK_ERROR;
		}
		else if (recv == 0)
		{
			throw SERVER_CLOSED;
		}
		else
			recivedSize += result;
	}
}
