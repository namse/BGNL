#pragma once

#include <map>
#include <WinSock2.h>
#include "Config.h"
#include "CircularBuffer.h"
#include "ObjectPool.h"

#define BUFSIZE	(128)

class ClientSession ;
class ClientManager ;
struct DatabaseJobContext ;

struct OverlappedIO : public OVERLAPPED
{
	OverlappedIO() : mObject(nullptr)
	{}

	ClientSession* mObject ;
} ;

class ClientSession// : public ObjectPool<ClientSession>
{
public:
	ClientSession(SOCKET sock)
		: mConnected(false), mLogon(false), mSocket(sock), mPlayerId(sock), mOverlappedRequested(0)
		, mPosX(0), mPosY(0), mPosZ(0), mDbUpdateCount(0), recvLength(0)
	{
		memset(&mClientAddr, 0, sizeof(SOCKADDR_IN)) ;
		memset(mPlayerName, 0, sizeof(mPlayerName)) ;
	}
	~ClientSession()
	{
	}


	
	void	OnRead(size_t len) ;
	void	OnWriteComplete(size_t len) ;

	bool	OnConnect(SOCKADDR_IN* addr) ;
	
	bool	PostRecv() ;

	bool	SendRequest(PacketHeader* pkt) ;
	bool	Broadcast(PacketHeader* pkt) ;

	void	Disconnect() ;

	bool	IsConnected() const { return mConnected ; }

	void	DatabaseJobDone(DatabaseJobContext* result) ;


	/// ���� Send/Recv ��û ���� �������� �˻��ϱ� ����
	void	IncOverlappedRequest()		{ ++mOverlappedRequested ; }
	void	DecOverlappedRequest()		{ --mOverlappedRequested ; }
	bool	DoingOverlappedOperation() const { return mOverlappedRequested > 0 ; }

public:

	void HandleSubmitNameRequest(Packet::SubmitNameRequest& inPacket);
	void HandleSubmitMapRequest(Packet::SubmitMapRequest& inPacket);
	void HandleSubmitAttackRequest(Packet::SubmitAttackRequest& inPacket);
/*
	void	HandleLoginRequest(LoginRequest& inPacket);
	void	HandleChatRequest(ChatBroadcastRequest& inPacket);*/
private:

	bool	SendFlush(); ///< Send��û ���ΰ͵� ��Ƽ� ����
	void	OnTick() ;

	void	LoginDone(int pid, double x, double y, double z, const char* name) ;
	void	UpdateDone() ;


private:
	double			mPosX ;
	double			mPosY ;
	double			mPosZ ;
	char			mPlayerName[MAX_NAME_LEN] ;

private:
	bool			mConnected ;
	bool			mLogon ;
	SOCKET			mSocket ;

	PlayerNumber		mPlayerId ; // is Same with socket number.
	SOCKADDR_IN		mClientAddr ;


	OverlappedIO	mOverlappedSend ;
	OverlappedIO	mOverlappedRecv ;
	int				mOverlappedRequested ;

	int				mDbUpdateCount ; ///< DB�� �ֱ������� ������Ʈ �ϱ� ���� ����

	friend class ClientManager ;

	char buf[BUFSIZE];
	int recvLength;
} ;




void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
