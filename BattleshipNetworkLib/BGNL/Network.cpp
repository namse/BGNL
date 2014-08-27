#include <WinSock2.h>
#include <Windows.h>
#include <crtdbg.h>

#include "Network.h"

const int Network::NAME_LEN = 16;
const int Network::MAP_SIZE = 8 * 8;

Network::Network()
:m_Connected(false)
{
	ZeroMemory(&m_WSAData, sizeof(m_WSAData));
	ZeroMemory(&m_Socket, sizeof(m_Socket));
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

	return Send((void*)name, sizeof(wchar_t) * NAME_LEN);
}

Network::NetworkResult Network::SubmitMap(const char* const mapData)
{
	_ASSERT(mapData);
	if (!mapData || !m_Connected) return NETWORK_ERROR;

	return Send((void*)mapData, sizeof(char)* MAP_SIZE);
}

Network::NetworkResult Network::SubmitAttack(const int x, const int y)
{
	if (!m_Connected) return NETWORK_ERROR;
	int pos[2] = { x, y };

	return Send((void*)pos, sizeof(pos));
}


// Recive 계열
Network::NetworkResult Network::GetPacketType(PacketType* const type)
{
	_ASSERT(type);
	if (!type || !m_Connected) return NETWORK_ERROR;

	return Recive((void*)type, sizeof(type));
}

Network::NetworkResult Network::GetAttackResult(AttackResultPacket* const data)
{
	_ASSERT(data);
	if (!data || !m_Connected) return NETWORK_ERROR;

	return Recive((void*)data, sizeof(data));
}

Network::NetworkResult Network::GetGameResult(GameResultPacket* const data)
{
	_ASSERT(data);
	if (!data || !m_Connected) return NETWORK_ERROR;

	return Recive((void*)data, sizeof(data));
}

Network::NetworkResult Network::GetFinalResult(GetFinalResultPacket* const data)
{
	_ASSERT(data);
	if (!data && !m_Connected) return NETWORK_ERROR;

	return Recive((void*)data, sizeof(data));
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
		else
			recivedSize += result;
	}

	return NETWORK_OK;
}
