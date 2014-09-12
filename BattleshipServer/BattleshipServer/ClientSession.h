#pragma once

#include <map>
#include <WinSock2.h>
#include "Config.h"
#include "CircularBuffer.h"
#include "ObjectPool.h"
#include "EventListener.h"

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

class ClientSession
	: public ObjectPool<ClientSession>, 
	public EventListener
{
public:
	ClientSession(SOCKET sock);
	~ClientSession();


	
	void	OnRead(size_t len) ;
	void	OnWriteComplete(size_t len) ;

	bool	OnConnect(SOCKADDR_IN* addr) ;
	
	bool	PostRecv() ;

	bool	SendRequest(PacketHeader* pkt) ;
	bool	Broadcast(PacketHeader* pkt) ;

	void	Disconnect() ;

	bool	IsConnected() const { return mConnected ; }

	void	DatabaseJobDone(DatabaseJobContext* result) ;


	/// 현재 Send/Recv 요청 중인 상태인지 검사하기 위함
	void	IncOverlappedRequest()		{ ++mOverlappedRequested ; }
	void	DecOverlappedRequest()		{ --mOverlappedRequested ; }
	bool	DoingOverlappedOperation() const { return mOverlappedRequested > 0 ; }


	void Notify(EventHeader* event);
public:

	void HandleSubmitNameRequest(Packet::SubmitNameRequest& inPacket);
	void HandleSubmitMapRequest(Packet::SubmitMapRequest& inPacket);
	void HandleSubmitAttackRequest(Packet::SubmitAttackRequest& inPacket);
/*
	void	HandleLoginRequest(LoginRequest& inPacket);
	void	HandleChatRequest(ChatBroadcastRequest& inPacket);*/
private:

	bool	SendFlush(); ///< Send요청 중인것들 모아서 보냄
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

	CircularBuffer	mSendBuffer;
	CircularBuffer	mRecvBuffer;

	OverlappedIO	mOverlappedSend ;
	OverlappedIO	mOverlappedRecv ;
	int				mOverlappedRequested ;

	int				mDbUpdateCount ; ///< DB에 주기적으로 업데이트 하기 위한 변수

	friend class ClientManager ;

} ;




void CALLBACK RecvCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
void CALLBACK SendCompletion(DWORD dwError, DWORD cbTransferred, LPWSAOVERLAPPED lpOverlapped, DWORD dwFlags) ;
