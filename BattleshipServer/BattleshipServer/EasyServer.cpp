#include "stdafx.h"
#include "EasyServer.h"

#include "Config.h"

#include "ClientSession.h"
#include "ClientManager.h"
#pragma comment(lib,"ws2_32.lib")

EasyServer* EasyServer::pInstance_ = nullptr;


EasyServer::EasyServer()
{
	/// Manager Init
	GClientManager = new ClientManager ;

	InitAccepThread();
}
EasyServer::~EasyServer(){
	delete GClientManager;
	CloseHandle(hCAThread);
}




bool EasyServer::InitAccepThread()
{
	/// DB Thread
	DWORD dwThreadId ;
	hCAThread = (HANDLE)_beginthreadex(NULL, 0, ClientAcceptThread, NULL, 0, (unsigned int*)&dwThreadId);
	if (hCAThread == NULL)
		return false;
	return true;
}

bool EasyServer::Run()
{
	SOCKET acceptSock = NULL;

	/// 새로 접속한 클라이언트 처리
	if (pendingAcceptList.Consume(acceptSock, false))
	{
		/// 소켓 정보 구조체 할당과 초기화
		ClientSession* client = GClientManager->CreateClient(acceptSock);

		SOCKADDR_IN clientaddr;
		int addrlen = sizeof(clientaddr);
		getpeername(acceptSock, (SOCKADDR*)&clientaddr, &addrlen);

		// 클라 접속 처리
		if (false == client->OnConnect(&clientaddr))
		{
			client->Disconnect();
		}

//		return true; ///< 다시 대기로
	}	

	/// APC Queue에 쌓인 작업들 처리
	SleepEx(1, TRUE);

	GClientManager->OnPeriodWork();
	return true;
}

unsigned int WINAPI ClientAcceptThread(LPVOID lpParam)
{
	/// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET)
		return -1;

	int opt = 1;
	setsockopt(listenSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(int));

	/// bind
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(LISTEN_PORT);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret = bind(listenSocket, (SOCKADDR*)&serveraddr, sizeof(serveraddr));
	if (ret == SOCKET_ERROR)
		return -1;

	/// listen
	ret = listen(listenSocket, SOMAXCONN);
	if (ret == SOCKET_ERROR)
		return -1;

	

	

	/// accept loop
	while (true)
	{
		SOCKET acceptedSocket = accept(listenSocket, NULL, NULL);
		if (acceptedSocket == INVALID_SOCKET)
		{
			printf("accept: invalid socket\n");
			continue;
		}

		pendingAcceptList.Produce(acceptedSocket);
	}

	// 윈속 종료
	WSACleanup();

	return 0;
}