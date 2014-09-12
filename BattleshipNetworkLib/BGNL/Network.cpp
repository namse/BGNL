#include <WinSock2.h>
#include <Windows.h>
#include <crtdbg.h>

#include "Network.h"


Network::Network()
:m_Socket(INVALID_SOCKET), m_Connected(false)
{
	ZeroMemory(&m_WSAData, sizeof(m_WSAData));
	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));
}


Network::~Network()
{
	if (m_Connected)
		Disconnect();
}



Network::NetworkResult Network::Connect(const char* const ip, const unsigned short port)
{
	_ASSERT(ip);
	if (!ip) return NETWORK_ERROR;

	if (WSAStartup(MAKEWORD(2, 2), &m_WSAData))
	{
		return NETWORK_ERROR;
	}

	ZeroMemory(&m_Socket, sizeof(m_Socket));
	ZeroMemory(&m_ServerAddr, sizeof(m_ServerAddr));

	int result;

	m_Socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_Socket == INVALID_SOCKET)
	{
		return NETWORK_ERROR;
	}

	m_ServerAddr.sin_family = AF_INET;
	m_ServerAddr.sin_addr.s_addr = inet_addr(ip);
	m_ServerAddr.sin_port = htons(port);
	
	result = connect(m_Socket, (sockaddr*)&m_ServerAddr, sizeof(m_ServerAddr));
	if (result == SOCKET_ERROR)
	{
		return NETWORK_ERROR;
	}

	m_Connected = true;

	return NETWORK_OK;
}

void Network::Disconnect()
{
	if (!m_Connected)
		return;

	closesocket(m_Socket);
	m_Connected = false;

	WSACleanup();
}


// Send 계열
Network::NetworkResult Network::SubmitName(const wchar_t* const name)
{
	_ASSERT(name);
	if (!name || !m_Connected) return NETWORK_ERROR;

	Packet::SubmitNameRequest packet;
	wcscpy_s(packet.mName, name);

	return Send(&packet, sizeof(packet));
}

Network::NetworkResult Network::SubmitMap(const char* const mapData)
{
	_ASSERT(mapData);
	if (!mapData || !m_Connected) return NETWORK_ERROR;

	Packet::SubmitMapRequest packet;
	memcpy_s(packet.mMap, MAP_SIZE, mapData, MAP_SIZE);

	return Send(&packet, sizeof(packet));
}

Network::NetworkResult Network::SubmitAttack(const int x, const int y)
{
	if (!m_Connected) return NETWORK_ERROR;
	int pos[2] = { x, y };

	Packet::SubmitAttackRequest packet;
	packet.x = x;
	packet.y = y;

	return Send(&packet, sizeof(packet));
}


// Recive 계열
Network::NetworkResult Network::GetPacketType(short* const type)
{
	_ASSERT(type);
	if (!type || !m_Connected) return NETWORK_ERROR;
	
	PacketHeader header;
	NetworkResult result;

	result = Recive(&header, sizeof(PacketHeader));
	*type = header.mType;
	
	return result;
}

Network::NetworkResult Network::GetErrorType(short* const error)
{
	_ASSERT(error);
	if (!error || !m_Connected) return NETWORK_ERROR;
	return Recive(error, sizeof(short));
}

Network::NetworkResult Network::GetAttackResult(AttackResult* const data)
{
	_ASSERT(data);
	if (!data || !m_Connected) return NETWORK_ERROR;

	Packet::AttackResult packet;
	NetworkResult result;

	result = Recive((char*)&packet + sizeof(PacketHeader), sizeof(packet) - sizeof(PacketHeader));
	data->attackResult = packet.mAttackResult;
	data->x = packet.x;
	data->y = packet.y;
	data->isMine = packet.mIsMine;

	return result;
}

Network::NetworkResult Network::GetGameResult(GameResult* const data)
{
	_ASSERT(data);
	if (!data || !m_Connected) return NETWORK_ERROR;

	Packet::GameOverResult packet;
	NetworkResult result;

	result = Recive((char*)&packet + sizeof(PacketHeader), sizeof(packet)-sizeof(PacketHeader));
	data->isWinner = packet.mIsWinner;
	data->turns = packet.mTurns;

	return result;
}

Network::NetworkResult Network::GetFinalResult(FinalResult* const data)
{
	_ASSERT(data);
	if (!data && !m_Connected) return NETWORK_ERROR;

	Packet::AllOverResult packet;
	NetworkResult result;

	result = Recive((char*)&packet + sizeof(PacketHeader), sizeof(packet)-sizeof(PacketHeader));
	data->winCount = packet.mWinCount;
	data->avgTurns = packet.mAverageTruns;

	return result;
}



Network::NetworkResult Network::Send(const void* const data, const unsigned int size)
{
	_ASSERT(data);
	if (!data || !m_Connected) return NETWORK_ERROR;

	unsigned int sentSize = 0;
	int result;

	while (sentSize < size)
	{
		result = send(m_Socket, (char*)data + sentSize, size - sentSize, 0);
		if (result == SOCKET_ERROR)
		{
			return NETWORK_ERROR;
		}
		else
			sentSize += result;
	}

	return NETWORK_OK;
}

Network::NetworkResult Network::Recive(void* const out_data, const unsigned int size)
{
	_ASSERT(out_data);
	if (!out_data || !m_Connected) return NETWORK_ERROR;

	unsigned int recivedSize = 0;
	int result;

	while (recivedSize < size)
	{
		result = recv(m_Socket, (char*)out_data + recivedSize, size - recivedSize, 0);
		if (result == SOCKET_ERROR)
		{
			return NETWORK_ERROR;
		}
		else if (recv == 0)
		{
			return NETWORK_CLOSED;
		}
		else
			recivedSize += result;
	}

	return NETWORK_OK;
}
